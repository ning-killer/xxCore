# EmxMedia设计使用说明

<!-- ---
html:
    toc: true
    number_sections: true
    toc_depth: 2
    toc_float: true
    smooth_scroll: true
    collapsed: true
--- -->

# 0.版本信息

| 版本 | 发布日期       | 作者      | 修改说明 |
| ---- | -------------- | --------- | -------- |
| v1.0 | 2022年10月13日 | xiaoxiong | 初版发布 |


# 1.服务程序EmxMediaServer
&emsp;&emsp;EmxMedia包含一个独立的服务程序EmxMediaServer，屏蔽了底层不同IPC芯片的差异，对外提供媒体服务，用户使用的时候只需要include头文件EmxMedia.hpp以及连接libEmxMedia.so动态库即可使用这些服务。服务程序EmxMediaServer运行于后台，实现了所有与IPC芯片媒体相关的任务操作，EmxMediaServer启动后会去加载两种类型的文件，一种是静态配置文件，里面存储的是芯片底层的配置，位于media的configs目录下，这类的配置文件用户一般不需要关心，另一种是动态加密的param文件，位于app的param目录下，保存的是用户对于media的动态配置信息，例如编码格式/osd等参数。EmxMediaServer识别configs目录的路径是依靠系统环境变量**EMX_MEDIA_CONFIGS_DIR**，因此需要在EmxMediaServer启动前配置好，例如:

```
export EMX_MEDIA_CONFIGS_DIR=/root/firmware/media/configs
```

# 2.系统启停控制

&emsp;&emsp;系统启停控制部分提供了EmxMedia功能的启动和停止，当EmxMediaServer程序运行后会默认启动所有媒体服务，当用户需要固件升级等需要大量内存空间时，可以调用此接口临时停止媒体服务使其释放出内存空间，对应的接口是MediaClientServer

# 3.图像ISP

&emsp;&emsp;ISP部分提供了设备ISP参数的配置和获取，夜视白天的状态切换，以及ISP实时状态的获取，可用于实现软光敏

# 4.视频编码

&emsp;&emsp;可用于配置和获取视频编码参数
## 4.3.视频编码配置示例
```c++
// 视频编码配置示例，音频/ISP/OSD等使用SetParam/GetParam接口进行参数配置的类都可参考此示例
class DemoVencConfig {
public:
    void Configs() {
        // 用于存放参数数据
        MediaVenc::Param param = {};
        // 定义一个通道0的编码器对象
        MediaClientVenc venc(0);
        // 先获取当前的配置
        if (venc.GetParam(param) != ErrCodeE::Success) {
            emxloge("GetParam failed\n");
            return;
        }

        // 切换编码格式
        if (param.codec == Emx::VideoCodecE::H265)
            param.codec = VideoCodecE::H264;
        else
            param.codec = Emx::VideoCodecE::H265;

        if (venc.SetParam(param) != ErrCodeE::Success) {
            emxloge("SetParam failed\n");
            return;
        }
        emxlogi("SetParam success\n");
    }
};
```
# 5.音频编码

&emsp;&emsp;MediaClientAenc可用于配置和获取视频编码参数

# 6.音频解码
## 6.4.示例
### 播放音频文件示例
```c++
// 播放音频文件示例
class DemoAudioPlay {
public:
    void Create(EuvLoop &loop) {
        // 初始化定时器
        m_timer.Create(loop);
        // 启动定时器，首次超时时间定为立刻超时，之后每隔3000ms超时一次
        // 注册定时器的超时回调函数Snap，每次超时播放一次音频文件
        m_timer.Start(0, 5000, []() {
            // 播放指定目录下的音频文件,可以多次重复调用，MediaServer会按照先后顺序组织播放队列进行播放
            MediaClientAdecStreamSync::PlayFile(0, "./demo.aac");
            // 如果想中断播放，可以在任何时刻调用如下接口, 音频会终止播放
            // MediaClientAdecStreamSync::PlayCtrlQuit(0);
        });
    }

    void Destroy() {
        // 销毁定时器
        m_timer.Destroy();
    }

private:
    EuvTimer m_timer; // 定义一个定时器，用来定时播放音频文件
};
```
### 解码播放实时音频流示例
```c++
// 解码播放实时音频流示例
class DemoTalkbackPlay {
public:
    void Create(EuvLoop &loop) {
        // 打开aac音频文件，需要保证解码通道配置为aac解码
        m_fp = fopen("./demo.aac", "rb");
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
    }

private:
    EuvTimer m_timer; // 定义一个定时器，用来播放音频文件
    EuvTimer m_timerStop; // 定义一个定时器，用来停止播放
    FILE *m_fp;
};
```

# 7.OSD
# 8.二维码扫描
## 二维码扫码示例
```c++
// 二维码扫码示例
class DemoQrScan {
public:
    void Create(EuvLoop &loop, char *buffer, int bufferSize) {
        // 启动扫码，扫码需要一个buffer用来接收数据
        // 注册一个回调函数，当扫描到有效的二维码后触发
        m_qr.StartQR(loop, buffer, bufferSize, [](int chn, const char *result) {
            printf("chn %d get QR content#%s#\n", chn, result);
        });
    }

    void Destroy() {
        //停止扫码
        m_qr.StopQR();
    }

private:
    MediaClientQRScan m_qr; // 定义一个二维码扫描对象用来执行扫码
};
```

# 9.缩略图抓拍
## 缩略图抓取示例
```c++
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
        FILE *fp = fopen("snap.jpg", "wb+");
        if (!fp) {
            emxloge("open snap failed\n");
            snap.Close();
            return;
        }
        fwrite(frame.data, 1, frame.size, fp);

        // 关闭snap接收通道
        snap.Close();
    }

private:
    EuvTimer m_timer; // 定义一个定时器，用来定时抓拍缩略图
};
```

# 10.音视频编码流
## 异步实时流获取示例
```c++
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
        // 注册当获取到媒体帧时候的回调
        m_stream.Start(channel, std::bind(&DemoSaveAsyncStream::OnRecvFrame, this, ph_1, ph_2));
        // 创建两个文件用来保存音频和视频的裸数据
        m_fpVenc = fopen("video.raw", "wb+");
        m_fpAenc = fopen("audio.raw", "wb+");
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
            // 收到视频帧
            fwrite(frame.data, 1, frame.size, m_fpVenc);
        } else {
            // 收到音频帧
            fwrite(frame.data, 1, frame.size, m_fpAenc);
        }
    }

private:
    MediaClientStreamAsync m_stream; // 定义一个异步音视频流的监听句柄
    FILE *m_fpVenc; // 定义file指针用来存储编码后的视频裸流
    FILE *m_fpAenc; // 定义file指针用来存储编码后的音频裸流
};
```

## 同步实时流获取示例

```c++
// 同步实时流获取示例
class DemoSaveSyncStream {
public:
    void Create() {
        m_quit = false;
        m_thread = std::thread([this]() {
            FILE *fpVenc = fopen("video.raw", "wb+");// 定义file指针用来存储编码后的视频裸流
            FILE *fpAenc = fopen("audio.raw", "wb+");// 定义file指针用来存储编码后的音频裸流
            // 首先定义通道，并向通道中加入需要监听的媒体帧类型
            MediaClientStream::Channel channel;
            channel.Add(Emx::MediaFrame::TypeE::Venc, 0); // 加入视频编码通道0
            channel.Add(Emx::MediaFrame::TypeE::Aenc, 0); // 加入音频编码通道0
            MediaClientStreamSync stream;
            stream.Open(channel, 1000);
            while (!m_quit) {
                MediaFrame frame = {};
                auto e = stream.GetFrame(frame, 1000);
                if (e != Emx::ErrCodeE::Success)
                    continue;
                //这里只会收到m_stream.Start的时候在channel中Add的帧类型+通道的帧
                if (frame.type == Emx::MediaFrame::TypeE::Venc) {
                    // 收到视频帧
                    fwrite(frame.data, 1, frame.size, fpVenc);
                } else {
                    // 收到音频帧
                    fwrite(frame.data, 1, frame.size, fpAenc);
                }
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
```

# 11.智能AI
## 11.6.AI示例
```c++
// 异步AI消息获取示例，调用方式与获取实时流类似
class DemoAI {
public:
    void Create(EuvLoop &loop) {
        // 创建AI监听对象
        m_ai = new MediaClientAiDataAsync(loop);
        // AI的获取不需要配置监听类型和通道，默认监听所有通道
        // 这里使用的方式是只监听消息，不获取实际数据
        // 推荐使用这种方式，避免不需要监听的通道或者类型去执行获取数据的拷贝操作，节约性能
        m_ai->Start(nullptr, std::bind(&DemoAI::ProcAiData, this, ph_1));
    }

    void Destroy() {
        // 销毁
        m_ai->Stop();
        delete m_ai;
    }

private:
    void ProcAiData(MediaAi::AiInfoHeader &header) {
        // 仅处理如下类型数据
        if (header.type != MediaAi::AiTypeE::MotionDetection &&
            header.type != MediaAi::AiTypeE::PersonDetection)
            return;

        // 根据消息获取AI数据
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success)
            return;
        switch (header.type) {
            case MediaAi::AiTypeE::MotionDetection: {
                // 处理移动侦测消息
                auto *data = (MediaAi::AiInfoMotionDetectionData *) header.data;
                // 遍历所有数据，例如：如果检测到运动面积大于10%，就认为是移动事件
                // 另外基于data中的x/y/w/h以及header中的srcPicSize，可以用来定位事件在画面中的位置
                // 可以用来做移动追踪或者检测区域的过滤
                for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoMotionDetectionData); i++) {
                    if (data[i].Area() > header.srcPicSize.w * header.srcPicSize.h * 10 / 100) {
                        emxlogi("motion alarm triggered\n");
                        return;
                    }
                }
                return;
            }
            case MediaAi::AiTypeE::PersonDetection: {
                // 处理移动侦测消息
                auto *data = (MediaAi::AiInfoPersonDetectionData *) header.data;
                // 遍历所有数据，例如：如果检测到运动面积大于10%，就认为是人行事件
                // 另外基于data中的x/y/w/h以及header中的srcPicSize，可以用来定位事件在画面中的位置
                // 可以用来做人行追踪或者检测区域的过滤
                for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoMotionDetectionData); i++) {
                    if (data[i].Area() > header.srcPicSize.w * header.srcPicSize.h * 10 / 100) {
                        emxlogi("person alarm triggered\n");
                        return;
                    }
                }
                return;
            }
//            case MediaAi::AiTypeE::Classify:
//                break;
//            case MediaAi::AiTypeE::FaceRecognition:
//                break;
//            case MediaAi::AiTypeE::PersonRecognition:
//                break;
            default:
                break;
        }
    }

private:
    MediaClientAiDataAsync *m_ai;
};
```
# 12.Region
# 13.视频输入
&emsp;&emsp;可用于配置和获取视频输入参数