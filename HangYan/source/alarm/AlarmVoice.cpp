/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#include "AlarmVoice.hpp"
#include <cmath>

using namespace Emx;

ErrCodeE AlarmVoice::Create() {
    //开始监听音频PCM数据
    m_voice = new MediaClientStreamAsync(m_ctx->loop);
    MediaClientStream::Channel channel;
    channel.Add(MediaFrame::TypeE::Pcm, 0);
    m_voice->Start(channel, [this](ErrCodeE e, MediaFrame &frame) {
        if (!m_ctx->env.voice.ena)
            return;
        if (e != ErrCodeE::Success)
            return;
        if (frame.type != MediaFrame::TypeE::Pcm)
            return;
        int sum = 0;
        int count = 0;
        for (int i = 0; i < frame.size >> 1; i++) {
            int data = abs((int) *(((short *) frame.data) + i));
            if (data > 0) {
                sum += data;
                count++;
            }
        }

        int db = count > 0 ? (int) (20.0 * log10(sum / count)) : 0;
        int targetDb;
        if (m_ctx->env.voice.sensitivity <= 80) {
            targetDb = m_ctx->env.voice.map[0];
        } else if (m_ctx->env.voice.sensitivity <= 85) {
            targetDb = m_ctx->env.voice.map[1];
        } else if (m_ctx->env.voice.sensitivity <= 90) {
            targetDb = m_ctx->env.voice.map[2];
        } else if (m_ctx->env.voice.sensitivity <= 95) {
            targetDb = m_ctx->env.voice.map[3];
        } else {
            targetDb = m_ctx->env.voice.map[4];
        }
        if (db >= targetDb) {
            emxlogd("voice db[%d] >= targetDb[%d]\n", db, targetDb);
            AlarmTrigger();
        }
    });
    return ErrCodeE::Success;
}

void AlarmVoice::Destroy() {
    m_voice->Stop();
    delete m_voice;
}
