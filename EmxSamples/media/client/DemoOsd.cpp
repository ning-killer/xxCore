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

void Filter(std::string& text, int limit) {
    if ((int)text.size() < limit) {
        return;
    }
    int index = 0;
    for (;;) {
        unsigned char c = static_cast<unsigned char>(text[index]);
        int charLength = 1; 
        if (c <= 127) {
        } else if (c >= 192 && c <= 247) {
            charLength = (c <= 223) ? 2 : ((c <= 239) ? 3 : 4);
        }
        if (index + charLength >= limit) {
            break;
        }
        index += charLength;
    }
    text = text.substr(0, index);
}

void RunMediaClientOSD(const Json::Value &data) {
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
    for (auto iter = osdParam.begin(); iter != osdParam.end();) {
        if ((*iter)->type == MediaOSD::TypeE::Text)
            iter = osdParam.erase(iter);
        else
            iter++;
    }
    try {
        for (auto text : data["text"]) {
            auto param = std::make_shared<MediaOSD::Param>();
            memset(param.get(), 0, sizeof(MediaOSD::Param));
            param->type = MediaOSD::TypeE::Text;
            std::string s_text = text["content"].asString();
            Filter(s_text, 256);
            memcpy(param->text.content, s_text.c_str(), s_text.size());
            param->text.margin.horizon = (MediaOSD::HAlignE)text["margin"]["horizon"]["align"].asInt();
            param->text.margin.rateX = text["margin"]["horizon"]["rate"].asInt();
            param->text.margin.vertical = (MediaOSD::VAlignE)text["margin"]["vertical"]["align"].asInt();
            param->text.margin.rateY = text["margin"]["vertical"]["rate"].asInt();
            param->text.thickness = 1;
            param->text.ena = true;
            param->text.size = text["size"].asInt();
            param->text.layer = text["layer"].asInt();
            memcpy(param->text.font, text["font"].asString().c_str(), sizeof(param->text.font));
            osdParam.emplace_back(param);
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }
    
    if (osdClient.SetParam(osdParam) != ErrCodeE::Success) {
        emxloge("osd GetParam failed.\n");
        return;
    }
    return;
}

// DemoOsd textFile note: x，y坐标基准1000传入
// {
// 	"text": [
// 		{
// 			"content": "你好",
// 			"ena": true,
// 			"font": "font.ttf",
// 			"layer": 0,
// 			"margin": {
// 				"horizon": {
// 					"align": 0,
// 					"rate": 30
// 				},
// 				"vertical": {
// 					"align": 0,
// 					"rate": 905
// 				}
// 			},
// 			"size": 56,
// 			"thickness": 1
// 		}
// 	]
// }

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [DemoOsd textFile]\n");
        return -1;
    }
    std::string textfile = std::string(argv[1]);
    
    Json::Value dataJson;
    if (EasyJson::Load(textfile.c_str(), dataJson) == ErrCodeE::Failure) {
        emxloge("read %s failed\n", textfile.c_str());
        return -1;
    }
    RunMediaClientOSD(dataJson);
    return 0;
}