/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileClient_HPP
#define EMX_FileClient_HPP

#include "FileHeader.hpp"
#include "Resource.hpp"

namespace Emx {
    class FileFunction;

    class FileClient : public EuvTcpServerClient {
    public:
        FileClient(EuvStmServerHostBase *host, Resource *res) :
                EuvTcpServerClient(host),
                m_res(res),
                m_waitHeader(true),
                m_function(nullptr) {}

        virtual ~FileClient();

        void SetClientIp(const char *ip) {
            strncpy(m_ip, ip, sizeof(m_ip));
        }

        const char *GetClientIp() { return m_ip; }

        void OnRecvData(char *data, int32_t size);

        void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override;

    public:
        FileHeader m_header;
        Resource *m_res;
    private:
        bool m_waitHeader;
        FileFunction *m_function;
        std::string m_data;
        char m_ip[16];
    };

}

#endif //EMX_FileClient_HPP
