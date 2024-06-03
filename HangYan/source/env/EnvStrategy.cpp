/*
 * @Author: xiong
 * @Date: 2022/10/19
*/

#include "EnvStrategy.hpp"

using namespace Emx;

ErrCodeE EnvStrategy::Parse(const Json::Value &json) {
    try {
        auto &speechJson = json["speech"];
        speech.ena = speechJson["ena"].asBool();
        speech.volume = speechJson["volume"].asInt();
        speech.repeat = speechJson["repeat"].asInt();
        strncpy(speech.url, speechJson["url"].asCString(), sizeof(speech.url));
        speech.playVoiceA = speechJson["playVoiceA"].asBool();

        auto &lightJson = json["light"];
        light.ena = lightJson["ena"].asBool();
        light.mode = (Light::ModeE) lightJson["mode"].asInt();
        light.duration = lightJson["duration"].asInt();

        auto &buzzJson = json["buzz"];
        buzz.ena = buzzJson["ena"].asBool();
        buzz.duration = buzzJson["duration"].asInt();
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("parse failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE EnvStrategy::Serialize(Json::Value &json) {
    try {
        auto &speechJson = json["speech"];
        speechJson["ena"] = speech.ena;
        speechJson["volume"] = speech.volume;
        speechJson["repeat"] = speech.repeat;
        speechJson["url"] = speech.url;
        speechJson["playVoiceA"] = speech.playVoiceA;

        auto &lightJson = json["light"];
        lightJson["ena"] = light.ena;
        lightJson["mode"] = (int) light.mode;
        lightJson["duration"] = light.duration;

        auto &buzzJson = json["buzz"];
        buzzJson["ena"] = buzz.ena;
        buzzJson["duration"] = buzz.duration;
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("serialize failed\n");
        return ErrCodeE::Failure;
    }
}
