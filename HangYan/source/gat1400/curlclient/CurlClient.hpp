/**
 * @file CurlClient.hpp
 * @author hening
 * @brief curl客户端
 * @date 2023-6-1
 */

#ifndef EMX_CURLCLIENT_HPP
#define EMX_CURLCLIENT_HPP

#include "EmxCore.hpp"
#include "curl.h"
#include "UserOperation.hpp"
#include <mutex>

namespace Emx {

    class CurlClient {
      public:

        typedef void (*CbWriteData) (void* buffer, size_t size, size_t nmemb, void* user);
        typedef void (*CbWriteHeader) (void* buffer, size_t size, size_t nmemb, void* user);
        typedef void (*CbWriteDebug) (CURL *p, curl_infotype itype, char *data, size_t size, void *user);

        //!@brief Http请求方法
        enum HttpPostWay : uint8_t {
            GET = 0, //!< get方式请求
            POST,    //!< post方式请求
        };

        //!@brief Curl代理配置
        //!@note 存在string数据，需要避免memset()操作
        struct CurlProxy {
            bool enable = false;    //!< 代理开关
            std::string addr = "";  //!< 代理addr
        };
        
        //!@brief Post数据信息结构
        //!@note 存在string数据，需要避免memset()操作
        struct PostData {
            std::string url = "";                   //!< 请求url
            std::string param = "";                 //!< 请求参数
            std::string response = "";              //!< 应答数据
            std::string responseHead = "";          //!< 应答header数据
            bool isdebug = false;                   //!< curl日志开关
            HttpPostWay way = HttpPostWay::POST;    //!< 请求方式
            int timeout = 3000;                     //!< 超时时间
            CURLcode code = CURLE_OK;               //!< 请求结果错误码
            CurlProxy proxy;                        //!< 代理信息
            struct curl_slist *header = nullptr;    //!< 请求头信息
            CbWriteData *cbWriteData = nullptr;     //!< 写应答数据回调
            CbWriteHeader *cbWriteHeader = nullptr; //!< 写应答数据头回调
            CbWriteDebug *cbWriteDebug = nullptr;   //!< 写curl日志的回调
            UserOperation *userOperation = nullptr; //!< 用户操作类指针
        };

      public:
        CurlClient();
        virtual ~CurlClient();
      
      public:
        /**
         * @brief 数据投递上传
         * @param data [in] PostData结构化数据
         * @return true 成功; false 失败
         * @note  调用者可使用FreePostData接口进行释放手动资源
         */
        virtual bool Post(const PostData *data) = 0;

        /**
         * @brief 释放PostData资源
         * @param data [in] PostData结构化数据
         */
        void FreePostData(PostData *data);

      protected:
        void Send(PostData *data, bool is_free_header = true);
    };
}

#endif //EMX_CURLCLIENT_HPP
