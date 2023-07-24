//
// Created by xiong on 2021/6/25.
//

#ifndef EMX_MediaServerQRScan_HPP
#define EMX_MediaServerQRScan_HPP

#include "media/MediaQRScan.hpp"
#include "MediaServerResource.hpp"

namespace Emx {
    class PanoQrPreProcess;

    class MediaServerQRScan : public MediaQRScan {
    public:
        explicit MediaServerQRScan(int32_t chn, MediaServerResource &res)
                : MediaQRScan(chn), m_res(res), m_started(false) {}

        virtual ~MediaServerQRScan() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerQRScan *Get(int chn) {
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

    protected:
        ErrCodeE CreateDecoder(int imageWidth, int imageHeight);

        void DestroyDecoder();

        ErrCodeE Decode(uint8_t *data, std::string &content);

    private:
        static ErrCodeE DoDecode(void *data, int w, int h, std::string &content);

        ErrCodeE CreatePanoPreProcess(Json::Value &json);

        void DestroyPanoPreProcess();


    protected:
        static std::vector<MediaServerQRScan *> m_array;
        MediaServerResource &m_res;
        bool m_started;
        int m_width;
        int m_height;
        enum class QrTypeE : uint8_t {
            Normal,
            Panorama,
        } m_type;
        PanoQrPreProcess *m_pano;
        std::string m_mapX;
        std::string m_mapY;
    };
}

#endif //EMX_MediaServerQRScan_HPP
