//
// Created by xiong on 2021/8/9.
//

#ifndef EMX_MediaServerOSD_HPP
#define EMX_MediaServerOSD_HPP

#include "media/MediaOSD.hpp"
#include "MediaServerResource.hpp"
#include "OutlineOsd.h"

namespace Emx {
    class MediaFactory;

    class MediaServerOSD : public MediaOSD {
    public:
        explicit MediaServerOSD(int32_t chn, MediaServerResource &res)
                : MediaOSD(chn), m_res(res) {}

        virtual ~MediaServerOSD() {
            DestroyAllOutline();
            DestroyAllParam();
        };

        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerOSD *Get(int chn) {
            if ((int) m_array.size() > chn)
                return m_array[chn];
            return nullptr;
        }

        static ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                                  const char *data, int32_t size, std::string &out);

    public:
        //note: the data and the size are not include the chn
        virtual ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out);

        virtual ErrCodeE Start() = 0;

        virtual ErrCodeE Stop() = 0;

        ErrCodeE GetParamRange(Json::Value &range) override;

    protected:
        bool ParamValid(const std::vector<std::shared_ptr<Param>> &param);

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

    protected:

        void JoinFontPath(const char *name, char *path) {
            snprintf(path, EMX_MAX_PATH_SIZE, "%s/font/%s", m_res.configs.GetDir(), name);
        }

        void LoadParamTime(Json::Value &json);

        void SaveParamTime(Json::Value &json);


        void LoadParamText(Json::Value &json);

        void SaveParamText(Json::Value &json);


        void LoadParamImage(Json::Value &json);

        void SaveParamImage(Json::Value &json);

        OutlineOsd *CreateOutline(int fontSize, const char *fontPath);

        void DestroyOutline(OutlineOsd *outline);

        void DestroyAllOutline() {
            for (auto e :m_outline)
                delete e;
            m_outline.clear();
        }

        void DestroyAllParam() {
            m_param.clear();
            m_paramTime.clear();
            m_paramText.clear();
            m_paramImage.clear();
        }

        static void GetTime(time_t time, TimeFormatE format, int maxSize, char *content);

        static void CalcCoordinate(Margin &margin,
                                   int regionWidth, int regionHeight,
                                   int backgroundWidth, int backgroundHeight,
                                   int &x, int &y);

    protected:
        static std::vector<MediaServerOSD *> m_array;
        MediaServerResource &m_res;
        std::vector<ParamTime *> m_paramTime;
        std::vector<ParamText *> m_paramText;
        std::vector<ParamImage *> m_paramImage;
        std::vector<std::shared_ptr<Param>> m_param;
        struct OutlineOsdCfg {
            OutlineOsd outline;
            int fontSize;
            char fontPath[EMX_MAX_PATH_SIZE];
            int refCnt;
        };
        std::list<OutlineOsdCfg *> m_outline;
    };

}


#endif //EMX_MediaServerOSD_HPP
