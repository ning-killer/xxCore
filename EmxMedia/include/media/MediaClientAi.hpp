//
// Created by xiong on 2022/5/12.
//

#ifndef EMX_MediaClientAi_HPP
#define EMX_MediaClientAi_HPP

#include <functional>
#include "core/EmxMsg.hpp"
#include "MediaRingBuffer.hpp"
#include "MediaTypedef.hpp"
#include "MediaAi.hpp"

namespace Emx {
    class MediaClientAi : public MediaAi {
    public:
        MediaClientAi(int32_t chn) : MediaAi(chn) {}

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE GetParamRange(Json::Value &range) override;

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @return ErrCode
         */
        static ErrCodeE GetChnNum(int &num);
    };

    //!@brief AI数据接收基类
    class MediaClientAiData {
    public:
        MediaClientAiData() {}

        virtual ~MediaClientAiData() {}

        /*!
         * @brief 仅接收到AI消息头信息的回调函数(未去接收实际消息数据)
         * @param header [in] 消息头信息
         */
        using OnGetInfoCallback = std::function<void(MediaAi::AiInfoHeader &header)>;

        /*!
         * @brief 接收到AI消息头以及消息数据的回调函数
         * @param e [in] 错误信息
         * @param header [in] 消息头信息
         */
        using OnGetDataCallback = std::function<void(Emx::ErrCodeE e, MediaAi::AiInfoHeader &header)>;

        /**
         * @brief 通过OnGetInfoCallback获取到header之后可通过此函数接收实际的消息数据
         * @param data [in,out] AI消息头信息
         * @return ErrCodeE
         */
        ErrCodeE GetDataByInfo(MediaAi::AiInfoHeader &data);

    protected:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    protected:
        OnGetInfoCallback m_cbInfo;
        OnGetDataCallback m_cbData;
        MediaRingBufferReader m_ring;
        std::string m_data;
    };

    //!@brief 异步AI数据接收
    class MediaClientAiDataAsync : public MediaClientAiData {
    public:
        MediaClientAiDataAsync(EuvLoop &loop) : MediaClientAiData(), m_loop(loop) {}

        virtual ~MediaClientAiDataAsync() {}

        /**
         * @brief 开始接收AI消息数据
         * @param cb [in] 获取到数据后的回调函数,如不需要接收实际数据，可填nullptr
         * @param cbInfo [in] 仅获取到消息头信息的回调函数(可选)
         * @return ErrCodeE
         */
        ErrCodeE Start(OnGetDataCallback cb, OnGetInfoCallback cbInfo = nullptr);

        //!@brief 停止接收AI消息数据
        void Stop();

        //!@brief 暂停接收AI消息数据
        void RecvFramePause() { m_sub.PauseRecv(); }

        //!@brief 恢复接收AI消息数据
        void RecvFrameResume() { m_sub.ResumeRecv(); }

        //!@brief 是否正在接收AI消息数据
        bool IsReceivingFrame() { return m_sub.IsReceiving(); }

    private:
        EuvLoop &m_loop;
        MsgTopicSubscriberAsync m_sub;
        char m_buffer[256];
    };

    //!@brief 同步AI数据接收
    class MediaClientAiDataSync : public MediaClientAiData {
    public:
        MediaClientAiDataSync() : MediaClientAiData() {}

        /**
         * @brief 打开接收端口
         * @param timeoutMs [in] 打开操作超时时间
         * @return ErrCodeE
         */
        ErrCodeE Open(int32_t timeoutMs);

        //!@brief 关闭接收端口
        void Close();

        /**
         * @brief 获取数据
         * @param data [out] 获取到的数据
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE GetData(MediaAi::AiInfoHeader &data, int32_t timeoutMs);

        /**
         * @brief 获取消息头信息
         * @param data [out] 获取到的消息头
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE GetInfo(MediaAi::AiInfoHeader &data, int32_t timeoutMs);

    private:
        MsgTopicSubscriberSync m_sub;
    };
}
#endif //EMX_MediaClientAi_HPP

