/*
* @Author: xiong
* @Date: 2023/6/2
*/

#ifndef EMX_WebServerLib_HPP
#define EMX_WebServerLib_HPP

namespace Emx {

    class WebServer;

    class WebServerLib {
    public:
        WebServerLib();

        virtual ~WebServerLib();

        void Create(bool newThread);

        void Destroy();

    private:
        WebServer *m_server;
    };
}

#endif //EMX_WebServerLib_HPP
