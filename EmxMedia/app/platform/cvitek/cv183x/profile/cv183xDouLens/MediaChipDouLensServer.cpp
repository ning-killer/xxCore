//
// Created by cx on 2023/4/21.
//

#include <core/EmxLog.hpp>
#include <MediaInner.hpp>
#include <core/EmxParam.hpp>
#include "MediaChipDouLensServer.hpp"

using namespace Emx;
ErrCodeE MediaChipDouLensServer::OnRecvReqSub(uint32_t module, uint32_t method, const char *data, int32_t size,
                                          std::string &out) {
    emxlogt("module[%u],method[%u],data size=%d\n", module, method, size);
    if ((MediaModE) module == MediaModE::Server) {
        if (method == (uint32_t) MediaDoubleLensMethodE::SwitchMode) {

            Json::Value modeJson, dataJson, sceneJson;
            EasyJson::Parse(data, size, dataJson);
            Param paramScene("media/scene");
            paramScene.Get(sceneJson);
            DestroyMedia();
            if (dataJson["mode"].asInt()) {
                Param param("media/stitchMode");
                param.Get(modeJson);
                modeJson["mode"] = dataJson["mode"].asInt();
                param.Set(modeJson);

                if (sceneJson["scene"].asString() != "normal4M_S") {
                    sceneJson["scene"] = "normal4M_S";
                    paramScene.Set(sceneJson);
                    m_res.configs.SetScene("normal4M_S");
                }

                Json::Value jsonVenc;
                m_res.configs.GetParam("venc", jsonVenc);

                for (auto &jsonAttr: modeJson["attr"]) {
                    if (jsonAttr["mode"] == modeJson["mode"]) {
                        jsonVenc[0]["width"] = jsonAttr["u32MaxW"].asInt();
                        jsonVenc[0]["height"] = jsonAttr["u32MaxH"].asInt();
                    }
                }
                m_res.configs.SetParam("venc", jsonVenc);

            } else {
                sceneJson["scene"] = "normal4M_N";
                paramScene.Set(sceneJson);
                m_res.configs.SetScene("normal4M_N");
            }
            out = data;
            CreateMedia();
            return ErrCodeE::Success;
        } else if (method == (uint32_t) MediaDoubleLensMethodE::SetOutMode) {

            Json::Value jsonData, jsonMode, jsonVenc, sceneJson;

            EasyJson::Parse(data, size, jsonData);
            Param param("media/stitchMode");
            param.Get(jsonMode);
            jsonMode["attr"] = jsonData;
            jsonMode["mode"] = 2;
            param.Set(jsonMode);

            Param paramScene("media/scene");
            paramScene.Get(sceneJson);
            sceneJson["scene"] = "normal4M_S";
            paramScene.Set(sceneJson);
            m_res.configs.SetScene("normal4M_S");

            m_res.configs.GetParam("venc", jsonVenc);
            for (auto &jsonAttr: jsonData) {
                if (jsonAttr["mode"] == 2) {
                    jsonVenc[0]["width"] = jsonAttr["u32MaxW"].asInt();
                    jsonVenc[0]["height"] = jsonAttr["u32MaxH"].asInt();
                }
            }
            m_res.configs.SetParam("venc", jsonVenc);
            out = data;
            return ErrCodeE::Success;

        } else if (method == (uint32_t) MediaDoubleLensMethodE::GetOutMode) {

            Json::Value modeJson;
            std::string strData;
            Param param("media/stitchMode");
            param.Get(modeJson);

            ErrCodeE e = EasyJson::Serialize(modeJson["attr"], strData);
            if (e != ErrCodeE::Success) {
                emxlogc("Serialize param failed : %d\n", e);
                return e;
            }
            out = strData;

            return ErrCodeE::Success;
        } else if (method == (uint32_t) MediaDoubleLensMethodE::DeleteMode) {

            Json::Value dataJson, sceneJson, jsonMode;

            EasyJson::Parse(data, size, dataJson);
            if (dataJson["delete"].asInt() == 0)
                Cmd::RunCheck("rm -rf /root/configs/dat/*;sync");

            Param paramScene("media/scene");
            paramScene.Get(sceneJson);
            sceneJson["scene"] = "normal4M_N";
            paramScene.Set(sceneJson);
            m_res.configs.SetScene("normal4M_N");

            Param param("media/stitchMode");
            param.Get(jsonMode);
            jsonMode["attr"] = Json::nullValue;
            jsonMode["mode"] = 0;
            param.Set(jsonMode);

            out = data;
            if (dataJson["reboot"].asBool())
                Cmd::RunCheck("reboot");
            return ErrCodeE::Success;
        }
    }
    return MediaChipServer::OnRecvReqSub(module, method, data, size, out);
}
