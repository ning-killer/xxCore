/**
 * @file   DemoAi.cpp
 * @author hening
 * @brief  媒体服务算法ai单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

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

int main(int argc, char *argv[]) {
    return 0;
}