/**
 * @file   DemoSnap.cpp
 * @author hening
 * @brief  媒体服务缩略图抓拍单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

// 缩略图抓取示例
class DemoSaveSnap {
  public:
    void Create(EuvLoop &loop) {
        // 初始化定时器
        m_timer.Create(loop);
        // 启动定时器，首次超时时间定为立刻超时，之后每隔3000ms超时一次
        // 注册定时器的超时回调函数Snap，每次超时抓拍一张图片
        m_timer.Start(0, 3000, std::bind(&DemoSaveSnap::Snap, this));
    }

    void Destroy() {
        // 销毁定时器
        m_timer.Destroy();
    }

  private:
    // 抓拍一张图片,这里演示的是阻塞抓拍模式，不需要使用loop, 回调模式可参考SaveStream添加Snap类型的通道
    void Snap() {
        // 添加需要监听的Snap通道，这里监听snap通道0
        MediaClientStream::Channel channel;
        channel.Add(MediaFrame::TypeE::Snap, 0);

        // 启动snap的监听
        MediaClientStreamSync snap;
        ErrCodeE e = snap.Open(channel, 3000);
        if (e != ErrCodeE::Success) {
            emxloge("open snap failed\n");
            return;
        }

        // 触发一个snap信号，MediaServer收到这个信号后会立刻发送一帧缩略图
        MediaClientSnap::Trigger(0, 3000);

        // 接收这帧缩略图
        MediaFrame frame = {};
        e = snap.GetFrame(frame, 3000);
        if (e != ErrCodeE::Success) {
            emxloge("get snap failed\n");
            return;
        }

        // 将缩略图保存下来
        char file[128] = { 0 };
        static int num = 0;
        snprintf(file, sizeof(file) - 1, "/customer/nfs/code-projects/Platform/MMC233Z/%d_snap.jpg", num);
        FILE *fp = fopen(file, "wb+");
        if (!fp) {
            emxloge("open snap failed\n");
            snap.Close();
            return;
        }
        fwrite(frame.data, 1, frame.size, fp);
        emxlogd("file(%s) snap ok.\n", file);
        num++;

        // 关闭snap接收通道
        snap.Close();
    }

  private:
    EuvTimer m_timer; // 定义一个定时器，用来定时抓拍缩略图
};

int main(int argc, char *argv[]) {
    DemoSaveSnap snapDemo;
    EuvLoop loop;
    loop.Init("DemoSnap", []() { });
    snapDemo.Create(loop);
    loop.Start(false);
    return 0;
}