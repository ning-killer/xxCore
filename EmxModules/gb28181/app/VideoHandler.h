#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include <unistd.h>
#include <thread>

class GBSdp;

class VideoHandler: public std::enable_shared_from_this<VideoHandler>
{
public:
    VideoHandler(const std::string& id) :m_id(id)
    {
        m_latestTime = time(NULL);
    }
    virtual ~VideoHandler() {
        if (_fd != -1)
            close(_fd);
    }

    virtual std::string GetType() = 0;

    virtual int Init(void* dlg, GBSdp& inSdp, std::string& outSdp) = 0;

    virtual int Start() = 0;

    virtual int Stop() = 0;

    virtual time_t GetLatestTime()
    {
        return m_latestTime;
    }

    virtual int Play(float speed)
    {
        return 0;
    }

    virtual int Pause(float pauseTime)
    {
        return 0;
    }

    virtual int SeekPos(float pos)
    {
        return 0;
    }

    virtual int Teardown()
    {
        return 0;
    }
protected:
    int ConnectServer();

    int InputPSFrame(char* psData, uint32_t psSize);

    int SendRTPPacket(char* rtp, uint32_t rtp_size);

    int packHeader(char* in, bool mark, uint32_t timeStamp);

    int SendRawData(const char* data, int len, uint8_t flag = 0);
protected:
    int _fd = -1;
    uint8_t _iTcp = 0;
    std::string _remoteIp;
    uint16_t _remotePort;

    std::string m_id;
    uint16_t _seq = 0;
    uint32_t _ssrc = 0;
    time_t m_latestTime;

    char rtpPacket[1500] = { 0 };
    uint8_t _quitFlag = 0;
    uint32_t _rtpTimestamp = 0;

    /*sip dialog*/
    void* _devDlg = nullptr;
};

int AddHandler(std::shared_ptr<VideoHandler> h, const std::string& id);

void RunHandler(const std::string& id);

void RemoveHandler(const std::string& id);

/*
 * @parm type 操作类型 0 倍速 1 暂停 2 拖动
 * @parm value 操作值
 */
void OperationHandler(const std::string& id, int type, float value);

void CheckHandlerTimeout();

//class VideoHandlerMgr{
//public:
//    VideoHandlerMgr();
//    ~VideoHandlerMgr();
//
//    int AddHandler(std::shared_ptr<VideoHandler> h, const std::string& id);
//
//    void RunHandler(const std::string& id);
//
//    void RemoveHandler(const std::string& id);
//
//    void Start();
//private:
//    void checkTimeoutTask();
//
//private:
//    uint8_t _quitFlag = 0;
//    std::mutex _hLock;
//    std::condition_variable _hCond;
//    std::map<std::string, std::shared_ptr<VideoHandler>> _hMap;
//    std::shared_ptr<std::thread> _checkThr;
//};
