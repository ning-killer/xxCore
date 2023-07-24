/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileClient.hpp"
#include "FileFunction.hpp"

using namespace Emx;

void FileClient::OnRecvData(char *data, int32_t size) {
    if (m_waitHeader) {
        m_data.append(data, size);
        if (m_data.size() < FileHeader::HeaderSize)
            return;
        m_waitHeader = false;
        //协议头接收完整，开始解析
        int off = 0;
        memcpy(&m_header.magic, m_data.data() + off, sizeof(m_header.magic));
        off += sizeof(m_header.magic);
        memcpy(&m_header.cmd, m_data.data() + off, sizeof(m_header.cmd));
        off += sizeof(m_header.cmd);
        memcpy(&m_header.size, m_data.data() + off, sizeof(m_header.size));
        off += sizeof(m_header.size);
        m_header.cmd = (FileHeader::CmdE) ntohl((uint32_t) m_header.cmd);
        m_header.size = ntohl(m_header.size);
        auto magic = m_header.magic;
        if (magic[0] != 'E' || magic[1] != 'P' || magic[2] != 'F' || magic[3] != 'P') {
            emxloge("client[%s] magic miss match [%c,%c,%c,%c]\n",
                   m_ip, magic[0], magic[1], magic[2], magic[3]);
            GetHost()->DisConnectClient(this);
            return;
        }
        m_function = FileFunctionFactory::Make(m_header.cmd, this);
        if (!m_function) {
            GetHost()->DisConnectClient(this);
            return;
        }
        if ((int) m_data.size() - FileHeader::HeaderSize > 0) {
            m_function->OnGetData((char *) m_data.data() + FileHeader::HeaderSize,
                                  (int) m_data.size() - FileHeader::HeaderSize);
        }
        m_data.clear();
    } else {
        m_function->OnGetData(data, size);
    }
}

FileClient::~FileClient() {
    delete m_function;
}

void FileClient::OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {
    m_function->OnSendDone(e, buffer, size);
}
