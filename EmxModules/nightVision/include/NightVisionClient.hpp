/*
* @Author: xiong
* @Date: 2022/10/12
*/

#ifndef EMX_NightVisionClient_HPP
#define EMX_NightVisionClient_HPP

#include "NightVision.hpp"

namespace Emx {

    class NightVisionClient : public NightVision {
    public:
        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParamTemp(const Param &param) override;

        ErrCodeE GetParamTemp(Param &param) override;

        ErrCodeE GetStatus(Status &status) override;
    };

    class NightVisionClientInfo {
    public:
        NightVisionClientInfo() : m_started(false) {}

        using OnGetInfoCallback = std::function<void(NightVision::Status &status)>;

        /**
         * @brief 开始接收NightVision消息数据
         * @param loop [in] loop
         * @param buffer [in] 提供一个buffer
         * @param bufferSize [in] 提供的buffer大小
         * @param cb [in] 获取到数据后的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Start(EuvLoop &loop, char *buffer, int bufferSize, OnGetInfoCallback cb);

        //!@brief 停止接收NightVision消息数据
        void Stop();

    protected:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        MsgTopicSubscriberAsync m_sub;
        OnGetInfoCallback m_cb;
        bool m_started;
    };

}

#endif //EMX_NightVisionClient_HPP
