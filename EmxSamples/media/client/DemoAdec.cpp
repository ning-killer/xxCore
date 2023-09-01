/**
 * @file   DemoAdec.cpp
 * @author hening
 * @brief  媒体服务audio解码单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

// 播放音频文件示例
class DemoAudioPlay {
  public:
    ~DemoAudioPlay() { Destroy(); }
    void Create(EuvLoop &loop) {
        // 初始化定时器
        m_timer.Create(loop);
        // 启动定时器，首次超时时间定为立刻超时，之后每隔3000ms超时一次
        // 注册定时器的超时回调函数Snap，每次超时播放一次音频文件
        m_timer.Start(0, 5000, []() {
            // 播放指定目录下的音频文件,可以多次重复调用，MediaServer会按照先后顺序组织播放队列进行播放
            char path[EMX_MAX_PATH_SIZE] = { 0 };
            snprintf(path, sizeof(path) - 1, "%s/audio/aginging.aac", Misc::GetAppConfigsDir());
            MediaClientAdecStreamSync::PlayFile(0, path);
            
        });
    }

    void Destroy() {
        // 销毁定时器
        m_timer.Destroy();
        // 如果想中断播放，可以在任何时刻调用如下接口, 音频会终止播放
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
    }

  private:
    EuvTimer m_timer; // 定义一个定时器，用来定时播放音频文件
};

// 解码播放实时音频流示例
class DemoTalkbackPlay {
  public:
    ~DemoTalkbackPlay() { Destroy(); }
    void Create(EuvLoop &loop) {
        char path[EMX_MAX_PATH_SIZE] = { 0 };
        snprintf(path, sizeof(path) - 1, "%s/audio/aginging.aac", Misc::GetAppConfigsDir());
        // 打开aac音频文件，需要保证解码通道配置为aac解码
        m_fp = fopen(path, "rb");
        // 初始化定时器
        m_timer.Create(loop);
        // 启动定时器，首次超时时间定为立刻超时，之后每隔3000ms超时一次
        // 注册定时器的超时回调函数，每次超时读取一帧音频文件并发送
        m_timer.Start(0, 20, [this]() {
            // 读取aac音频文件
            char buffer[2048];
            auto n = fread(buffer, 1, 7, m_fp);
            if (n <= 0) {
                fseek(m_fp, SEEK_SET, 0);
                emxlogd("EOF\n");
                return;
            }
            auto size = ((buffer[3] & 0x03) << 11) | (buffer[4] << 3) | (buffer[5] >> 5);
            n = fread(buffer + 7, 1, size - 7, m_fp);
            if ((int) n != size - 7) {
                emxloge("Not an ADTS packet\n");
                return;
            }
            // 将音频帧发送给MediaServer进行解码播放
            MediaAdec::Frame frame = {};
            frame.data = (uint8_t *) buffer;
            frame.size = (int) size;
            frame.tsInUs = 0;
            frame.seq = 0;
            emxlogd("PushFrame.\n");
            MediaClientAdecStreamSync::PushFrame(0, frame);
        });

        // 初始化定时器
        m_timerStop.Create(loop);
        // 启动定时器，10s后停止播放
        m_timerStop.Start(10000, 0, [this]() {
            m_timer.Stop();
        });
    }

    void Destroy() {
        // 销毁定时器
        m_timer.Destroy();
        m_timerStop.Destroy();
        if (m_fp) {
            fclose(m_fp);
            m_fp = nullptr;
        }
        // 如果想中断播放，可以在任何时刻调用如下接口, 音频会终止播放
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
    }

  private:
    EuvTimer m_timer; // 定义一个定时器，用来播放音频文件
    EuvTimer m_timerStop; // 定义一个定时器，用来停止播放
    FILE *m_fp;
};

void TestBase() {
    int num;
    if (MediaClientAdec::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("adec GetChnNum failed, num(%d).\n", num);
        return;
    }

    MediaClientAdec adecClient(0);
    MediaAdec::Param param;
    if (adecClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("adec GetParam failed.\n");
        return;
    }
    emxlogd("adec param: codec[%d];bitRate[%d];sampleRate[%d];volume[%d];bitWidth[%d]\n"
            , param.codec, param.bitRate, param.sampleRate, param.volume, param.bitWidth);
    int vol = param.volume;
    if (vol == 60) {
        vol = 30;
    }
    if (adecClient.SetVolume(vol) != ErrCodeE::Success) {
        emxloge("adec SetVolume failed.\n");
        return;
    }
    if (adecClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("adec GetParam failed.\n");
        return;
    }
    emxlogd("adec param: codec[%d];bitRate[%d];sampleRate[%d];volume[%d];bitWidth[%d]\n"
            , param.codec, param.bitRate, param.sampleRate, param.volume, param.bitWidth);
    
    // 验证PlayFileWithVolume()
    int playNum = 10;
    for (int i = 0; i < playNum; i++) {
        char path[EMX_MAX_PATH_SIZE] = { 0 };
        snprintf(path, sizeof(path) - 1, "%s/audio/aginging.aac", Misc::GetAppConfigsDir());
        MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 5);
    }
}

int main(int argc, char *argv[]) {
    // TestBase();

    EuvLoop loop;
    loop.Init("DemoAdec", []() { });
    // DemoAudioPlay audioPlayDemo;
    // audioPlayDemo.Create(loop);

    DemoTalkbackPlay talkbackPlayDemo;
    talkbackPlayDemo.Create(loop);

    loop.Start(false);
    return 0;
}