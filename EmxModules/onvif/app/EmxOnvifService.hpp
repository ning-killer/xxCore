/*
* @Author: xiong
* @Date: 2023/1/12
*/

#ifndef EMX_EmxOnvifService_HPP
#define EMX_EmxOnvifService_HPP

#include "EmxCore.hpp"
#include "Onvif.hpp"

class OnvifService;
namespace Emx {

    class EmxOnvifService : public Onvif {
    public:
        EmxOnvifService() : Onvif() {}

        virtual ~EmxOnvifService() {}

        void Create(bool newThread);

        void Destroy();

        ErrCodeE LoadParam(Param &param);

        ErrCodeE SaveParam(const Param &param);

    private:
        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;

    private:
        ErrCodeE OnRecvMsg(uint32_t module, uint32_t method,
                           const char *data, int32_t size,
                           std::string &out);

    private:
        EuvLoop m_loop;
        char m_buffer[4096];
        MsgRpcRep m_resp;
        std::shared_ptr<OnvifService> m_onvif;
    };
}

#endif //EMX_EmxOnvifService_HPP
