#include "EmxPlatform.hpp"
#include "EmxCore.hpp"

using namespace Emx;

EmxPlatform::PlatE EmxPlatform::GetPlat(const std::string platStr)
{
    EmxPlatform::PlatE pe = EmxPlatform::PlatE::EUNKNOWN;

    if (platStr == "HJQV1")
        return EmxPlatform::PlatE::EHJQV1;
    if (platStr == "HJQV2")
        return EmxPlatform::PlatE::EHJQV2;
    if (platStr == "21CN")
        return EmxPlatform::PlatE::E21CN;
    if (platStr == "TY")
        return EmxPlatform::PlatE::ETY;
    if (platStr == "TYKJ")
        return EmxPlatform::PlatE::ETYKJ;
    if (platStr == "CAMTELLS")
        return EmxPlatform::PlatE::ECAMTELLS;
    if (platStr == "YY")
        return EmxPlatform::PlatE::EYY;
    if (platStr == "MJ")
        return EmxPlatform::PlatE::EMJ;
    return pe;
}

