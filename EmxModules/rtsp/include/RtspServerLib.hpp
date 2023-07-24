/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_RtspServerLib_HPP
#define EMX_RtspServerLib_HPP

namespace Emx {

    class EmxRtspServer;

    class RtspServerLib {
    public:
        RtspServerLib();

        virtual ~RtspServerLib();

        void Create(bool newThread);

        void Destroy();

    private:
        EmxRtspServer *m_server;
    };
}

#endif //EMX_RtspServerLib_HPP
