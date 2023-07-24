#pragma once
#include "VideoHandler.h"

class PlayHandler :public VideoHandler
{
public:
    PlayHandler(const std::string& id);
    ~PlayHandler();

    std::string GetType() override
    {
        return "play";
    }

    int Init(void* dlg, GBSdp& inSdp, std::string& outSdp) override;

    int Start() override;

    int Stop() override;
private:
    void sendStreamTask();
private:
    std::shared_ptr<std::thread> _streamTask;
    uint8_t _sendKeyFrame = 0;
};
