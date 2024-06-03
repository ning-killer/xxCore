/*
* @Author: xiong
* @Date: 2022/9/22
*/

#include "EnvBurn.hpp"

using namespace Emx;

ErrCodeE EnvBurn::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    OVDLoginPassword = json["OVDLoginPassword"].asString();
    OVDMediaEncPassword = json["OVDMediaEncPassword"].asString();
    cmei = json["cmei"].asString();
    andlinkID = json["andlinkID"].asString();
    andlinkKey = json["andlinkKey"].asString();
    return ErrCodeE::Success;
}

ErrCodeE EnvBurn::Save() {
    try {
        Json::Value json;
        json["OVDLoginPassword"] = OVDLoginPassword;
        json["OVDMediaEncPassword"] = OVDMediaEncPassword;
        json["cmei"] = cmei;
        json["andlinkID"] = andlinkID;
        json["andlinkKey"] = andlinkKey;
        ErrCodeE e = Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("failed\n");
        return ErrCodeE::Failure;
    }
}
