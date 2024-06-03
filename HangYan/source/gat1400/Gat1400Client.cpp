#include "Gat1400Client.hpp"

using namespace Emx;

Gat1400Client *Gat1400Client::m_instance = nullptr;

Gat1400Client* Gat1400Client::Instance() {
    if (m_instance ==  nullptr) {
        m_instance = new Gat1400Client();
    }
    return m_instance;
}

Gat1400Client::Gat1400Client()
    : m_started(false) 
    , m_gat1400Mgr(new Gat1400Mgr(&m_initParam)) {
    m_loop.Init("Gat1400Client", [this]() {} );
    m_loop.Start();
}

Gat1400Client::~Gat1400Client() {
    m_loop.StopAndDeInit();
    if (m_gat1400Mgr != nullptr) {
        delete m_gat1400Mgr;
        m_gat1400Mgr = nullptr;
    }
}

bool Gat1400Client::Create(OvdCtx *ctx) {
    CHECK_GAT1400_PTR(ctx, false);
    if (!ctx->env.face.ena && !ctx->env.batteryCar.on && !ctx->env.licensePlate.on 
        && !ctx->env.passengerFlow.on && !ctx->env.regionalPeople.on && !ctx->env.offDuty.on) {
        return false;
    }
    Gat1400Util::InitParam param = { 0 };
    memcpy(param.deviceId, ctx->env.sn.Get(), strlen(ctx->env.sn.Get()));
    memcpy(param.password, ctx->env.burn.OVDLoginPassword.c_str(), ctx->env.burn.OVDLoginPassword.size());
    param.mode = ctx->env.face.mode;
    return Start(param);
}

bool Gat1400Client::Start(const Gat1400Util::InitParam &param) {
    CHECK_GAT1400_PTR(m_gat1400Mgr, false);
    if (m_started) {
        return true;
    }
    memset(&m_initParam, 0, sizeof(m_initParam));
    memcpy(&m_initParam, &param, sizeof(param));
    m_gat1400Mgr->Create(&m_loop);
    m_started = true;
    return true;
}

bool Gat1400Client::Stop() {
    CHECK_GAT1400_PTR(m_gat1400Mgr, false);
    if (!m_started) {
        return true;
    }
    m_gat1400Mgr->Stop();
    m_started = false;
    return true;
}

bool Gat1400Client::SetNetWorkStatus(const bool onConnect) {
    Gat1400Mgr::SetNetState(onConnect);
    return true;
}

bool Gat1400Client::Upload(const Gat1400Util::UploadDataParam *upload_param) {
    CHECK_GAT1400_PTR(m_gat1400Mgr, false);
    CHECK_GAT1400_PTR(upload_param, false);
    if (!m_started) {
        emxloge("gat1400 client is stop!\n");
        return false;
    }
    return m_gat1400Mgr->Upload(upload_param);
}

std::string Gat1400Client::GetGetSourceId(const std::string &devceid, const std::string &shottime) {
    // SourceID =DeviceID【20 位】+图像类型"02"【2 位】+抓拍时间 yyyyMMddHHmmss【14 位】+随机数【5位】，共 41 位
    int iRandrom = 0;
    char buf[256] = { 0 };
    iRandrom = Gat1400Util::GetRandom(1, 99999); //5位随机数
    snprintf(buf, sizeof(buf) - 1
            , "%s02%s%05d", devceid.c_str()
            , shottime.c_str(), iRandrom);
    return std::string(buf);
}

std::string Gat1400Client::GetFaceId(const std::string &sourceid) {
    // FaceID = SourceID【41 位】+"06"【2 位】+随机数【5 位】，共 48 位
    int iRandrom = 0;
    char buf[256] = { 0 };
    iRandrom = Gat1400Util::GetRandom(1, 99999); //5位随机数
    snprintf(buf, sizeof(buf) - 1, "%s06%05d", sourceid.c_str(), iRandrom);
    return std::string(buf);
}