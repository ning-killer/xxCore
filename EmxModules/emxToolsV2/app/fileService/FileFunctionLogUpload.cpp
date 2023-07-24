/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileFunctionLogUpload.hpp"
#include "FileClient.hpp"

using namespace Emx;

FileFunctionLogUpload::FileFunctionLogUpload(FileClient *c) : FileFunction(c) {
    m_fp = nullptr;
}

FileFunctionLogUpload::~FileFunctionLogUpload() {
    if (m_fp)
        fclose(m_fp);
    emxLog.Resume();
}


void FileFunctionLogUpload::OnGetData(char *data, int size) {
    OutHeader out = {};
    Log::Config cfg = {};
    if(emxLog.GetConfig(cfg) != ErrCodeE::Success){
        emxloge("cannot get log cfg %s\n");
        out.code = (uint32_t) CodeE::OpenFailed;
        goto Final;
    }
    emxLog.Pause();
    m_fp = fopen(cfg.normal.path, "rb");
    if (!m_fp) {
        emxloge("cannot open %s\n", cfg.normal.path);
        out.code = (uint32_t) CodeE::OpenFailed;
        goto Final;
    } else {
        out.fileSize = File::Size(cfg.normal.path);
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
    out.code = htonl(out.code);
    buffer->append((char *) &out.code, sizeof(out.code));
    if (m_c->Send(buffer) != ErrCodeE::Success) {
        emxloge("send:%d failed\n", (int) buffer->size());
        m_c->GetHost()->DisConnectClient(m_c);
    }
}

void FileFunctionLogUpload::OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {
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

