#include "CurlSyncClient.hpp"

using namespace Emx;

CurlSyncClient::CurlSyncClient() { 
}

CurlSyncClient::~CurlSyncClient() {
}

bool CurlSyncClient::Post(const PostData *data) {
    Send(const_cast<PostData*>(data));
    return data->code == CURLE_OK ? true : false;
}