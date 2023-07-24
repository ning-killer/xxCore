/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventServer_HPP
#define EMX_EventServer_HPP

#include "EventResource.hpp"
#include "EventGlobal.hpp"

namespace Emx {

    class EventServer : public EventGlobal {
    public:
        EventServer() {}

        virtual ~EventServer() {}

        void Create(bool newThread);

        void Destroy();

    private:
        ErrCodeE GetSupportedEventChannel(std::vector<Event::Channel> &channels) override;

        ErrCodeE GetParam(std::shared_ptr<Param> &param) override;

        ErrCodeE SetParam(const std::shared_ptr<Param> &param) override;

    private:

        ErrCodeE ParseConfig();

        void CreateAllPlugin();

        void DestroyAllPlugin();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

    private:
        EventResource m_res;
        MsgRpcRep m_resp;
        std::shared_ptr<Param> m_param;
        std::vector<Event::Channel> m_events;
        Schedule m_schedule;
    };

}

#endif //EMX_EventServer_HPP
