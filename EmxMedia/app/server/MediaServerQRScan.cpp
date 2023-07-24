//
// Created by xiong on 2021/6/25.
//
#include "EmxCore.hpp"
#include "MediaServerQRScan.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"
#include "zbar.h"
#include "PanoQrPreProcess.hpp"

using namespace Emx;

std::vector<MediaServerQRScan *> MediaServerQRScan::m_array;

void MediaServerQRScan::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("qrScan", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeQRScan(chn, res);
        m_array.push_back(e);
    }
}

void MediaServerQRScan::Destroy() {
    for (auto e : m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerQRScan::OnRecvReq(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out) {
    int32_t chn;
    memcpy(&chn, data, sizeof(chn));
    if (chn >= (int32_t) m_array.size()) {
        emxloge("chn %d not exists\n", chn);
        return ErrCodeE::ResNotExist;
    }
    data += sizeof(chn);
    size -= sizeof(chn);
    switch (method) {
        case (uint32_t) MediaQrScanMethodE::Start:
            return m_array[chn]->Start();
        case (uint32_t) MediaQrScanMethodE::Stop:
            return m_array[chn]->Stop();
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerQRScan::OnRecvReqSub(uint32_t module, uint32_t method,
                                         const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerQRScan::CreateDecoder(int imageWidth, int imageHeight) {
    m_width = imageWidth;
    m_height = imageHeight;
    emxlogd("chn %d image size [%d,%d]\n", m_chn, m_width, m_height);
    Json::Value root;
    if (m_res.configs.GetCfg("qrDecoder", root) != ErrCodeE::Success) {
        emxloge("cannot read qrDecoder.json\n");
        return ErrCodeE::OpenFailed;
    }
    auto json = root[m_chn];
    m_type = (QrTypeE) json["type"].asInt();
    switch (m_type) {
        case QrTypeE::Panorama:
            return CreatePanoPreProcess(json);
        default:
            break;
    }
    return ErrCodeE::Success;
}

void MediaServerQRScan::DestroyDecoder() {
    switch (m_type) {
        case QrTypeE::Panorama:
            DestroyPanoPreProcess();
            break;
        default:
            break;
    }
}

ErrCodeE MediaServerQRScan::Decode(uint8_t *data, std::string &content) {
    switch (m_type) {
        case QrTypeE::Panorama: {
            unsigned char *dataY;
            int res = m_pano->preProcess(data, nullptr);
            if (res != 1) {
                emxlogt("chn %d preProcess failed\n", m_chn);
                return ErrCodeE::Failure;
            }
            PanoQrPreProcess::EpRect rect;
            dataY = m_pano->GetRemapDataY(&rect);
            if ((rect.w < 0) || (rect.h < 0)) {
                emxlogt("chn %d GetRemapDataY failed\n", m_chn);
                return ErrCodeE::Failure;
            }
            return DoDecode(dataY, rect.w, rect.h, content);
        }
        default:
            break;
    }
    return DoDecode(data, m_width, m_height, content);
}

ErrCodeE MediaServerQRScan::DoDecode(void *data, int w, int h, std::string &content) {
    /* create a reader */
    auto scanner = zbar::zbar_image_scanner_create();

    /* configure the reader */
    zbar::zbar_image_scanner_set_config(scanner, zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_POSITION, 1);

    /* wrap image data */
    auto image = zbar::zbar_image_create();
    zbar::zbar_image_set_format(image, *(int *) "Y800");
    zbar::zbar_image_set_size(image, w, h);

    zbar::zbar_image_set_data(image, data, w * h, nullptr);

    int n = zbar_scan_image(scanner, image);
    if (n > 0) {
        /* extract results */
        const zbar::zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        for (; symbol; symbol = zbar::zbar_symbol_next(symbol)) {
            const char *out = zbar::zbar_symbol_get_data(symbol);
            content.assign(out);
            zbar::zbar_image_destroy(image);
            zbar::zbar_image_scanner_destroy(scanner);
            return ErrCodeE::Success;
        }
    }
    zbar::zbar_image_destroy(image);
    zbar::zbar_image_scanner_destroy(scanner);
    return ErrCodeE::Failure;
}


ErrCodeE MediaServerQRScan::CreatePanoPreProcess(Json::Value &json) {
    PanoQrPreProcess::EpSize imgSize;
    imgSize.w = m_width;
    imgSize.h = m_height;

    PanoQrPreProcess::EpPoint centerL = {};
    PanoQrPreProcess::EpPoint centerR = {};

    char mapPath[EMX_MAX_PATH_SIZE] = {};
    snprintf(mapPath, sizeof(mapPath), "%s/map/%s", m_res.configs.GetDir(), json["map"]["x"].asCString());
    if (File::Read(mapPath, m_mapX) != ErrCodeE::Success) {
        emxloge("chn %d read map x %s failed\n", mapPath, m_chn);
        return ErrCodeE::Failure;
    }
    snprintf(mapPath, sizeof(mapPath), "%s/map/%s", m_res.configs.GetDir(), json["map"]["y"].asCString());
    if (File::Read(mapPath, m_mapY) != ErrCodeE::Success) {
        emxloge("chn %d read map y %s failed\n", mapPath, m_chn);
        return ErrCodeE::Failure;
    }
    Param param("pano/center");
    Json::Value center;
    if (param.Get(center) == ErrCodeE::Success) {
        centerL.x = center["left"]["x"].asInt();
        centerL.y = center["left"]["y"].asInt();
        centerR.x = center["right"]["x"].asInt();
        centerR.y = center["right"]["y"].asInt();
        emxlogd("center:LX[%d] LY[%d] RX[%d] RY[%d]\n", centerL.x, centerL.y, centerR.x, centerR.y);
    } else {
        centerL.x = m_width / 4;
        centerL.y = m_height / 2;
        centerR.x = m_width / 4 * 3;
        centerR.y = m_height / 2;
        emxloge("use default center:LX[%d] LY[%d] RX[%d] RY[%d]\n", centerL.x, centerL.y, centerR.x, centerR.y);
    }

    m_pano = new PanoQrPreProcess;
    m_pano->preProcessInitWithMap(imgSize, centerL, centerR,
                                  (short *) m_mapX.data(), (short *) m_mapY.data(),
                                  json["mirror"].asBool());
    emxlogd("panorama QR preprocess done\n");
    return ErrCodeE::Success;
}

void MediaServerQRScan::DestroyPanoPreProcess() {
    if (m_pano) {
        m_pano->FreeAllBuffer();
        delete m_pano;
        m_pano = nullptr;
    }
}