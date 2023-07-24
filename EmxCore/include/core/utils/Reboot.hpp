//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Reboot_HPP
#define EMX_Reboot_HPP

#include "core/msg/MsgTopic.hpp"
#include <functional>


namespace Emx {
    class Reboot {
    public:
        /*!
         * @brief 重启系统
         * @param delayS [in] 几s后重启，默认立即重启
         */
        static void DoReboot(int delayS = 0);
    };

    class RebootMonitor {
    public:
        RebootMonitor() {}

        virtual ~RebootMonitor() {}

        using OnReboot = std::function<void()>;

        void Create(EuvLoop &loop, char *buffer, int bufferSize, OnReboot cb);

        void Destroy();

    private:
        MsgTopicSubscriberAsync m_sub;
        OnReboot m_cb;
    };

}

#endif //EMX_Reboot_HPP
