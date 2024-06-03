/*
* @Author: xiong
* @Date: 2022/12/8
*/

#include "VoipAvPush.hpp"
#include "cmcc_rtc_api.h"

using namespace Emx;

void VoipAvPush::Create(int session, int chnNum) {
    Destroy();
    m_session = session;
    /* 发送视频数据时，建议前 50 帧每隔 10 帧发送一个 I 帧 */
    m_iFrameCnt = 50;
    m_loop.Init("VoipAvPush", [this]() { m_stream.Stop(); });
    MediaClientStream::Channel chn;
    chn.Add(MediaFrame::TypeE::Venc, chnNum);
    m_stream.Start(chn, std::bind(&VoipAvPush::OnGetFrame, this, ph_1, ph_2));
    m_loop.Start();
    m_created = true;
}

void VoipAvPush::Destroy() {
    if (!m_created)
        return;
    m_loop.StopAndDeInit();
    m_created = false;
}

void VoipAvPush::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (m_loop.isQuiting())
        return;
    if (e != ErrCodeE::Success)
        return;
    if (m_iFrameCnt > 0) {
        if (m_iFrameCnt % 10 == 0) {
            MediaClientVenc venc(frame.chn);
            venc.FlushIDR();
        }
        m_iFrameCnt--;
    }
    cmcc_rtc_send_video(m_session, (char *) (frame.data), (int) frame.size);
}
