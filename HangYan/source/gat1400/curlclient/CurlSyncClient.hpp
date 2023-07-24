/**
 * @file CurlSyncClient.hpp
 * @author hening
 * @brief curl同步Post客户端
 * @date 2023-6-1
 */

#ifndef EMX_CURL_SYNC_CLIENT_HPP
#define EMX_CURL_SYNC_CLIENT_HPP

#include "CurlClient.hpp"

namespace Emx {

    //!@brief Curl同步发送客户端
    class CurlSyncClient : public CurlClient {
      public:
        CurlSyncClient();
        ~CurlSyncClient();

        bool Post(const PostData *data) override;
    };
}

#endif //EMX_CURL_SYNC_CLIENT_HPP
