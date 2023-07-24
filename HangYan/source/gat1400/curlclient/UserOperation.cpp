#include "UserOperation.hpp"

using namespace Emx;

bool Gat1400FaceUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("UploadFace ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

