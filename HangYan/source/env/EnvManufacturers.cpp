/*
* @Author: xiong
* @Date: 2022/11/11
*/

#include "EnvManufacturers.hpp"

using namespace Emx;

ErrCodeE EnvManufacturers::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get failed\n");
            return e;
        }
        strncpy(deviceVendor, json["deviceVendor"].asCString(), sizeof(deviceVendor));
        strncpy(deviceBrand, json["deviceBrand"].asCString(), sizeof(deviceBrand));
        strncpy(deviceType, json["deviceType"].asCString(), sizeof(deviceType));
        strncpy(os, json["os"].asCString(), sizeof(os));
        strncpy(reserve, json["reserve"].asCString(), sizeof(reserve));
        strncpy(chipFactory, json["chipFactory"].asCString(), sizeof(chipFactory));
        strncpy(chipModel, json["chipModel"].asCString(), sizeof(chipModel));
        strncpy(chipUuid, json["chipUuid"].asCString(), sizeof(chipUuid));
        strncpy(romSize, json["romSize"].asCString(), sizeof(romSize));
        strncpy(ramSize, json["ramSize"].asCString(), sizeof(ramSize));
        strncpy(power, json["power"].asCString(), sizeof(power));
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load param failed\n");
        return ErrCodeE::Failure;
    }
}
