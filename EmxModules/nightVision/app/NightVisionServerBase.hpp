/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionServerBase_HPP
#define EMX_NightVisionServerBase_HPP

#include "NightVision.hpp"

namespace Emx {

    class NightVisionServerBase : public NightVision {
    public:
        NightVisionServerBase() : NightVision() {}

        virtual ~NightVisionServerBase() {}

        using OnCreated = std::function<void()>;

        ErrCodeE Create(OnCreated cb, bool newThread);

        void Destroy() { DestroySub(); }

    protected:

        void Switch(ModeE mode);

        virtual ErrCodeE CreateSub(Json::Value &json) = 0;

        virtual void DestroySub() = 0;

        virtual void DoSwitch(ModeE mode) = 0;

        void PublishSwitching();

    private:

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParamTemp(Param &param) override;

        ErrCodeE SetParamTemp(const Param &param) override;

        ErrCodeE GetStatus(Status &status) override;

    protected:
        EuvLoop m_loop;
        EuvTimer m_switchTimer;
        MsgRpcRep m_resp;
        bool m_respConnected;
        MsgTopicPublisherAsync m_pub;
        bool m_pubConnected;
        char m_buffer[1024];
        Param m_running;
        Param m_param;
        Status m_status;
        int postSwitchTimeMs;
        OnCreated m_cb;
    };

}

#endif //EMX_NightVisionServerBase_HPP
