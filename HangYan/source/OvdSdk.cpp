/*
* @Author: xiong
* @Date: 2022/11/9
*/

#include "OvdSdk.hpp"
#include "OvdUtils.hpp"
#include "CallBackWrap.h"

using namespace Emx;

static void ovdLogOutCB(const char *buff) { if (buff) { emxlogn("[SDK]:%s", buff); }}

void OvdSdk::Init(OvdCtx *ctx, OnInitDone onInitDone) {
    if (m_inited)
        return;

    m_onInitDone = std::move(onInitDone);
    m_timer.Create(ctx->loop);

    //分配一块内存空间用于存储在外部线程中初始化SDK所需参数
    auto initSdkArg = new InitSDKArg;

    //cap
    OVDCapInfo &capInfo = initSdkArg->capInfo;
    memcpy(&capInfo, &ctx->env.cap.info, sizeof(capInfo));

    //client
    OVDClientParam &clientParam = initSdkArg->clientParam;
    memcpy(&clientParam, &ctx->env.client.param, sizeof(clientParam));
    //fill dynamic param
    strncpy(clientParam.OVDLoginPassword, ctx->env.burn.OVDLoginPassword.c_str(),
            sizeof(clientParam.OVDLoginPassword));
    strncpy(clientParam.OVDMediaEncPassword, ctx->env.burn.OVDMediaEncPassword.c_str(),
            sizeof(clientParam.OVDMediaEncPassword));
    strncpy(clientParam.OVDDeviceID, ctx->env.sn.Get(), sizeof(clientParam.OVDDeviceID));
    strncpy(clientParam.OVDHardWareModel, ctx->env.devInfo.GetHardware()->version,
            sizeof(clientParam.OVDHardWareModel));
    strncpy(clientParam.OVDSystemVersion, ctx->env.devInfo.GetOuter()->version, sizeof(clientParam.OVDSystemVersion));
    strncpy(clientParam.OVDModelId, ctx->env.modelId.id, sizeof(clientParam.OVDModelId));
    strncpy(clientParam.OVDmacaddress, ctx->env.mac.Get(), sizeof(clientParam.OVDmacaddress));
    clientParam.tz = OvdUtils::TimeZoneToNum(ctx->env.timeZone.Get());

    //log
    OVDLogParam &logParam = initSdkArg->logParam;
#if 1
    logParam.logLevel = (OVDLogLevel) ctx->env.cfg.logLevel;
    logParam.logSTD = OVD_LOGSTD_NO;
    logParam.pOVDLogOutCallBack = ctx->env.cfg.callback ? ovdLogOutCB : nullptr;
#else
    logParam.logLevel = OVD_LOGLEVEL_DEBUG;
    logParam.logSTD = (OVDLogSTD) m_env.cfg.logSTD;
    logParam.pOVDLogOutCallBack = ovdLogOutCB;
#endif
    initSdkArg->logContrl = ctx->env.cfg.logContrl;

    //call back
    OVD_CallBackFunList &callBackFunList = initSdkArg->callBackFunList;
    InitCallBackList(&callBackFunList);

    std::string &jsonParam = initSdkArg->jsonParam;
    Param param("ovd/jsonParam");
    Json::Value json;
    param.Get(json);
    jsonParam = json.toStyledString();

    // OVD_Init 过程中会触发m_cbResp的回调，由于在同一个loop中会造成死锁
    // 所以将其放到线程池中运行
    m_initSDKWork.Create(&ctx->loop, initSdkArg,
                         [](void *arg) {
                             //外部线程中初始化OVD SDK，并记录返回值
                             auto initSdkArg = (InitSDKArg *) arg;
                             initSdkArg->ret = OVD_Init(&initSdkArg->capInfo,
                                                        &initSdkArg->clientParam,
                                                        &initSdkArg->logParam,
                                                        &initSdkArg->callBackFunList,
                                                        (char *) initSdkArg->jsonParam.data());
                         },
                         [this](ErrCodeE e, void *arg) {
                             auto initSdkArg = (InitSDKArg *) arg;
                             if (e != ErrCodeE::Success || initSdkArg->ret != 0) {
                                 //如果初始化失败了，这里的timer用于延迟重新初始化vod sdk
                                 emxlogc("ovd sdk init failed, retrying after 3s\n");
                                 m_timer.Start(3000, 0, [this]() {
                                     m_initSDKWork.Run();
                                 });
                                 return;
                             }
                             //初始化成功，销毁work和分配的参数内存，执行后续工作
                             m_initSDKWork.Destroy();
                             OVD_setlogcontrol(initSdkArg->logContrl);
                             OVD_setloglevel(initSdkArg->logParam.logLevel);
//                             eapilLog.SetLevel(OvdUtils::OvdLevelToEapilLevel(initSdkArg->logParam.logLevel));
                             emxlogn("set ovd log level:%d\n", (int) initSdkArg->logParam.logLevel);
                             //设置SDK与云服务器之间的心跳周期
                        #ifdef OVDSDK_APIVER_1_0
                             OVD_SetKeepaliveIntervel(10);
                        #endif
                             delete (InitSDKArg *) arg;
                             emxlogi("ovd sdk init done\n");
                             m_onInitDone();
                         });
    m_initSDKWork.Run();
    m_inited = true;
}

void OvdSdk::DeInit() {
    if (!m_inited) {
        m_initSDKWork.Destroy();
        m_timer.Destroy();
        m_inited = false;
    }

}
