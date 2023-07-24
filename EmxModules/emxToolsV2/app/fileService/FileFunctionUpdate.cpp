/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileFunctionUpdate.hpp"
#include "FileClient.hpp"

using namespace Emx;

FileFunctionUpdate::FileFunctionUpdate(FileClient *c) : FileFunction(c) {
    m_updateHandle.Create(2000);
    m_updateEvent.Create(&c->m_res->loop, c->m_res->buffer, c->m_res->bufferSize,
                         [this](UpdateEvent::Event &e) {
                             auto buffer = std::make_shared<std::string>();
                             buffer->append(m_c->m_header.magic, sizeof(m_c->m_header.magic));
                             auto cmd = (FileHeader::CmdE) htonl((uint32_t) m_c->m_header.cmd);
                             buffer->append((char *) &cmd, sizeof(cmd));
                             uint32_t msgSize = htonl(OutHeader::Size);
                             buffer->append((char *) &msgSize, sizeof(msgSize));
                             buffer->append((char *) &e.stat, sizeof(e.stat));
                             e.session = htonl(e.session);
                             buffer->append((char *) &e.session, sizeof(e.session));
                             e.download.total = htonl(e.download.total);
                             buffer->append((char *) &e.download.total, sizeof(e.download.total));
                             e.download.current = htonl(e.download.current);
                             buffer->append((char *) &e.download.current, sizeof(e.download.current));
                             e.burn.total = htonl(e.burn.total);
                             buffer->append((char *) &e.burn.total, sizeof(e.burn.total));
                             e.burn.current = htonl(e.burn.current);
                             buffer->append((char *) &e.burn.current, sizeof(e.burn.current));
                             if (m_c->Send(buffer) != ErrCodeE::Success) {
                                 emxloge("send:%d failed\n", (int) buffer->size());
                                 m_c->GetHost()->DisConnectClient(m_c);
                             }
                         });
}

FileFunctionUpdate::~FileFunctionUpdate() {
    m_updateHandle.Destroy();
    m_updateEvent.Destroy();
}

void FileFunctionUpdate::OnGetData(char *data, int size) {
    if (m_updateHandle.Push((uint8_t *) data, size) != ErrCodeE::Success) {
        emxloge("update failed");
        m_c->GetHost()->DisConnectClient(m_c);
    }
}
