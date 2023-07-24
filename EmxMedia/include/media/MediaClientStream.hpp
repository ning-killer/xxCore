//
// Created by xiong on 2021/8/6.
//

#ifndef EMX_MediaClientStream_HPP
#define EMX_MediaClientStream_HPP

#include <functional>
#include "core/EmxMsg.hpp"
#include "MediaRingBuffer.hpp"
#include "MediaTypedef.hpp"

namespace Emx {

    //!@brief MediaFrame数据接收基类
    class MediaClientStream {
    public:
        MediaClientStream() {}

        virtual ~MediaClientStream() {}

        /*!
         * @brief 接收到MediaFrame帧数据的回调函数
         * @param e [in] 错误信息
         * @param frame [in] 媒体帧数据
         */
        using OnGetFrameCallback = std::function<void(Emx::ErrCodeE e, MediaFrame &frame)>;

        /*!
         * @brief 仅接收到MediaFrame帧信息的回调函数(未去接收实际帧数据，里面的data数据无效)
         * @param frame [in] 媒体帧信息
         */
        using OnGetFrameInfoCallback = std::function<void(MediaFrame &)>;

        //!@brief 媒体通道定义
        class Channel {
        public:

            /**
             * @brief 增加一个通道
             * @param type [in] 帧类型
             * @param chn [in] 通道号
             */
            void Add(MediaFrame::TypeE type, int32_t chn) {
                array.emplace_back(type, chn);
            }

            //!@brief 媒体通道内容实体
            struct Item {
                Item(MediaFrame::TypeE t, int32_t c) : type(t), chn(c), inited(false) {}

                MediaFrame::TypeE type; //!<帧类型
                int32_t chn; //!<通道号
                uint32_t waitSeq; //!<期望的序号(仅venc需要)
                bool inited; //!<是否已经初始化(仅venc需要，控制出错后等待I帧)
            };

            /**
             * @brief 获取通道内某个实体
             * @param t [in] 帧类型
             * @param c [in] 通道号
             */
            Item *GetChnItem(MediaFrame::TypeE t, int32_t c) {
                for (auto &e : array) {
                    if (e.type == t && e.chn == c)
                        return &e;
                }
                return nullptr;
            }

            std::vector<Item> array; //!<通道数组
        };

        /**
         * @brief 根据帧信息获取帧数据
         * @param frame [in,out] 帧信息，帧数据
         * @return ErrCodeE
         */
        ErrCodeE GetFrameByInfo(MediaFrame &frame);

    protected:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

        ErrCodeE CheckFrame(ErrCodeE errPop, MediaFrame &frame);

    protected:
        OnGetFrameCallback m_cbFrame;
        OnGetFrameInfoCallback m_cbFrameInfo;
        MediaRingBufferReader m_ring;
        std::string m_data;
        Channel m_chn;
    };

    //!@brief MediaFrame异步数据接收
    class MediaClientStreamAsync : public MediaClientStream {
    public:
        MediaClientStreamAsync(EuvLoop &loop) : MediaClientStream(), m_loop(loop) {}

        virtual ~MediaClientStreamAsync() {}

        /**
         * @brief 开始接收
         * @param chn [in] 需要接收的通道
         * @param cb [in] 接收到MediaFrame帧数据的回调函数
         * @param cbInfo [in] 仅接收到MediaFrame帧信息的回调函数(未去接收实际帧数据),可选参数
         * @return ErrCodeE
         */
        ErrCodeE Start(Channel &chn, OnGetFrameCallback cb, OnGetFrameInfoCallback cbInfo = nullptr);

        //!@brief 停止MediaFrame异步数据接收
        void Stop();

        //!@brief 暂停接收消息数据
        void RecvFramePause() { m_sub.PauseRecv(); }

        //!@brief 恢复接收消息数据
        void RecvFrameResume() { m_sub.ResumeRecv(); }

        //!@brief 是否正在接收消息数据
        bool IsReceivingFrame() { return m_sub.IsReceiving(); }

    private:
        EuvLoop &m_loop;
        MsgTopicSubscriberAsync m_sub;
        char m_buffer[256];
    };

    //!@brief MediaFrame同步数据接收
    class MediaClientStreamSync : public MediaClientStream {
    public:
        MediaClientStreamSync() : MediaClientStream() {}

        /**
         * @brief 打开接收端口
         * @param chn [in] 需要接收的通道
         * @param timeoutMs [in] 打开操作超时时间
         * @return ErrCodeE
         */
        ErrCodeE Open(Channel &chn, int32_t timeoutMs);

        //!@brief 关闭接收端口
        void Close();

        /**
         * @brief 获取数据
         * @param frame [out] 获取到的帧数据
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE GetFrame(MediaFrame &frame, int32_t timeoutMs);

        /**
         * @brief 获取消息头信息
         * @param frame [out] 获取到的消息头
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE GetFrameInfo(MediaFrame &frame, int32_t timeoutMs);

    private:
        MsgTopicSubscriberSync m_sub;
    };
}

#endif //EMX_MediaClientStream_HPP
