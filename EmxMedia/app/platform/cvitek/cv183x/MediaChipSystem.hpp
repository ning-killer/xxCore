#ifndef EMX_MediaChipSystem_HPP
#define EMX_MediaChipSystem_HPP

#include "sample_comm.h"
#include "MediaServerSystem.hpp"

namespace Emx {

    class MediaChipSystem : public MediaServerSystem {
    public:
        MediaChipSystem(MediaServerResource &res) : MediaServerSystem(res), m_started(false) {}

        virtual ~MediaChipSystem() {}

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

    protected:
        virtual ErrCodeE InitVb(const Json::Value &json);

        virtual void DeInitVb(const Json::Value &json);

        virtual ErrCodeE InitVi(const Json::Value &json);

        virtual void DeInitVi(const Json::Value &json);

        virtual ErrCodeE InitVpss(const Json::Value &json);

        virtual void DeInitVpss(const Json::Value &json);

        virtual ErrCodeE InitExpandMode(const Json::Value &json);

        virtual void DeInitExpandMode(const Json::Value &json);

        virtual ErrCodeE SetViVpssMode(const Json::Value &json);

        virtual ErrCodeE CreateVpssGroup(const Json::Value &json);

        virtual ErrCodeE CreatePipe(const Json::Value &json);

        virtual void DestroyVpssGroup(const Json::Value &json);

        virtual void DestroyPipe(const Json::Value &json);

    protected:
        bool m_started;
    private:
        SAMPLE_VI_CONFIG_S m_viCfg;
    };
}


#endif //EMX_MediaChipSystem_HPP
