/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileFunctionWrite.hpp"
#include "FileClient.hpp"

using namespace Emx;

FileFunctionWrite::FileFunctionWrite(FileClient *c) : FileFunction(c) {
    m_currentSize = 0;
    m_waitHeader = true;
    memset(&m_inHeader, 0, sizeof(m_inHeader));
    m_fp = nullptr;
}

FileFunctionWrite::~FileFunctionWrite() {
    if (m_fp)
        fclose(m_fp);
}

void FileFunctionWrite::OnGetData(char *data, int size) {
    size_t n;
    m_currentSize += size;
    CodeE code = CodeE::Writing;
    if (m_waitHeader) {
        m_data.append(data, size);
        if (m_data.size() >= InHeader::Size) {
            m_waitHeader = false;
            memcpy(m_inHeader.md5, m_data.data(), sizeof(m_inHeader.md5));
            memcpy(m_inHeader.path, m_data.data() + sizeof(m_inHeader.md5), sizeof(m_inHeader.path));
            emxlogd("write file md5[%.*s] path[%.*s]\n",
                   (int) sizeof(m_inHeader.md5), m_inHeader.md5,
                   (int) sizeof(m_inHeader.path), m_inHeader.path);
            m_fp = fopen(m_inHeader.path, "wb+");
            if (!m_fp) {
                emxloge("cannot open %s\n", m_inHeader.path);
                code = CodeE::OpenPathFailed;
                goto Final;
            } else {
                data = (char *) m_data.data() + InHeader::Size;
                size = (int) m_data.size() - InHeader::Size;
            }
        }
    }
    m_md5.update(data, size);
    n = fwrite(data, sizeof(char), size, m_fp);
    if (n != (size_t)size) {
        emxloge("write %s failed, write len:%d, src size:%d\n", m_inHeader.path, n, size);
        code = CodeE::WriteFailed;
        goto Final;
    }
    if (m_currentSize == m_c->m_header.size) {
        m_md5.finalize();
        auto actualMd5 = m_md5.hexdigest();
        if (strncmp(actualMd5.c_str(), m_inHeader.md5, sizeof(m_inHeader.md5)) != 0) {
            code = CodeE::Md5MissMatch;
            emxloge("md5 miss match except[%.*s],actual[%s]\n",
                   sizeof(m_inHeader.md5), m_inHeader.md5, actualMd5.c_str());
        } else {
            fflush(m_fp);
            fsync(fileno(m_fp));
            fclose(m_fp);
            m_fp = nullptr;
            code = CodeE::Success;
        }
        emxlogd("file write ok, md5 match succ!\n");
    }
    Final:
    OutHeader out = {};
    auto buffer = std::make_shared<std::string>();
    buffer->append(m_c->m_header.magic, sizeof(m_c->m_header.magic));
    auto cmd = (FileHeader::CmdE) htonl((uint32_t) m_c->m_header.cmd);
    buffer->append((char *) &cmd, sizeof(cmd));
    uint32_t msgSize = htonl(OutHeader::Size);
    buffer->append((char *) &msgSize, sizeof(msgSize));
    out.totalSize = htonl(m_c->m_header.size);
    buffer->append((char *) &out.totalSize, sizeof(out.totalSize));
    out.receivedSize = htonl(m_currentSize);
    buffer->append((char *) &out.receivedSize, sizeof(out.receivedSize));
    out.code = htonl(out.code);

    buffer->append((char *) &out.code, sizeof(out.code));
    if (m_c->Send(buffer) != ErrCodeE::Success || code != CodeE::Writing) {
        emxlogd("receivedSize:%d, totalSize:%d\n", m_currentSize, m_c->m_header.size);
        m_c->GetHost()->DisConnectClient(m_c);
    }
}
