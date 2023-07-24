//
// Created by xiong on 2020/11/6.
//

#ifndef EMX_EAPILSTREAMSERVER_HPP
#define EMX_EAPILSTREAMSERVER_HPP

#include <list>
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "Net.hpp"

#define EMX_STREAM_SERVICE_PORT (8090)
namespace Emx {
    class Stream {
    public:
        /********************************音视频请求与应答公共头********************************************/
        //命令类型
        enum class CmdTypeE : uint8_t {
            Req = 0,
            Resp = 1
        };
        struct Header {
            char sync[3];     //起始码 固定“AVS”
            CmdTypeE type; //命令类型 0-请求 1-应答
            uint32_t size;  //数据长度
        };
        enum class ErrCodeE : uint32_t {
            Normal = 0,
            UnSupportStreamType = 2,
            OverConnectNum = 3
        };
        /********************************音视频请求参数********************************************/
        //码流类型定义
        enum class StreamTypeE : uint8_t {
            Major = 4,
            Minor = 5
        };
        struct AVReqInfo {
            StreamTypeE type; //码流类型
            uint8_t chn;    //通道号
            uint8_t reserved[2];     //预留
        };

        //帧类型定义
        enum class FrameTypeE : uint8_t {
            IFrame = 1,
            PFrame = 2,
            Audio = 3
        };
        /********************************视频流应答头********************************************/
        //视频编码类型定义
        enum class VideoCodecChnTypeE : uint8_t {
            H264Major = 4,
            H264Minor = 5,
            H265Major = 6,
            H265Minor = 7,
            MjpegMajor = 8,
            MjpegMinor = 9
        };
        struct VideoInfo {
            FrameTypeE frameType;    //帧类型
            VideoCodecChnTypeE encoderType;  //视频编码类型
            uint8_t frameRate;    //帧率
            uint8_t reserved[1];       //预留
            uint16_t width;  //视频宽
            uint16_t height; //视频高
            uint32_t frameSize;     //帧大小
            uint32_t frameId;       //帧ID
            uint32_t timeStampH;
            uint32_t timeStampL; //时间戳单位ms
            uint32_t errorCode;  //错误码
            uint32_t extLength;  //扩展长度
        };
        /********************************音频流应答头********************************************/
        enum class AudioCodecChnTypeE : uint8_t {
            G711U = 1,
            G711A = 2,
            G726 = 4,
            PCM = 8,
            AAC = 9
        };

        struct AudioInfo {
            FrameTypeE frameType;   //帧类型
            AudioCodecChnTypeE encoderType; //音频编码类型
            uint8_t reserved[2];      //预留
            uint16_t bitCount;   //采样位数
            uint16_t samples;    //采样率
            uint32_t frameSize;    //帧大小
            uint32_t frameId;      //帧ID
            uint32_t timeStampH;
            uint32_t timeStampL; //时间戳单位ms
            uint32_t errorCode;  //错误码
            uint32_t extLength;  //扩展长度
        };
    };


    class StreamClient : public EuvTcpServerClient {
    public:
        StreamClient(EuvStmServerHostBase *host, EuvLoop &loop) :
                EuvTcpServerClient(host) {
            waitSize = sizeof(Stream::Header);
            waitHeader = true;
            m_avStream = nullptr;
            running = false;
            buffered = 0;
        }

        void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override;

        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

        void SetupStream(int chn);

        std::shared_ptr<MediaClientStreamAsync> m_avStream;
        bool waitHeader;
        bool running;
        uint32_t waitSize;
        Stream::Header header;
        std::string data;
        char ip[Net::Ipv4AddrSize];
        Stream::VideoInfo video;
        Stream::AudioInfo audio;
        uint32_t frameId;
        int32_t buffered;
    };

    class StreamService : public EuvTcpServerHost {
    public:

        StreamService() : EuvTcpServerHost() {}

        virtual ~StreamService() {};

        void CreateService(Json::Value &json);

        void DestroyService();

    private:
        void OnQuit();

        EuvStmServerClientBase *OnAllocClient() override;

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

    private:
        EuvLoop m_loop;
        int32_t m_maxSendBufSize;
        int32_t m_maxAVBufferNumber;
        char m_buffer[4096];
        std::list<StreamClient *> m_clientList;
    };
}


#endif //EMX_EAPILSTREAMSERVER_HPP
