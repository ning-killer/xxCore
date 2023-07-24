/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_NetServerLib_HPP
#define EMX_NetServerLib_HPP

namespace Emx {

    class NetServer;

    class NetServerLib {
    public:
        NetServerLib();

        virtual ~NetServerLib();

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        NetServer *m_server;
    };

}

#endif //EMX_NetServerLib_HPP
