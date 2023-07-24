#include <core/EmxLog.hpp>
#include "MediaChipServer.hpp"

using namespace Emx;

ErrCodeE MediaChipServer::OnRecvReqSub(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) {
    return ErrCodeE::OperationNotSupport;
}
