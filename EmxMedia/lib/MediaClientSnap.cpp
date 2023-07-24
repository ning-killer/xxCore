//
// Created by xiong on 2021/5/28.
//

#include "core/EmxLog.hpp"
#include "core/EmxMsg.hpp"
#include "media/MediaClientSnap.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientSnap::Trigger(int chn, int32_t timeoutMs) {
    std::string rep;
    char data[sizeof(chn)];
    memcpy(data, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Snap,
                                        (uint32_t) MediaSnapMethodE::Trigger,
                                        data, sizeof(data), &(rep), timeoutMs);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
