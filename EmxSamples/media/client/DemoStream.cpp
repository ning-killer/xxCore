/**
 * @file   DemoStream.cpp
 * @author hening
 * @brief  媒体服务音视频流单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>
#include <thread>

using namespace Emx;

// 异步实时流获取示例
class DemoSaveAsyncStream {
  public:
    // MediaClientStreamAsync m_stream对象需要在初始化的时候需要传入一个loop
    DemoSaveAsyncStream(EuvLoop &loop) : m_stream(loop) {
        m_fpVenc = nullptr;
        m_fpAenc = nullptr;
    }

    void Create() {
        // 首先定义通道，并向通道中加入需要监听的媒体帧类型
        MediaClientStream::Channel channel;
        channel.Add(Emx::MediaFrame::TypeE::Venc, 0); // 加入视频编码通道0
        channel.Add(Emx::MediaFrame::TypeE::Aenc, 0); // 加入音频编码通道0
        channel.Add(Emx::MediaFrame::TypeE::Pcm, 0); 
        // 注册当获取到媒体帧时候的回调
        m_stream.Start(channel, std::bind(&DemoSaveAsyncStream::OnRecvFrame, this, ph_1, ph_2));
        // 创建两个文件用来保存音频和视频的裸数据
        m_fpVenc = fopen("/mnt/nfs/video.raw", "wb+");
        m_fpAenc = fopen("/mnt/nfs/audio.raw", "wb+");
    }

    void Destroy() {
        // 停止媒体帧的监听
        m_stream.Stop();
        // 关闭文件
        if (m_fpVenc) {
            fclose(m_fpVenc);
            m_fpVenc = nullptr;
        }
        if (m_fpAenc) {
            fclose(m_fpAenc);
            m_fpAenc = nullptr;
        }
    }

  private:
    // 当收到媒体帧时触发此回调
    void OnRecvFrame(ErrCodeE e, MediaFrame &frame) {
        // 如果获取失败则返回
        if (e != Emx::ErrCodeE::Success) {
            emxlogw("recv stream failed\n");
            return;
        }

        //这里只会收到m_stream.Start的时候在channel中Add的帧类型+通道的帧
        if (frame.type == Emx::MediaFrame::TypeE::Venc) {
            emxlogd("venc client get chn[%d]: frame seq[%d]; interval[%llu]; size[%d]; iskeyFrame[%d]\n", frame.chn
                    , frame.seq, uint64_t(uint64_t(frame.tsInUs / 1000) - last_venc_ts), frame.size, frame.isKeyFrame);
            // 收到视频帧
            // fwrite(frame.data, 1, frame.size, m_fpVenc);
            last_venc_ts = uint64_t(frame.tsInUs / 1000);
        } else if (frame.type == Emx::MediaFrame::TypeE::Aenc) {
            emxlogd("adec client get chn[%d]: frame seq[%d]; interval[%llu]; size[%d]; iskeyFrame[%d]\n", frame.chn
                        , frame.seq, uint64_t(uint64_t(frame.tsInUs / 1000) - last_aenc_ts), frame.size, frame.isKeyFrame);
            // 收到音频帧
            // fwrite(frame.data, 1, frame.size, m_fpAenc);
            last_aenc_ts = uint64_t(frame.tsInUs / 1000);
        } else if (frame.type == Emx::MediaFrame::TypeE::Pcm) {
            emxlogd("pcm client get chn[%d]: frame seq[%d]; interval[%llu]; size[%d]; iskeyFrame[%d]\n", frame.chn
                        , frame.seq, uint64_t(uint64_t(frame.tsInUs / 1000) - last_pcm_ts), frame.size, frame.isKeyFrame);
            last_pcm_ts = uint64_t(frame.tsInUs / 1000);
        }
        emxlogd("async get stream ok [%d].\n", (int)frame.type); 
    }

  private:
    MediaClientStreamAsync m_stream; // 定义一个异步音视频流的监听句柄
    FILE *m_fpVenc; // 定义file指针用来存储编码后的视频裸流
    FILE *m_fpAenc; // 定义file指针用来存储编码后的音频裸流
    uint64_t last_pcm_ts = 0;
    uint64_t last_aenc_ts = 0;
    uint64_t last_venc_ts = 0;
};

// 同步实时流获取示例
class DemoSaveSyncStream {
  public:
    ~DemoSaveSyncStream() { Destroy(); }
    void Create() {
        m_quit = false;
        m_thread = std::thread([this]() {
            FILE *fpVenc = fopen("/mnt/nfs/video.raw", "wb+");// 定义file指针用来存储编码后的视频裸流
            FILE *fpAenc = fopen("/mnt/nfs/audio.raw", "wb+");// 定义file指针用来存储编码后的音频裸流
            // 首先定义通道，并向通道中加入需要监听的媒体帧类型
            MediaClientStream::Channel channel;
            channel.Add(Emx::MediaFrame::TypeE::Venc, 0); // 加入视频编码通道0
            channel.Add(Emx::MediaFrame::TypeE::Aenc, 0); // 加入音频编码通道0
            MediaClientStreamSync stream;
            stream.Open(channel, 1000);
            while (!m_quit) {
                MediaFrame frame = {};
                if (stream.GetFrame(frame, 1000) != Emx::ErrCodeE::Success) {
                    emxloge("sync get stream failed.\n");
                    continue;
                }
                //这里只会收到m_stream.Start的时候在channel中Add的帧类型+通道的帧
                if (frame.type == Emx::MediaFrame::TypeE::Venc) {
                    // 收到视频帧
                    fwrite(frame.data, 1, frame.size, fpVenc);
                } else {
                    // 收到音频帧
                    fwrite(frame.data, 1, frame.size, fpAenc);
                }
                emxloge("sync get stream ok [%d].\n", (int)frame.type);
            }
            stream.Close();
            fclose(fpVenc);
            fclose(fpAenc);
        });
    }

    void Destroy() {
        m_quit = true;
        m_thread.join();
    }

  private:
    std::thread m_thread;
    bool m_quit;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [DemoStream false(同步) | true(异步)]\n");
        return -1;
    }
    int flag = std::stoi(argv[1]);

    
    EuvLoop loop;
    loop.Init("DemoQRScan", []() { });
    DemoSaveAsyncStream asyncStreamDemo(loop);
    DemoSaveSyncStream syncStreamDemo;

    if (flag) {
        asyncStreamDemo.Create();
    } else {
        syncStreamDemo.Create();
    }

    loop.Start(false);
    return 0;
}