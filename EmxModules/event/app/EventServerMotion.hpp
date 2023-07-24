/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventServerMotion_HPP
#define EMX_EventServerMotion_HPP

#include "EventMotion.hpp"
#include "EventResource.hpp"
#include "NightVisionClient.hpp"
#include "EmxMedia.hpp"
#include "EventPluginBase.hpp"

namespace Emx {

    class EventServerMotion : public EventMotion, EventPluginBase {
    public:
        EventServerMotion(int chn, EventResource &res) : EventMotion(chn),
                                                         EventPluginBase("motion", Event::TypeE::Motion, chn, res) {}

        static void Create(EventResource &res);

        static void Destroy();

        static EventServerMotion *Get(int chn) {
            if ((int) m_array.size() > chn)
                return m_array[chn];
            return nullptr;
        }

        static ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                                  const char *data, int32_t size, std::string &out);

    private:
        ErrCodeE GetParam(std::shared_ptr<Param> &param) override;

        ErrCodeE SetParam(const std::shared_ptr<Param> &param) override;

    private:
        ErrCodeE Start();

        void Stop();

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

        ErrCodeE LoadConfig();

        void InitializeCtx();

        void ProcAiData(MediaAi::AiInfoHeader &header);

    private:
        static std::vector<EventServerMotion *> m_array;
        std::shared_ptr<Param> m_param;
        int m_sensitivityAreaMap[10];
        bool m_delayProcessing;//延迟判断中
        EuvTimer m_detectDelayTimer;
        NightVisionClientInfo m_night;//用于检测夜视切换状态
        NightVision::Status m_nightStatus;//用于存储夜视切换状态
        MediaClientAiDataAsync *m_ai;
    };

}

#endif //EMX_EventServerMotion_HPP
