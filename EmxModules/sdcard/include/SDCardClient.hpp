//
// Created by xiong on 2021/6/1.
//

#ifndef EMX_SDCardClient_HPP
#define EMX_SDCardClient_HPP

#include <functional>

#include "SDCard.hpp"

namespace Emx {
    //!@brief 操作设备SD卡的客户端接口
    class SDCardClient : public SDCard {
    public:
        SDCardClient(int chn) : SDCard(chn) {}

        /**
         * @brief 获取设备支持的通道数量
         * @return 通道数量
         */
        static int GetChnNum();

        ErrCodeE Mount() override;

        ErrCodeE UnMount() override;

        ErrCodeE Format() override;

        ErrCodeE GetInfo(Info &info) override;

        ErrCodeE Check() override;
    };

    //!用于监听SD卡插拔事件
    class SDCardClientEvent {
    public:
        /**
         * @brief SD卡插拔事件回调函数
         * @param chn [in] 发生时间的通道号
         * @param info [in] SD卡信息
         */
        using SDCardInfoCallback = std::function<void(int chn, SDCard::Info &info)>;

        /**
         * @brief 构造函数
         * @param loop [in] 需要绑定的loop
         * @param buffer [in] 提供用于缓存的buffer
         * @param size [in] 用于缓存buffer的大小
         * @param cb [in] SD卡插拔事件回调函数
         */
        SDCardClientEvent(EuvLoop &loop, char *buffer, int size, SDCardInfoCallback cb) :
                m_loop(loop), m_buffer(buffer), m_size(size), m_cb(std::move(cb)) {
        }

        /**
         * @brief 创建监听
         * 创建成功之后，会收到异步的SDCardInfoCallback回调
         * 方便使用者知道SD卡当前的状态
         */
        void Create();

        //!@brief 销毁监听
        void Destroy();

    private:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        EuvLoop &m_loop;
        char *m_buffer;
        int m_size;
        SDCardInfoCallback m_cb;
        MsgTopicSubscriberAsync m_sub;
    };
}

#endif //EMX_SDCardClient_HPP
