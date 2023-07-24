#pragma once
#include "VideoHandler.h"

class PlaybackHandler :public VideoHandler
{
public:
	PlaybackHandler(const std::string& id);
	~PlaybackHandler();

    std::string GetType() override
    {
        return "playback";
    }

    int Init(void* dlg, GBSdp& inSdp, std::string& outSdp) override;

    int Start() override;

    int Stop() override;

    virtual int Play(float speed) override;

    virtual int Pause(float pauseTime) override;

    virtual int SeekPos(float pos) override;

    virtual int Teardown() override;
private:
    void sendStreamTask();
private:
    std::shared_ptr<std::thread> _streamTask;
    uint8_t _sendKeyFrame = 0;

    uint32_t m_beginTime = 0;
    uint32_t m_endTime = 0;
};