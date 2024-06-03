//
// Created by xiong on 2022/7/5.
//

#ifndef EMX_Key_HPP
#define EMX_Key_HPP

#include "EmxBsp.hpp"

namespace Emx {

    class Key {
    public:
        static const int DetectInterval = 100; //100ms
        enum class EventE : uint8_t {
            Click,
            LongPress,
            Up,
            Down,
        };

        enum class ModeE : uint8_t {
            Key,
            Check,
        };

        using OnKeyEvent = std::function<void(const char *name, EventE e)>;

        Key(const char *name, EuvLoop &loop, Bsp::GpioLike *key, int longPressTimeMs,ModeE mode) :
                m_name(name), m_loop(loop), m_key(key), m_longPressTime(longPressTimeMs),
                m_pressTime(0),m_mode(mode),m_checkStatus(false){}

        virtual ~Key() {}

        void Create(OnKeyEvent e);

        void Destroy();

    private:
        void OnTimer();

    private:
        std::string m_name;
        EuvLoop m_loop;
        Bsp::GpioLike *m_key;
        int m_longPressTime;
        OnKeyEvent m_cb;
        EuvTimer m_timer;
        int m_pressTime;
        ModeE m_mode;
        bool m_checkStatus;
    };

}

#endif //EMX_Key_HPP
