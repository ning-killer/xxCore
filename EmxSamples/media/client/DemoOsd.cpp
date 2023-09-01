/**
 * @file   DemoOsd.cpp
 * @author hening
 * @brief  媒体服务OSD功能单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

void RunMediaClientOSD() {
    int num;
    if (MediaClientOSD::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("osd GetChnNum failed, num(%d).\n", num);
        return;
    }

    MediaClientOSD osdClient(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    if (osdClient.GetParam(osdParam) != ErrCodeE::Success) {
        emxloge("osd GetParam failed.\n");
        return;
    }
    Json::Value chn;
    chn["time"].resize(0);
    chn["text"].resize(0);
    chn["image"].resize(0);
    for (auto &item : osdParam) {
        if (item->type == MediaOSD::TypeE::Time) {
            Json::Value json;
            json["ena"] = item->time.ena;
            json["size"] = item->time.size;
            json["thickness"] = item->time.thickness;
            json["layer"] = item->time.layer;
            json["font"] = item->time.font;
            json["format"] = (int) item->time.format;
            json["margin"]["horizon"]["rate"] = item->time.margin.rateX;
            json["margin"]["horizon"]["align"] = (int) item->time.margin.horizon;
            json["margin"]["vertical"]["rate"] = item->time.margin.rateY;
            json["margin"]["vertical"]["align"] = (int) item->time.margin.vertical;
            chn["time"].append(json);
        } else if (item->type == MediaOSD::TypeE::Text) {
            Json::Value json;
            json["ena"] = item->text.ena;
            json["size"] = item->text.size;
            json["thickness"] = item->text.thickness;
            json["layer"] = item->text.layer;
            json["font"] = item->text.font;
            json["content"] = item->text.content;
            json["margin"]["horizon"]["rate"] = item->text.margin.rateX;
            json["margin"]["horizon"]["align"] = (int) item->text.margin.horizon;
            json["margin"]["vertical"]["rate"] = item->text.margin.rateY;
            json["margin"]["vertical"]["align"] = (int) item->text.margin.vertical;
            chn["text"].append(json);
        } else if (item->type == MediaOSD::TypeE::Image) {
            Json::Value json;
            json["ena"] = item->image.ena;
            json["layer"] = item->image.layer;
            json["width"] = item->image.width;
            json["height"] = item->image.height;
            json["alpha"] = item->image.alpha;
            json["path"] = item->image.path;
            json["margin"]["horizon"]["rate"] = item->image.margin.rateX;
            json["margin"]["horizon"]["align"] = (int) item->image.margin.horizon;
            json["margin"]["vertical"]["rate"] = item->image.margin.rateY;
            json["margin"]["vertical"]["align"] = (int) item->image.margin.vertical;
            chn["image"].append(json);
        }
    }
    emxlogd("osd message chn0: %s\n", chn.toStyledString().c_str());

    int text_rateX = 100;
    int text_rateY = 100;
    for (auto &item : osdParam) {
        if (item->type == MediaOSD::TypeE::Time) {
            item->text.ena = true;
            item->text.margin.vertical = MediaOSD::VAlignE::Top;
            item->text.margin.rateY = 0;
            item->text.margin.horizon = MediaOSD::HAlignE::Left;
            item->text.margin.rateX= 0;
        } else if (item->type == MediaOSD::TypeE::Text) {
            item->text.ena = true;
            item->text.margin.vertical = MediaOSD::VAlignE::Top;
            item->text.margin.rateY = text_rateY;
            text_rateY += 100;
            item->text.margin.horizon = MediaOSD::HAlignE::Left;
            item->text.margin.rateX= text_rateX;
            text_rateX += 200; 
        } else if (item->type == MediaOSD::TypeE::Image) {
            memset(item->image.path, 0 ,sizeof(item->image.path));
            snprintf(item->image.path, sizeof(item->image.path) - 1, "/customer/nfs/code-projects/Platform/MMC233Z/firmware/app/configs/logo/2560x1440.rgba");
            item->text.margin.vertical = MediaOSD::VAlignE::Top;
            item->text.margin.rateY = 500;
            item->text.margin.horizon = MediaOSD::HAlignE::Left;
            item->text.margin.rateX= 500;
        }
    }
    if (osdClient.SetParam(osdParam) != ErrCodeE::Success) {
        emxloge("osd GetParam failed.\n");
        return;
    }
    return;
}

int main(int argc, char *argv[]) {
    RunMediaClientOSD();
    return 0;
}