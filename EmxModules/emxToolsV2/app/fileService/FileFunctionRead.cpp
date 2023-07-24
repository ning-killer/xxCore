/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileFunctionRead.hpp"
#include "FileClient.hpp"

using namespace Emx;


FileFunctionRead::FileFunctionRead(FileClient *c) : FileFunction(c) {
    m_waitHeader = true;
    memset(&m_inHeader, 0, sizeof(m_inHeader));
    m_fp = nullptr;
}

FileFunctionRead::~FileFunctionRead() {
    if (m_fp)
        fclose(m_fp);
}

void FileFunctionRead::OnGetData(char *data, int size) {
    if (!m_waitHeader)
        return;
    OutHeader out = {};
    m_data.append(data, size);
    if (m_data.size() < InHeader::Size)
        return;
    m_waitHeader = false;
    memcpy(m_inHeader.path, m_data.data(), sizeof(m_inHeader.path));
    emxlogd("read file path[%.*s]\n", (int) sizeof(m_inHeader.path), m_inHeader.path);
    m_fp = fopen(m_inHeader.path, "rb");
    if (!m_fp) {
        emxloge("cannot open %s\n", m_inHeader.path);
        out.code = (uint32_t) CodeE::OpenFailed;
        goto Final;
    } else {
        int n;
        while ((n = (int) fread(m_c->m_res->buffer, 1, m_c->m_res->bufferSize, m_fp)) > 0) {
            if (n < 0) {
                emxloge("read %s failed\n", m_inHeader.path);
                out.code = (uint32_t) CodeE::ReadFailed;
                goto Final;
            }
            m_md5.update(m_c->m_res->buffer, n);
            out.fileSize += n;
        }
        fseek(m_fp, 0, SEEK_SET);
        m_md5.finalize();
        auto actualMd5 = m_md5.hexdigest();
        memcpy(out.md5, actualMd5.data(), sizeof(out.md5));
    }

    Final:
    auto buffer = std::make_shared<std::string>();
    buffer->append(m_c->m_header.magic, sizeof(m_c->m_header.magic));
    auto cmd = (FileHeader::CmdE) htonl((uint32_t) m_c->m_header.cmd);
    buffer->append((char *) &cmd, sizeof(cmd));
    uint32_t msgSize = htonl(OutHeader::Size + out.fileSize);
    buffer->append((char *) &msgSize, sizeof(msgSize));
    out.fileSize = htonl(out.fileSize);
    buffer->append((char *) &out.fileSize, sizeof(out.fileSize));
    buffer->append(out.md5, sizeof(out.md5));
    out.code = htonl(out.code);
    buffer->append((char *) &out.code, sizeof(out.code));
    if (m_c->Send(buffer) != ErrCodeE::Success) {
        emxloge("send:%d failed\n", (int) buffer->size());
        m_c->GetHost()->DisConnectClient(m_c);
    }
}

void FileFunctionRead::OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {
    if (e != ErrCodeE::Success) {
        emxloge("send:%d failed\n", size);
        return;
    }
    auto buf = std::make_shared<std::string>();
    auto n = (int) fread(m_c->m_res->buffer, 1, m_c->m_res->bufferSize, m_fp);
    if (n >= 0) {
        buf->assign(m_c->m_res->buffer, n);
        if (m_c->Send(buf) != ErrCodeE::Success) {
            emxloge("send:%d failed\n", (int) buf->size());
            m_c->GetHost()->DisConnectClient(m_c);
        }
    }
}
