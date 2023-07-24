/*
* @Author: xiong
* @Date: 2023/5/31
*/

#ifndef EMX_SDCardServerLib_HPP
#define EMX_SDCardServerLib_HPP

#include <functional>

namespace Emx {
    class SDCardServer;

    class SDCardServerLib {
    public:
        SDCardServerLib();

        virtual ~SDCardServerLib();

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        SDCardServer *m_server;
    };
}
#endif //EMX_SDCardServerLib_HPP
