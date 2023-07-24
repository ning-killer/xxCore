/*
* @Author: xiong
* @Date: 2022/11/11
*/

#ifndef EAPIL_EnvManufacturers_HPP
#define EAPIL_EnvManufacturers_HPP

#include "EnvBase.hpp"

namespace Emx {

    class EnvManufacturers : public EnvBase {
    public:
        EnvManufacturers() : EnvBase("manufacturers") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override { return ErrCodeE::Success; }

    public:
        char deviceVendor[128];
        char deviceBrand[32];
        char deviceType[16];
        char os[16];//"Linux-4.9.37"
        char reserve[16];//"eapil"
        char chipFactory[16];//Cvitek
        char chipModel[16];//ARM
        char chipUuid[32];//uuid
        char romSize[16];//16MB
        char ramSize[16];//128MB
        char power[16];//供电类型，电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
    };

}

#endif //EAPIL_EnvManufacturers_HPP
