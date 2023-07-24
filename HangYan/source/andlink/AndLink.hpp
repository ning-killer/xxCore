//
// Created by xiong on 2022/6/23.
//

#ifndef EAPIL_AndLink_HPP
#define EAPIL_AndLink_HPP

#include "OvdCtx.hpp"
#include "andlink_pub.h"

namespace Emx {

    class AndLink {
    public:
        AndLink() : m_created(false) {}

        virtual ~AndLink() {}

        void Create(OvdCtx *ctx);

        void Destroy();

        int Reset();

        int SetScanCode(char *userKey, char *gwAddress, char *gwAddress2);

        int SetIp(const char *ip);

    private:
        bool m_created;
    };
}

#endif //EAPIL_AndLink_HPP
