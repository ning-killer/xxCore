//
// Created by xiong on 2022/4/27.
//

#ifndef EMX_MediaChipAi_HPP
#define EMX_MediaChipAi_HPP


#include "AudioCodec.hpp"
#include "MediaServerAi.hpp"
#include "MediaChipAiPlug.hpp"
#include "sample_comm.h"

namespace Emx {

    class MediaChipAi : public MediaServerAi {
    public:
        MediaChipAi(int32_t chn, MediaServerResource &res) :
                MediaServerAi(chn, res),
                m_started(false), m_quit(false) {}

        virtual ~MediaChipAi() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

    protected:

        virtual void AddAiPlug();

        virtual void ClearAiPlug();

        virtual void StartWork(Json::Value &json);

        virtual void StopWork();

        virtual void OnWork(void *arg);

        virtual void OnWorkComplete(ErrCodeE e, void *arg);

    protected:
        struct Work {
            Work(int grp, int chn) : vpssGrp(grp), vpssChn(chn) {}

            int vpssGrp;
            int vpssChn;
            std::list<MediaChipAiPlug::OnFrameCallBack> cbFrameList;
            std::list<MediaChipAiPlug::OnCompleteCallBack> cbCompleteList;
            EuvWork work;
        };

        bool m_started;
        bool m_quit;
        std::list<Work *> m_workList;
        std::list<MediaChipAiPlug *> m_plugList;
        MediaChipAiHandle *m_handle;
    };
}

#endif //EMX_MediaChipAi_HPP
