#include "CurlClient.hpp"

using namespace Emx;
#define TestPrint 0

CurlClient::CurlClient() {
}

CurlClient::~CurlClient() {
}

void CurlClient::FreePostData(PostData *data) {
    if (data == nullptr) {
        return;
    }

    if (data->header != nullptr) {
        curl_slist_free_all(data->header);
        data->header = nullptr;
    }

    // 静态回调函数指针无需进行手动释放掉，其与程序生命周期保持一致。
    // userOperation无需释放由外部管控

    // 其他数据都为系统可自动回收内存的类型
    // if (data != nullptr) {
    //     delete data;
    //     data = nullptr;
    // }

    // data->url.clear();
    // data->url.shrink_to_fit();
    // data->param.clear();
    // data->param.shrink_to_fit();
    // data->response.clear();
    // data->response.shrink_to_fit();
    // data->responseHead.clear();
    // data->responseHead.shrink_to_fit();
    // data->proxy.addr.clear();
    if (data != nullptr) {
        delete data;
        data = nullptr;
    }
}

void CurlClient::Send(PostData *data, bool is_free_header) {
    data->code = CURLE_FAILED_INIT;
    if (data == nullptr) {
        emxloge("send data is null\n");
        return;
    }
    CURL *curl = curl_easy_init();
    if(curl == nullptr) {
        emxloge("curl init failed\n");
        return;
    }
    //配置curl日志支持
    if (data->isdebug && data->cbWriteDebug != nullptr) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, data->cbWriteDebug);
    }
    //配置请求头信息
    if (data->header != nullptr) {
#if TestPrint
        Json::Value json;
        curl_slist* currentHeader = data->header;
        while (currentHeader) {
            std::string header(currentHeader->data);
            size_t colonPos = header.find(':');
            if (colonPos != std::string::npos) {
                std::string field = header.substr(0, colonPos);
                std::string value = header.substr(colonPos + 1);
                json[field] = value;
            }
            currentHeader = currentHeader->next;
        }
        emxlogt("header-data: %s\n", json.toStyledString().c_str());
#endif
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, data->header);
    } else {
        emxlogt("data header is null!\n");
    }

#if TestPrint
    emxlogt("curl-data: %s\n", data->url.c_str());
#endif
    //配置请求地址
    curl_easy_setopt(curl, CURLOPT_URL, data->url.c_str());
    //配置http请求方法
    if (data->way == HttpPostWay::POST) {
        curl_easy_setopt(curl, CURLOPT_POST, 0);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data->param.c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_POST, 0);
    }
    //配置接收应答数据
    if (data->cbWriteData != nullptr) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data->cbWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&(data->response));
    }
    //配置接收应答数据头
    if (data->cbWriteHeader != nullptr) {
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, data->cbWriteHeader);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&(data->responseHead));
    }
    //默认不验证证书和HOST
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    //配置超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, data->timeout);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, data->timeout);
    if (data->proxy.enable) {
        curl_easy_setopt(curl, CURLOPT_PROXY, data->proxy.addr.c_str());
    }
    data->code = curl_easy_perform(curl);
    if (curl != nullptr) {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
    if (is_free_header) {
        if (data->header != nullptr) {
            curl_slist_free_all(data->header);
            data->header = nullptr;
        }
    }
    if (data->code != CURLE_OK) {
        emxloge("curl_easy_perform failed, error code(%d) : %s\n"
                    , data->code, curl_easy_strerror(data->code));
    }
}