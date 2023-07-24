/*
* @Author: xiong
* @Date: 2022/12/8
*/

#ifndef EAPIL_VoipAvPush_HPP
#define EAPIL_VoipAvPush_HPP

#include "EmxCore.hpp"
#include "EmxMedia.hpp"

namespace Emx {

    class VoipAvPush {
    public:
        VoipAvPush() : m_created(false), m_stream(m_loop) {}

        void Create(int session, int chnNum);

        void Destroy();

        bool IsRunning() { return m_created; }

        int GetSession() { return m_session; }

    private:
        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

    private:
        bool m_created;
        EuvLoop m_loop;
        MediaClientStreamAsync m_stream;
        int m_session;
        int m_iFrameCnt;
    };

}

#endif //EAPIL_VoipAvPush_HPP
