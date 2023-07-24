//
// Created by xiong on 2022/7/8.
//

#include "Zone.hpp"

using namespace Emx;

ErrCodeE Zone::Parse(const Json::Value &json) {
    if (json.size() != Width * Height / 8)
        return ErrCodeE::Failure;
    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width / 8; j++) {
            auto data = (uint8_t) json[i * Width / 8 + j].asInt();
            for (int k = 7; k >= 0; k--)
                map[i * Width + j * 8 + 7 - k] = (bool) ((data >> k) & 0x01);
        }
    return ErrCodeE::Success;
}

ErrCodeE Zone::Serialize(Json::Value &json) {
    json.clear();
    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width / 8; j++) {
            uint8_t data = 0;
            for (int k = 7; k >= 0; k--)
                data |= ((uint8_t) map[i * Width + j * 8 + 7 - k]) << k;
            json.append(data);
        }
    return ErrCodeE::Success;
}
