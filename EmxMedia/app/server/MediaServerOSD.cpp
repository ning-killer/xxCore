//
// Created by xiong on 2021/8/9.
//

#include "EmxCore.hpp"
#include "MediaServerOSD.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;
std::vector<MediaServerOSD *> MediaServerOSD::m_array;

void MediaServerOSD::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("osd", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeOSD(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerOSD::Destroy() {
    for (auto e : m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerOSD::OnRecvReq(uint32_t module, uint32_t method,
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
        case (uint32_t) MediaParamMethodE::SetParam: {
            auto p = (Param *) data;
            std::vector<std::shared_ptr<Param>> param;
            param.reserve(size / (int) sizeof(Param));
            for (int i = 0; i < size / (int) sizeof(Param); i++) {
                param.push_back(std::make_shared<Param>(*(p + i)));
            }
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            std::vector<std::shared_ptr<Param>> param;
            ErrCodeE e = m_array[chn]->GetParam(param);
            out.clear();
            for (auto &item:param)
                out.append((char *) item.get(), sizeof(Param));
            return e;
        }
        case (uint32_t) MediaParamMethodE::GetParamRange: {
            Json::Value range;
            ErrCodeE e = m_array[chn]->GetParamRange(range);
            out = range.toStyledString();
            return e;
        }
        case (uint32_t) MediaParamMethodE::GetChnNum: {
            int chnNum = (int) m_array.size();
            out.assign((const char *) &chnNum, sizeof(chnNum));
            return ErrCodeE::Success;
        }
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerOSD::OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerOSD::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("osd", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    DestroyAllParam();
    auto &json = root[m_chn];
    if (json.isMember("time"))
        LoadParamTime(json["time"]);
    if (json.isMember("text"))
        LoadParamText(json["text"]);
    if (json.isMember("image"))
        LoadParamImage(json["image"]);
    return ErrCodeE::Success;
}

ErrCodeE MediaServerOSD::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("osd", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    SaveParamTime(json);
    SaveParamText(json);
    SaveParamImage(json);
    root[m_chn] = json;
    return m_res.configs.SetParam("osd", root);
}

ErrCodeE MediaServerOSD::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("osd", range);
}


bool MediaServerOSD::ParamValid(const std::vector<std::shared_ptr<Param>> &param) {
    return true;
}

OutlineOsd *MediaServerOSD::CreateOutline(int fontSize, const char *fontPath) {
    for (auto &e : m_outline) {
        if (e->fontSize == fontSize && strcmp(e->fontPath, fontPath) == 0) {
            e->refCnt++;
            emxlogt("reference %p count %d\n", &e->outline, e->refCnt);
            return &e->outline;
        }
    }
    auto outline = new OutlineOsdCfg;
    outline->refCnt = 1;
    outline->fontSize = fontSize;
    strncpy(outline->fontPath, fontPath, sizeof(outline->fontPath));
    outline->outline.Init(fontSize, fontPath);
    m_outline.push_back(outline);
    emxlogt("create %p\n", &outline->outline);
    return &outline->outline;
}

void MediaServerOSD::DestroyOutline(OutlineOsd *outline) {
    for (auto iter = m_outline.begin(); iter != m_outline.end(); iter++) {
        auto e = (*iter);
        if (&e->outline == outline) {
            e->refCnt--;
            emxlogt("reference %p count %d\n", &e->outline, e->refCnt);
            if (e->refCnt == 0) {
                emxlogt("destroy %p\n", outline);
                delete e;
                m_outline.erase(iter);
            }
            return;
        }
    }
    emxloge("untrace %p\n", outline);
}

void MediaServerOSD::LoadParamTime(Json::Value &json) {
    for (auto &item : json) {
        auto param = std::make_shared<Param>();
        memset(param.get(), 0, sizeof(Param));
        param->type = MediaOSD::TypeE::Time;
        auto elem = &param->time;
        elem->ena = item["ena"].asBool();
        elem->size = item["size"].asInt();
        elem->thickness = item["thickness"].asInt();
        elem->layer = item["layer"].asInt();
        strncpy(elem->font, item["font"].asCString(), sizeof(elem->font));
        elem->format = (TimeFormatE) item["format"].asInt();
        elem->margin.horizon = (MediaOSD::HAlignE) item["margin"]["horizon"]["align"].asInt();
        elem->margin.rateX = item["margin"]["horizon"]["rate"].asInt();
        elem->margin.vertical = (MediaOSD::VAlignE) item["margin"]["vertical"]["align"].asInt();
        elem->margin.rateY = item["margin"]["vertical"]["rate"].asInt();
        m_paramTime.push_back(elem);
        m_param.push_back(param);
    }
}

void MediaServerOSD::SaveParamTime(Json::Value &json) {
    Json::Value array;
    array.resize(0);
    for (auto param : m_paramTime) {
        Json::Value time;
        time["ena"] = param->ena;
        time["size"] = param->size;
        time["thickness"] = param->thickness;
        time["layer"] = param->layer;
        time["font"] = param->font;
        time["format"] = (int) param->format;
        Json::Value horizon;
        horizon["rate"] = param->margin.rateX;
        horizon["align"] = (int) param->margin.horizon;
        Json::Value vertical;
        vertical["rate"] = param->margin.rateY;
        vertical["align"] = (int) param->margin.vertical;
        Json::Value margin;
        margin["horizon"] = horizon;
        margin["vertical"] = vertical;
        time["margin"] = margin;
        array.append(time);
    }
    json["time"] = array;
}

void MediaServerOSD::LoadParamText(Json::Value &json) {
    for (auto &item : json) {
        auto param = std::make_shared<Param>();
        memset(param.get(), 0, sizeof(Param));
        param->type = MediaOSD::TypeE::Text;
        auto elem = &param->text;
        elem->ena = item["ena"].asBool();
        elem->size = item["size"].asInt();
        elem->thickness = item["thickness"].asInt();
        elem->layer = item["layer"].asInt();
        strncpy(elem->font, item["font"].asCString(), sizeof(elem->font));
        strncpy(elem->content, item["content"].asCString(), sizeof(elem->content));
        elem->margin.horizon = (MediaOSD::HAlignE) item["margin"]["horizon"]["align"].asInt();
        elem->margin.rateX = item["margin"]["horizon"]["rate"].asInt();
        elem->margin.vertical = (MediaOSD::VAlignE) item["margin"]["vertical"]["align"].asInt();
        elem->margin.rateY = item["margin"]["vertical"]["rate"].asInt();
        m_paramText.push_back(elem);
        m_param.push_back(param);
    }
}

void MediaServerOSD::SaveParamText(Json::Value &json) {
    Json::Value array;
    array.resize(0);
    for (auto param : m_paramText) {
        Json::Value text;
        text["ena"] = param->ena;
        text["size"] = param->size;
        text["thickness"] = param->thickness;
        text["layer"] = param->layer;
        text["font"] = param->font;
        text["content"] = param->content;
        Json::Value horizon;
        horizon["rate"] = param->margin.rateX;
        horizon["align"] = (int) param->margin.horizon;
        Json::Value vertical;
        vertical["rate"] = param->margin.rateY;
        vertical["align"] = (int) param->margin.vertical;
        Json::Value margin;
        margin["horizon"] = horizon;
        margin["vertical"] = vertical;
        text["margin"] = margin;
        array.append(text);
    }
    json["text"] = array;
}

void MediaServerOSD::LoadParamImage(Json::Value &json) {
    for (auto &item : json) {
        auto param = std::make_shared<Param>();
        memset(param.get(), 0, sizeof(Param));
        param->type = MediaOSD::TypeE::Image;
        auto elem = &param->image;
        elem->ena = item["ena"].asBool();
        elem->layer = item["layer"].asInt();
        elem->width = item["width"].asInt();
        elem->height = item["height"].asInt();
        elem->alpha = item["alpha"].asInt();
        strncpy(elem->path, item["path"].asCString(), sizeof(elem->path));
        elem->margin.horizon = (MediaOSD::HAlignE) item["margin"]["horizon"]["align"].asInt();
        elem->margin.rateX = item["margin"]["horizon"]["rate"].asInt();
        elem->margin.vertical = (MediaOSD::VAlignE) item["margin"]["vertical"]["align"].asInt();
        elem->margin.rateY = item["margin"]["vertical"]["rate"].asInt();
        m_paramImage.push_back(elem);
        m_param.push_back(param);
    }
}

void MediaServerOSD::SaveParamImage(Json::Value &json) {
    Json::Value array;
    array.resize(0);
    for (auto param : m_paramImage) {
        Json::Value image;
        image["ena"] = param->ena;
        image["layer"] = param->layer;
        image["width"] = param->width;
        image["height"] = param->height;
        image["alpha"] = param->alpha;
        image["path"] = param->path;
        Json::Value horizon;
        horizon["rate"] = param->margin.rateX;
        horizon["align"] = (int) param->margin.horizon;
        Json::Value vertical;
        vertical["rate"] = param->margin.rateY;
        vertical["align"] = (int) param->margin.vertical;
        Json::Value margin;
        margin["horizon"] = horizon;
        margin["vertical"] = vertical;
        image["margin"] = margin;
        array.append(image);
    }
    json["image"] = array;
}

void MediaServerOSD::GetTime(time_t time, MediaOSD::TimeFormatE format, int maxSize, char *content) {
    const char *week[7] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

    struct tm xtm = {};
    localtime_r(&time, &xtm);
    switch (format) {
        case TimeFormatE::Normal:
            snprintf(content, maxSize, "%04d-%02d-%02d %02d:%02d:%02d",
                     (1900 + xtm.tm_year), (1 + xtm.tm_mon), xtm.tm_mday,
                     xtm.tm_hour, xtm.tm_min, xtm.tm_sec);
            break;
        case TimeFormatE::NormalWithZhWeek: {
            snprintf(content, maxSize, "%04d-%02d-%02d %s %02d:%02d:%02d",
                     (1900 + xtm.tm_year), (1 + xtm.tm_mon), xtm.tm_mday,
                     week[xtm.tm_wday],
                     xtm.tm_hour, xtm.tm_min, xtm.tm_sec);
            break;
        }
        default:
            break;
    }
}

void MediaServerOSD::CalcCoordinate(Margin &margin,
                                    int regionWidth, int regionHeight,
                                    int backgroundWidth, int backgroundHeight,
                                    int &x, int &y) {
    //计算x方向的偏移量
    int xOff = margin.rateX * backgroundWidth / RateBase;
    if (margin.horizon == MediaOSD::HAlignE::Left)
        x = xOff;
    else if (margin.horizon == MediaOSD::HAlignE::Right)
        x = backgroundWidth - regionWidth - xOff;
    else
        x = ((backgroundWidth - regionWidth) >> 1) + xOff;
    if (x < 0)
        x = 0;
    if (x + regionWidth > backgroundWidth)
        x = backgroundWidth - regionWidth;
    //x should be align by 2 on overlayEx mode
    x = (x >> 1) << 1;

    //计算y方向的偏移量
    int yOff = margin.rateY * backgroundHeight / RateBase;
    if (margin.vertical == MediaOSD::VAlignE::Top)
        y = yOff;
    else if (margin.vertical == MediaOSD::VAlignE::Bottom)
        y = backgroundHeight - regionHeight - yOff;
    else
        y = ((backgroundHeight - regionHeight) >> 1) + yOff;
    if (y < 0)
        y = 0;
    if (y + regionHeight > backgroundHeight)
        y = backgroundHeight - regionHeight;
}