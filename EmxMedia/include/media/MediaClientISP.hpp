//
// Created by xiong on 2021/6/1.
//

#ifndef EMX_MediaClientISP_HPP
#define EMX_MediaClientISP_HPP

#include "MediaISP.hpp"

namespace Emx {

    class MediaClientISP : public MediaISP {
    public:
        MediaClientISP(int32_t chn) : MediaISP(chn) {}

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE GetParamRange(Json::Value &range) override;

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @return ErrCode
         */
        static ErrCodeE GetChnNum(int &num);

        ErrCodeE SetRunMode(RunModeE mode) override;

        ErrCodeE GetRunMode(RunModeE &mode) override;
    };

    //!@brief 异步ISPInfo数据接收
    class MediaClientISPInfo {
    public:
        MediaClientISPInfo() : m_started(false) {}


        //!@brief 异步ISP信息接收回调函数
        using OnGetInfoCallback = std::function<void(MediaISP::Info &info)>;

        /**
         * @brief 开始接收ISP消息数据
         * @param loop [in] loop
         * @param buffer [in] 提供一个buffer
         * @param bufferSize [in] 提供的buffer大小
         * @param cb [in] 获取到数据后的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Start(EuvLoop &loop, char *buffer, int bufferSize, OnGetInfoCallback cb);

        //!@brief 停止接收ISP消息数据
        void Stop();

    protected:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        MsgTopicSubscriberAsync m_sub;
        OnGetInfoCallback m_cb;
        bool m_started;
    };

}

#endif //EMX_MediaClientISP_HPP
