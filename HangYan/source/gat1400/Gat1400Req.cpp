#include "Gat1400Req.hpp"

using namespace Emx;

size_t Gat1400Req::OnUnRegisterWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnKeepAliveWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnRegisterWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnGetGateWayWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnGetTimeWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnUpLoadFaceWriteData(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

size_t Gat1400Req::OnRegisterWriteHeader(void* buffer
    , size_t size, size_t nmemb, void* user) {
    std::string& response = *((std::string*)user);
    response.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

Gat1400Req::Gat1400Req(Gat1400Util::InitParam *param)
    : m_initParam(param)
    , m_faceAsyncCurlClient(new CurlAsyncClient())
    , m_faceUploadUserOpera(new Gat1400FaceUploadUserOpera()) {
}

Gat1400Req::~Gat1400Req() {
    StopSyncCurlClient();
    if (m_faceAsyncCurlClient != nullptr) {
        delete m_faceAsyncCurlClient;
        m_faceAsyncCurlClient = nullptr;
    }
    Gat1400FaceUploadUserOpera *faceUser = dynamic_cast<Gat1400FaceUploadUserOpera*>(m_faceUploadUserOpera);
    if (faceUser != nullptr) {
        delete faceUser;
        faceUser = nullptr;
    }
}

bool Gat1400Req::StartSyncCurlClient(EuvLoop *loop) {
    if (m_faceAsyncCurlClient == nullptr) {
        return false;
    }
    if (!m_faceAsyncCurlClient->Create(loop)) {
        return false;
    }
    return m_faceAsyncCurlClient->Start();
}

bool Gat1400Req::StopSyncCurlClient() {
    if (m_faceAsyncCurlClient == nullptr) {
        return false;
    }
    return m_faceAsyncCurlClient->Stop();
}

void Gat1400Req::UpdateDeviceId() {
    /**
        本业务流程需要设备上报设备ID，请在对接AndLink及安防SDK后，获取到设备ID（安防平台16位设备序列号。
        如果接入方完全按照1400标准的20位DeviceId实现，请在16位设备序列号前补0）并联系相关接口人添加设备ID信息后，便可开始1400协议对接。
    */
    if (strlen(m_initParam->deviceId) >= 20) {
        return;
    }
    if (strlen(m_initParam->deviceId) < 16) {
        emxloge("device_id size < 16: %s\n", m_initParam->deviceId);
        return;
    }
#if 1 //接口文档写的需要补齐，但实际可不补齐
    int zero_num = 20 - strlen(m_initParam->deviceId);
    std::string s_tmp = "";
    for (int i = 0; i < zero_num; i++) {
        s_tmp.append("0");
    }
    s_tmp.append(std::string(m_initParam->deviceId, strlen(m_initParam->deviceId)));
    memset(m_initParam->deviceId, 0, strlen(m_initParam->deviceId));
    memcpy(m_initParam->deviceId, s_tmp.c_str(), s_tmp.size());
#endif
    emxlogd("gat1400 deviceId: %s\n", m_initParam->deviceId);
    return;
}

Gat1400Util::RegisterRet Gat1400Req::Register(Gat1400Util::RegisterAuthParam &auth_param) {
    auth_param.url = "/VIID/System/Register";
    auth_param.httpMethod = "POST";
    std::string authHeader = PackAuthHeader(auth_param);
    Json::Value dataJson, bodyJson;
    bodyJson["DeviceID"] = std::string(m_initParam->deviceId);
    dataJson["RegisterObject"] = bodyJson;

    CurlClient::PostData data;
    data.header = curl_slist_append(data.header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        data.header = curl_slist_append(data.header, authHeader.c_str());
    }
    data.way = CurlClient::HttpPostWay::POST;
    data.url = m_gateParam.host1400 + "/VIID/System/Register";
    data.timeout = m_gateParam.timeout;
    data.param = dataJson.toStyledString();
    data.cbWriteData = (CurlClient::CbWriteData*)OnRegisterWriteData;
    data.cbWriteHeader = (CurlClient::CbWriteData*)OnRegisterWriteHeader;

    CurlSyncClient curlClient;
    Gat1400Util::RegisterRet ret = Gat1400Util::RegisterRet::RegisterError;
    do {
        if(!curlClient.Post(&data)) {
            emxloge("Register Post Failed!\n");
            break;
        }
        emxlogi("Register Post response data: %s\n", data.response.c_str());
        Json::Value ackJson;
        if (EasyJson::Parse(data.response, ackJson) != ErrCodeE::Success) {
            emxloge("Register ackJson Parse Failed!\n");
            break;
        }
        if ((ackJson["ResponseStatusObject"].isObject()
            && ackJson["ResponseStatusObject"]["StatusCode"].isInt()
            && ackJson["ResponseStatusObject"]["StatusCode"].asInt() == 0)
            || (ackJson["status"].isInt() && ackJson["status"].asInt() == 0)) {
            // 设备已经授权
            ret = Gat1400Util::RegisterRet::Authorized;
            break;
        }
        if ((ackJson["status"].isInt() && ackJson["status"].asInt() == 401)
            && (ackJson["error"].isString() && ackJson["error"].asString() == "Unauthorized")) {
            // 设备未授权状态
            emxlogd("gat1400 Register responseHead: %s\n", data.responseHead.c_str());
            std::map<std::string, std::string> authHeaderMap;
            ParseRegisterRplyHeader(data.responseHead, authHeaderMap);
            std::string authkeyStr = Gat1400Util::GetMapValueByKey("WWW-Authenticate",authHeaderMap);
            if (authkeyStr.size() == 0) {
                emxloge("GetMapValue by WWW-Authenticate is failed!\n");
                break;
            }
            auth_param.nonce =  GetStringValueByKey("nonce=", "\"", authkeyStr);
            auth_param.qop =  GetStringValueByKey("qop=", "\"", authkeyStr);
            auth_param.realm =  GetStringValueByKey("realm=", "\"", authkeyStr);
            m_regauthParam = auth_param;
            emxlogd("m_regauthParam: [nonce=%s | qop=%s | realm=%s]\n"
                    , m_regauthParam.nonce.c_str(), m_regauthParam.qop.c_str()
                    , m_regauthParam.realm.c_str());
            ret = Gat1400Util::RegisterRet::UnAuthorized;;
            break;
        }
        // 其他失败场景： Ret = Gat1400Util::RegisterRet::RegisterError;
    } while(false);
    curlClient.FreePostData(&data);
    return ret;
}

bool Gat1400Req::UnRegister() {
    Gat1400Util::RegisterAuthParam authParam = m_regauthParam;
    authParam.url = "/VIID/System/UnRegister";
    authParam.httpMethod = "POST";
    std::string authHeader = PackAuthHeader(authParam);
    Json::Value dataJson, bodyJson;
    bodyJson["DeviceID"] = std::string(m_initParam->deviceId);
    dataJson["UnRegisterObject"] = bodyJson;

    CurlClient::PostData data;
    data.header = curl_slist_append(data.header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        data.header = curl_slist_append(data.header, authHeader.c_str());
    }
    data.way = CurlClient::HttpPostWay::POST;
    data.url = m_gateParam.host1400 + "/VIID/System/UnRegister";
    data.timeout = m_gateParam.timeout;
    data.param = dataJson.toStyledString();
    data.cbWriteData = (CurlClient::CbWriteData*)OnUnRegisterWriteData;

    CurlSyncClient curlClient;
    bool ret = false;
    do {
        if(!curlClient.Post(&data)) {
            emxloge("UnRegister Post Failed!\n");
            break;
        }
        emxlogi("UnRegister Post response data: %s\n", data.response.c_str());
        Json::Value ackJson;
        if (EasyJson::Parse(data.response, ackJson) != ErrCodeE::Success) {
            emxloge("UnRegister ackJson Parse Failed!\n");
            break;
        }
        if (!ackJson["ResponseStatusObject"].isObject()
            && ackJson["ResponseStatusObject"]["StatusCode"].isInt()
            && ackJson["ResponseStatusObject"]["StatusCode"].asInt() == 0) {
            ret = true;
            break;
        }
    } while(false);
    curlClient.FreePostData(&data);
    return ret;
}

int Gat1400Req::KeepAlive() {
    Gat1400Util::RegisterAuthParam authParam = m_regauthParam;
    authParam.url = "/VIID/System/Keepalive";
    authParam.httpMethod = "POST";
    std::string authHeader = PackAuthHeader(authParam);
    Json::Value dataJson, bodyJson;
    bodyJson["DeviceID"] = std::string(m_initParam->deviceId);
    dataJson["KeepaliveObject"] = bodyJson;

    CurlClient::PostData data;
    data.header = curl_slist_append(data.header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        data.header = curl_slist_append(data.header, authHeader.c_str());
    }
    data.way = CurlClient::HttpPostWay::POST;
    data.url = m_gateParam.host1400 + "/VIID/System/Keepalive";
    data.timeout = m_gateParam.timeout;
    data.param = dataJson.toStyledString();
    data.cbWriteData = (CurlClient::CbWriteData*)OnKeepAliveWriteData;

    CurlSyncClient curlClient;
    bool ret = 0;
    do {
        if(!curlClient.Post(&data)) {
            emxloge("KeepAlive Post Failed!\n");
            break;
        }
        emxlogi("KeepAlive Post response data: %s\n", data.response.c_str());
        Json::Value ackJson;
        if (EasyJson::Parse(data.response, ackJson) != ErrCodeE::Success) {
            emxloge("KeepAlive ackJson Parse Failed!\n");
            break;
        }
        if (ackJson["ResponseStatusObject"].isObject()
            && ackJson["ResponseStatusObject"]["StatusCode"].isInt()) {
            ret = ackJson["ResponseStatusObject"]["StatusCode"].asInt();
            break;
        }
        if (ackJson["status"].isInt()) {
            ret = ackJson["status"].asInt();
            break;
        }
    } while(false);
    curlClient.FreePostData(&data);
    return ret;
}

bool Gat1400Req::GetGateWay(Gat1400Util::GateParam &gate_param) {
    if (strlen(m_initParam->deviceId) < 20) {
        emxloge("device_id is error: %s\n", m_initParam->deviceId);
        return false;
    }
    std::string id = std::string(m_initParam->deviceId);
    std::string sign;
	std::string nonce = Gat1400Util::GetNonceStr(16);
    if (!Gat1400Util::GetSign(nonce, id, m_initParam->password, sign)) {
        emxloge("GetSign: %s\n", sign.c_str());
        return false;
    }
	char urlBuf[256] = { 0 };
	std::string url = CMCC_1400_GATEWAY_URL;
	sprintf(urlBuf, "?deviceId=%s&nonce=%s&sign=%s", id.c_str(), nonce.c_str(), sign.c_str());
	url.append("/config/clientConfig/get");
	url.append(urlBuf);
    emxlogd("get platform config, url:%s\n", url.c_str());

    CurlClient::PostData data;
    data.way = CurlClient::HttpPostWay::GET;
    data.url = url;
    data.cbWriteData = (CurlClient::CbWriteData*)OnGetGateWayWriteData;

    CurlSyncClient curlClient;
    bool ret = false;
    do {
        if(!curlClient.Post(&data)) {
            emxloge("GetGateWay Post Failed!\n");
            break;
        }
        emxlogi("GetGateWay Post response data: %s\n", data.response.c_str());
        Json::Value ackJson;
        if (EasyJson::Parse(data.response, ackJson) != ErrCodeE::Success) {
            emxloge("GetGateWay ackJson Parse Failed!\n");
            break;
        }
        if (!ackJson["resultCode"].isInt()) {
            break;
        }
        if (ackJson["resultCode"].asInt() != 200) {
            break;
        }
        if (!ackJson["resultData"].isObject()) {
            break;
        }
        Json::Value dataJson;
        dataJson = ackJson["resultData"];
        if (dataJson["host1400"].isString()) {
            ret = true;
            gate_param.host1400 = dataJson["host1400"].asString();
        }
        if (dataJson["getConfigInterval"].isInt()) {
            gate_param.cfgInterval = dataJson["getConfigInterval"].asInt();
        }
        if (dataJson["retry"].isInt()) {
            gate_param.retry = dataJson["retry"].asInt();
        }
        if (dataJson["timeout"].isInt()) {
            gate_param.timeout = dataJson["timeout"].asInt() * 1000;
        }
        if (dataJson["keepAliveInterval"].isInt()) {
            gate_param.keepAliveInterval = dataJson["keepAliveInterval"].asInt();
        }
        gate_param.postSceneImg = false;
        //note: 拓展解析是否全景图上传开关
        if (!ackJson["extendInfoList"].isArray()) {
            m_gateParam = gate_param;
            break;
        }
        for (auto object: ackJson["extendInfoList"]) {
            if (object["infoCode"].isString() 
                && object["infoCode"].asString() == "scene"
                && object["faceScene"].isInt()) {
                gate_param.postSceneImg = object["faceScene"].asInt() == 0 ? false : true;
            }
        }
        m_gateParam = gate_param;
    } while(false);
    curlClient.FreePostData(&data);
    return ret;
}

bool Gat1400Req::GetTime() {
    Gat1400Util::RegisterAuthParam authParam = m_regauthParam;
    authParam.url = "/VIID/System/Time";
    authParam.httpMethod = "GET";
    std::string authHeader = PackAuthHeader(authParam);

    CurlClient::PostData data;
    data.header = curl_slist_append(data.header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        data.header = curl_slist_append(data.header, authHeader.c_str());
    }
    data.way = CurlClient::HttpPostWay::GET;
    data.url = m_gateParam.host1400 + "/VIID/System/Time";
    data.timeout = m_gateParam.timeout;
    data.cbWriteData = (CurlClient::CbWriteData*)OnGetTimeWriteData;

    CurlSyncClient curlClient;
    bool ret = false;
    do {
        if(!curlClient.Post(&data)) {
            emxloge("GetTime Post Failed!\n");
            break;
        }
        emxlogi("GetTime Post response data: %s\n", data.response.c_str());
        Json::Value ackJson;
        if (EasyJson::Parse(data.response, ackJson) != ErrCodeE::Success) {
            emxloge("GetTime ackJson Parse Failed!\n");
            break;
        }
        if (!ackJson["SystemTimeObject"].isObject()) {
            break;
        }
        ret = true;
        if (ackJson["SystemTimeObject"]["VIIDServerID"].isString()) {
            m_sysTimeParam.VIIDServerID = ackJson["SystemTimeObject"]["VIIDServerID"].asString();
        }
        if (ackJson["SystemTimeObject"]["TimeMode"].isString()) {
            m_sysTimeParam.TimeMode = ackJson["SystemTimeObject"]["TimeMode"].asString();
        }
        if (ackJson["SystemTimeObject"]["LocalTime"].isString()) {
            m_sysTimeParam.LocalTime = ackJson["SystemTimeObject"]["LocalTime"].asString();
        }
        if (ackJson["SystemTimeObject"]["TimeZone"].isString()) {
            m_sysTimeParam.TimeZone = ackJson["SystemTimeObject"]["TimeZone"].asString();
        }
    } while(false);
    curlClient.FreePostData(&data);
    return ret;
}

bool Gat1400Req::UpLoadFace(const Gat1400Util::UploadDataParam &upload_param) {
    if (m_faceAsyncCurlClient == nullptr) {
        emxloge("CurlAsyncClient is no Create!\n");
        return false;
    }
    Gat1400Util::RegisterAuthParam authParam = m_regauthParam;
    authParam.url = "/VIID/Faces";
    authParam.httpMethod = "POST";
    std::string authHeader = PackAuthHeader(authParam);
    std::string s_param;
    if (!PackUploadFacesParam(upload_param, s_param)) {
        return false;
    }
    CurlClient::PostData data;
    data.header = curl_slist_append(data.header, "Content-Type: application/viid+json");
    if (authHeader.size() != 0) {
        data.header = curl_slist_append(data.header, authHeader.c_str());
    }
    data.way = CurlClient::HttpPostWay::POST;
    data.url = m_gateParam.host1400 + "/VIID/Faces";
    data.timeout = m_gateParam.timeout;
    data.param = s_param;
    data.cbWriteData = (CurlClient::CbWriteData*)OnUpLoadFaceWriteData;
    data.userOperation = (UserOperation*)m_faceUploadUserOpera;
    m_faceAsyncCurlClient->Post(&data);
    m_faceAsyncCurlClient->FreePostData(&data);
    return true;
}

bool Gat1400Req::PackUploadFacesParam(const Gat1400Util::UploadDataParam &upload_param, std::string &param) {
    bool ret = false, isHaveFullImg = false;
    Json::Value SubImageInfoObjectArray, SubImageInfoObject, SubImageInfoObjectArrayFull;
    for(auto sub : upload_param.subImgList) {
        ret = true;
        SubImageInfoObjectArray["ImageID"] = sub.ImageID;
        SubImageInfoObjectArray["EventSort"] = sub.EventSort;
        SubImageInfoObjectArray["DeviceID"] = upload_param.deviceId;
        SubImageInfoObjectArray["StoragePath"] = sub.StoragePath;
        SubImageInfoObjectArray["Type"] = sub.Type;
        SubImageInfoObjectArray["FileFormat"] = sub.FileFormat;
        SubImageInfoObjectArray["ShotTime"] = sub.ShotTime;
        SubImageInfoObjectArray["Width"] = sub.Width;
        SubImageInfoObjectArray["Height"] = sub.Height;
        SubImageInfoObjectArray["Data"] = sub.Data;
        if (!sub.isfullImg) {
            SubImageInfoObject["SubImageInfoObject"].append(SubImageInfoObjectArray);
        } else if (sub.isfullImg && isHaveFullImg && m_gateParam.postSceneImg) {
            isHaveFullImg = true;
            SubImageInfoObjectArrayFull = SubImageInfoObjectArray;
        }
    }
    if (isHaveFullImg && m_gateParam.postSceneImg) {
        // 大图数据追加到列表最后一个
        SubImageInfoObject["SubImageInfoObject"].append(SubImageInfoObjectArrayFull);
    }
    Json::Value FaceObjectArray, FaceObject;
    FaceObjectArray["FaceID"] = upload_param.Id;
    FaceObjectArray["InfoKind"] = 1;
    FaceObjectArray["SourceID"] = upload_param.SourceID;
    FaceObjectArray["DeviceID"] = upload_param.deviceId;
    FaceObjectArray["LeftTopX"] = upload_param.LeftTopX;
    FaceObjectArray["LeftTopY"] = upload_param.LeftTopY;
    FaceObjectArray["RightBtmX"] = upload_param.RightBtmX;
    FaceObjectArray["RightBtmY"] = upload_param.RightBtmY;
    FaceObjectArray["LocationMarkTime"] = upload_param.LocationMarkTime;
    FaceObjectArray["FaceAppearTime"] = upload_param.AppearTime;
    FaceObjectArray["FaceDisAppearTime"] = upload_param.DisAppearTime;
    FaceObjectArray["IsSuspectedTerrorist"] = 2;
    FaceObjectArray["IsCriminalInvolved"] = 2;
    FaceObjectArray["IsDetainees"] = 2;
    FaceObjectArray["IsVictim"] = 2;
    FaceObjectArray["IsSuspiciousPerson"] = 2;
    FaceObjectArray["SubImageList"] = SubImageInfoObject;
    FaceObject["FaceObject"][0] = FaceObjectArray;
    Json::Value paramJson;
    paramJson["FaceListObject"] = FaceObject;
    param = paramJson.toStyledString();
    return ret;
}

int Gat1400Req::m_index = 0;

std::string Gat1400Req::PackAuthHeader(const Gat1400Util::RegisterAuthParam &param) {
    std::string authheaderStr = "";
    do {
        if (param.qop != "auth") {
            emxlogd("unsupport method : %s\n", param.qop.c_str());
            break;
        }
        std::string cnonce = Gat1400Util::GetNonceStr(8);
        std::string ha1 = "", ha2 = "", response = "";
        char buffer[1024] = { 0 };
        char nonceCount[16] = { 0 };
        snprintf(nonceCount, sizeof(nonceCount) - 1, "%08d", ++m_index);

        // ha1=MD5(username:realm:password)
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s:%s"
                , m_initParam->deviceId, param.realm.c_str()
                , m_initParam->password);
        Gat1400Util::GetMd5(buffer, ha1);

        // ha2=MD5(method:digestURI)
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s"
                , param.httpMethod.c_str(), param.url.c_str());
        Gat1400Util::GetMd5(buffer, ha2);

        // response=MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "%s:%s:%s:%s:%s:%s"
                ,  ha1.c_str(), param.nonce.c_str(), nonceCount
                ,  cnonce.c_str(), param.qop.c_str(), ha2.c_str());
        Gat1400Util::GetMd5(buffer, response);

        // pack
        memset(buffer, 0 ,sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "Authorization: Digest username=\"%s\",realm=\"%s\",nonce=\"%s\",uri=\"%s\",qop=%s,nc=%s,cnonce=\"%s\",response=\"%s\""
                , m_initParam->deviceId, param.realm.c_str(), param.nonce.c_str()
                , param.url.c_str(), param.qop.c_str(), nonceCount, cnonce.c_str(), response.c_str());
        authheaderStr = std::string(buffer);
    } while (false);
    return authheaderStr;
}

bool Gat1400Req::ParseRegisterRplyHeader(const std::string &result
    , std::map<std::string, std::string> &dst) {
    if (result.size() == 0) {
        return false;
    }
    int index = 0, last_index = 0;
    std::string tmp, key, val;
    std::string findKey = "\r\n";
    std::map<std::string, std::string> hdsMap;
    do {
        if (index == 0) {
            last_index = index;
        } else {
            last_index = index + findKey.size();
        }
        index = result.find(findKey, last_index);
        if (index > 0) {
            tmp = result.substr(last_index, index - last_index);
            //根据":"拆分字符
            int subIndex = 0;
            subIndex = tmp.find(":", 0);
            if (subIndex > 0) {
                key = tmp.substr(0, subIndex);
                val = tmp.substr(subIndex+1);
                dst[Trim(key)] = Trim(val);
            }
        }
    } while (index > 0);
    if (dst.size() > 0) {
        return true;
    }
    return false;
}

std::string &Gat1400Req::TrimLeft(std::string &str) {
    str.erase(0, str.find_first_not_of(" "));
    return str;
}

std::string &Gat1400Req::TrimRight(std::string &str) {
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

std::string &Gat1400Req::Trim(std::string &str) {
    return TrimLeft(TrimRight(str));
}

std::string Gat1400Req::GetStringValueByKey(const std::string &key, 
    const std::string &start_end, const std::string &src) {
    std::string retStr  = "";
    std::string tmpSrc = src;
    int index = 0, index2 = 0, index3 = 0;
    index = tmpSrc.find(key);
    if (index > 0) {
        index2 = tmpSrc.find(start_end, index + key.size());
        if (index2 > 0) {
            index3 = tmpSrc.find(start_end, index2 + 1);
            retStr = tmpSrc.substr(index2 + 1, index3 - index2 - 1);
        }
    }
    return retStr;
}