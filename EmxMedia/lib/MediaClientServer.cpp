//
// Created by xiong on 2021/8/5.
//
#include "core/EmxLog.hpp"
#include "core/EmxMsg.hpp"
#include "MediaInner.hpp"
#include "media/MediaClientServer.hpp"

using namespace Emx;

ErrCodeE MediaClientServer::CreateMedia() {
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID, (uint32_t) MediaModE::Server,
                                        (uint32_t) MediaServerMethodE::Create);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientServer::DestroyMedia() {
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID, (uint32_t) MediaModE::Server,
                                        (uint32_t) MediaServerMethodE::Destroy);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
