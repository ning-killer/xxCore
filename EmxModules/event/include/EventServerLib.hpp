/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_EventServerLib_HPP
#define EMX_EventServerLib_HPP

namespace Emx {
    class EventServer;

    class EventServerLib {
    public:
        EventServerLib();

        virtual ~EventServerLib();

        void Create(bool newThread);

        void Destroy();

    private:
        EventServer *m_server;
    };

}

#endif //EMX_EventServerLib_HPP
