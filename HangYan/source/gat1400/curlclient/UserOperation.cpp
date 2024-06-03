#include "UserOperation.hpp"

using namespace Emx;

std::string UserOperation::PackAuthHeader() {
    std::string authheaderStr = "";
    do {
        if (m_initParam == nullptr || m_authParam == nullptr) {
            emxloge("param is nulll!\n");
            break;
        }
        if (m_authParam->qop != "auth") {
            emxlogd("unsupport method : %s\n", m_authParam->qop.c_str());
            break;
        }
        std::string cnonce = Gat1400Util::GetNonceStr(8);
        std::string ha1 = "", ha2 = "", response = "";
        char buffer[1024] = { 0 };
        char nonceCount[16] = { 0 };
        snprintf(nonceCount, sizeof(nonceCount) - 1, "%08d", ++Gat1400Util::m_index);

        // ha1=MD5(username:realm:password)
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s:%s"
                , m_initParam->deviceId, m_authParam->realm.c_str()
                , m_initParam->password);
        Gat1400Util::GetMd5(buffer, ha1);

        // ha2=MD5(method:digestURI)
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s"
                , m_authParam->httpMethod.c_str(), m_authParam->url.c_str());
        Gat1400Util::GetMd5(buffer, ha2);

        // response=MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s:%s:%s:%s:%s"
                ,  ha1.c_str(), m_authParam->nonce.c_str(), nonceCount
                ,  cnonce.c_str(), m_authParam->qop.c_str(), ha2.c_str());
        Gat1400Util::GetMd5(buffer, response);

        // pack
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "Authorization: Digest username=\"%s\",realm=\"%s\",nonce=\"%s\",uri=\"%s\",qop=%s,nc=%s,cnonce=\"%s\",response=\"%s\""
                , m_initParam->deviceId, m_authParam->realm.c_str(), m_authParam->nonce.c_str()
                , m_authParam->url.c_str(), m_authParam->qop.c_str(), nonceCount, cnonce.c_str(), response.c_str());
        authheaderStr = std::string(buffer);
    } while (false);
    return authheaderStr;
}

bool UserOperation::UpdateAuthHeader(struct curl_slist *header) {
    if (m_authParam == nullptr || m_initParam == nullptr) {
        return false;
    }
    if (header != nullptr) {
        curl_slist_free_all(header);
        header = nullptr;
    }
    std::string authHeader = PackAuthHeader();
    header = curl_slist_append(header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        header = curl_slist_append(header, authHeader.c_str());
    }
    return true;
}

bool Gat1400FaceUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("UploadFace ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

bool Gat1400MotorVehiclesUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("Gat1400MotorVehiclesUploadUserOpera ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

bool Gat1400NonMotorVehiclesUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("Gat1400NonMotorVehiclesUploadUserOpera ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

bool Gat1400TrafficUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("Gat1400TrafficUploadUserOpera ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

bool Gat1400RegionUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        // todo： 临时处理
        if (result.size() == 0) {
            return true;
        }
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("Gat1400RegionUploadUserOpera ackJson Parse Failed: %s\n", result.c_str());
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}

bool Gat1400OnLeaveUploadUserOpera::isUploadOk(const std::string &result) {
    bool ret = false;
    do {
        Json::Value ackJson;
        // todo： 临时处理
        if (result.size() == 0) {
            return true;
        }
        if (EasyJson::Parse(result, ackJson) != ErrCodeE::Success) {
            emxloge("Gat1400OnLeaveUploadUserOpera ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusListObject"].isObject()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"].isArray()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].isInt()
            && ackJson["ResponseStatusListObject"]["ResponseStatusObject"][0]["StatusCode"].asInt() == 0) {
            ret = true;
        }
    } while(false);
    return ret;
}
