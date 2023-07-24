#include "cvi_ae.h"
#include "isp/cv183x/cvi_awb.h"
#include "sample_comm.h"
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "MediaChipISP.hpp"
#include "MediaInner.hpp"


using namespace Emx;

ErrCodeE MediaChipISP::Start() {
    if (m_started)
        return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    Json::Value root;
    m_res.configs.GetCfg("isp", root);
    auto &json = root[m_chn];
    m_pipe = json["pipe"].asInt();
    m_infoInvMs = json["infoInvMs"].asInt();
    m_wdrMode = (WDR_MODE_E) json["wdrMode"].asInt();
    m_timer.Create(m_res.loop);
    SetRunMode(RunModeE::Day);
    StartCollection();
    m_started = true;
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    StopCollection();
    m_timer.Destroy();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetParam(const MediaISP::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    StopCollection();
    memcpy(&m_param, &param, sizeof(MediaISP::Param));
    SaveParam();
    StartCollection();
    IspUpdate();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::GetParam(MediaISP::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaISP::Param));
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetRunMode(RunModeE mode) {
    if (m_mode == mode)
        return ErrCodeE::Success;

    Json::Value root;
    m_res.configs.GetCfg("isp", root);
    auto &json = root[m_chn];
    const char *name;
    int fps;

    switch (mode) {
        case RunModeE::Day:
            name = "day";
            fps = json["fps"]["day"].asInt();
            break;
        case RunModeE::Night:
            name = "night";
            fps = json["fps"]["night"].asInt();
            break;
        case RunModeE::NightWithLight:
            name = "nightWithLight";
            fps = json["fps"]["night"].asInt();
            break;
        default:
            emxloge("unknown mode %d\n", (int) mode);
            return ErrCodeE::IllegalParam;
    }

    CVI_S32 s32Ret = CVI_SUCCESS;
    CVI_CHAR binName[BIN_FILE_LENGTH];
    char path[EMX_MAX_PATH_SIZE];

    snprintf(path, sizeof(path), "%s/isp/%s", m_res.configs.GetDir(), json["pq"][name].asCString());
    CVI_BIN_SetBinName(m_wdrMode, path);
    s32Ret = CVI_BIN_GetBinName(binName);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("GetBinName(%s) fail\n", binName);
        return ErrCodeE::Failure;
    }

    std::string buffer;
    if (File::Read(path, buffer) != ErrCodeE::Success) {
        emxloge("read %s failed\n", path);
        return ErrCodeE::Failure;
    }
    s32Ret = CVI_BIN_ImportBinData((uint8_t *) buffer.data(), (uint32_t) buffer.size());
    if (s32Ret != CVI_SUCCESS) {
        emxloge("CVI_BIN_ImportBinData failed with %#x!\n", s32Ret);
    }

    ISP_PUB_ATTR_S pubAttr = {};
    if (CVI_ISP_GetPubAttr(m_pipe, &pubAttr) == CVI_SUCCESS) {
        if (pubAttr.f32FrameRate != (float) fps) {
            pubAttr.f32FrameRate = (float) fps;
            if (CVI_ISP_SetPubAttr(m_pipe, &pubAttr) == CVI_SUCCESS) {
                SetAntiFlicker(m_param.antiFlickerHz, m_param.forceAntiFlickerEna);
                Json::Value jsonFps;
                jsonFps["chn"] = m_pipe;
                jsonFps["fps"] = fps;
                std::string data;
                EasyJson::Serialize(json, data);
                m_res.pub.Publish(MEDIA_TOPIC_ISP_FPS_CHANGE, data.data(), (int) data.size() + 1);
                //
            } else {
                emxloge("CVI_ISP_SetPubAttr %d failed with %#x!\n", m_pipe, s32Ret);
            }
        }
    }else {
        emxloge("CVI_ISP_GetPubAttr %d failed with %#x!\n", m_pipe, s32Ret);
    }

    m_mode = mode;
    return ErrCodeE::Success;
}

void MediaChipISP::StartCollection() {
    m_timer.Start(3000, m_infoInvMs, [this]() {
        Collection();
    });
}

void MediaChipISP::StopCollection() {
    m_timer.Stop();
}

void MediaChipISP::Collection()
{
    ISP_EXP_INFO_S stExpInfo = {};/*获取AE内部状态信息，包括256段直方图和平均亮度等统计信息。同时还可以获取AE运行状态中的曝光时间，增益，曝光量和实际生效的AE route等信息*/
    if (CVI_ISP_QueryExposureInfo((VI_PIPE) m_pipe, &stExpInfo) != CVI_SUCCESS)
        return;

    CVI_U16 redGain = 0;
    CVI_U16 blueGain = 0;
    if (m_mode == RunModeE::Night) {
        //if (CVI_ISP_GetGrayWorldAwbInfo((VI_PIPE) m_pipe, &redGain, &blueGain) != CVI_SUCCESS)
          //  return;
    }
    else{
        ISP_WB_INFO_S stWBInfo = {};//定义白平衡，饱和度，颜色校正
        if (CVI_ISP_QueryWBInfo((VI_PIPE) m_pipe, &stWBInfo) != CVI_SUCCESS)//获取当前白平衡增益系数，检测色温，饱和度值，颜色校正矩阵系数
            return;
        redGain = stWBInfo.u16Rgain;//R通道增益值
        blueGain = stWBInfo.u16Bgain;//B通道增益值
    }

    Info info = {};
    info.chn = m_chn;
    info.iso = (int) stExpInfo.u32ISO;/* 当前sensor模拟增益*sensor数字增益*ISP数字增益 *100*/
    info.redGain = redGain;
    info.blueGain = blueGain;
    m_res.pub.Publish(MEDIA_TOPIC_ISP_INFO, (char *) &info, (int) sizeof(info));
}

void MediaChipISP::IspUpdate()
{
    SetExposure(m_param.expMode,m_param.autoMaxExpGain,
                m_param.manualExpGain,m_param.compensation,m_param.expTime);
    SetWb(m_param.wbMode,m_param.redGain,m_param.blueGain);
    SetSharpness(m_param.sharpness);
    SetAmp(PROC_AMP_BRIGHTNESS,m_param.brightness);//亮度
    SetAmp(PROC_AMP_CONTRAST,m_param.contrast);//对比度
    SetAmp(PROC_AMP_SATURATION,m_param.hue);//色度
    SetAmp(PROC_AMP_HUE,m_param.hue);//饱和度
    SetDeNoise(m_param.deNoise);//降噪程度
    SetAntiFlicker(m_param.antiFlickerHz,m_param.forceAntiFlickerEna);//抗闪烁
}

/**
 * @brief 设置色彩参数
 * 亮度,色度,对比度，饱和度
 * @param chn vpss通道
 * @param isp 色彩参数
 * @return int 0=成功 其他=失败
 */
ErrCodeE MediaChipISP::SetAmp(PROC_AMP_E AmpMode,uint8_t value)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    int tmp;
    s32Ret = CVI_VPSS_GetGrpProcAmp(m_vpss, AmpMode, &tmp);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("vpss %d CVI_VPSS_GetGrpProcAmp err,s32Ret=%#x\n",m_vpss);
        return ErrCodeE::Failure;
    }
    emxlogi("AmpMode:%d,value:%d\n",(int)AmpMode,tmp);

    tmp  = value*100/255;
    if (100 < tmp) {
        tmp = 100;
    } else if (0 > tmp) {
        tmp = 0;
    }

    s32Ret = CVI_VPSS_SetGrpProcAmp(m_vpss, PROC_AMP_E::PROC_AMP_BRIGHTNESS, tmp);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("vpss %d CVI_VPSS_SetGrpProcAmp err,s32Ret=%#x\n",m_vpss);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetExposure(OptModeE expMode, ExpGainE autoGain, ExpGainE manualGain,
                     uint8_t compensation, int32_t expTime)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_EXPOSURE_ATTR_S stExposureAttr;
    s32Ret = CVI_ISP_GetExposureAttr(m_pipe, &stExposureAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    stExposureAttr.enOpType = (ISP_OP_TYPE_E)expMode;
    if (stExposureAttr.enOpType){
        //stExposureAttr.stManual.u32ExpTime =

        stExposureAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
        stExposureAttr.stManual.bUseISONum = 0;
        stExposureAttr.stManual.enISONumOpType = OP_TYPE_MANUAL;
        stExposureAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
        stExposureAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
        stExposureAttr.stManual.u32ExpTime = expTime;
        stExposureAttr.stManual.u32AGain = (CVI_U32)manualGain * 1024;
        stExposureAttr.stManual.u32ISPDGain = compensation*4096/255 + 4096;
    }else{
        stExposureAttr.stAuto.bUseISONum = 1;
        stExposureAttr.stAuto.stISONumRange.u32Max = (int)autoGain * 800;
        stExposureAttr.stAuto.u16EVBias = compensation*2048/255;
    }

    s32Ret = CVI_ISP_SetExposureAttr(m_pipe, &stExposureAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_SetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
/**
 * @brief 设置锐度参数
 * @param sharpness 锐度参数
 */
ErrCodeE MediaChipISP::SetSharpness(uint8_t sharpness)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_SHARPEN_ATTR_S stSharpnessAttr;

    s32Ret = CVI_ISP_GetSharpenAttr(m_pipe,&stSharpnessAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetSharpenAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    stSharpnessAttr.Enable = CVI_TRUE;
    stSharpnessAttr.enOpType = OP_TYPE_AUTO;
    if(sharpness != 128){
        stSharpnessAttr.enOpType = OP_TYPE_MANUAL;
        stSharpnessAttr.stManual.GlobalGain = sharpness;
    }

    s32Ret = CVI_ISP_SetSharpenAttr(m_pipe,&stSharpnessAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_SetSharpenAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

/**
 * @brief 设置降噪参数
 * @param sharpness 降噪参数
 */
ErrCodeE MediaChipISP::SetDeNoise(uint8_t deNoise)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_TNR_ATTR_S nioseTnrAttr;

    s32Ret = CVI_ISP_GetTNRAttr(m_pipe,&nioseTnrAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetTNRAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    nioseTnrAttr.enOpType = OP_TYPE_AUTO;
    if(deNoise != 128) {
        nioseTnrAttr.enOpType = OP_TYPE_MANUAL;
        for (int i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
            nioseTnrAttr.stManual.TnrStrength0 = deNoise;
        }
    }
    s32Ret = CVI_ISP_SetTNRAttr(m_pipe,&nioseTnrAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_SetTNRAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

/*设置白平衡*/
ErrCodeE MediaChipISP::SetWb(OptModeE wbMode, uint8_t redGain, uint8_t blueGain)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_WB_ATTR_S wbAttrS = {0};
    s32Ret = CVI_ISP_GetWBAttr(m_pipe,&wbAttrS);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetWBAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    wbAttrS.bByPass  = CVI_TRUE;
    if (OptModeE::Auto == wbMode){
        wbAttrS.enOpType = OP_TYPE_AUTO;
    }else{
        wbAttrS.enOpType = OP_TYPE_MANUAL;
        wbAttrS.stManual.u16Bgain = blueGain;
        wbAttrS.stManual.u16Rgain = redGain;
    }

    s32Ret = CVI_ISP_SetWBAttr(m_pipe,&wbAttrS);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_SetWBAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetAntiFlicker(uint8_t antiFlicker, bool forceAntiFlickerEna){

    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_EXPOSURE_ATTR_S stExpAttr = {};
    s32Ret = CVI_ISP_GetExposureAttr(m_pipe, &stExpAttr);//获取AE曝光属性
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }
    /*stAuto 自动曝光属性结构体 stAntiflicker 抗闪烁属性设置 自动抗闪烁模式ISP_ANTIFLICKER_AUTO_MODE*/
    stExpAttr.stAuto.stAntiflicker.bEnable = CVI_TRUE;
    stExpAttr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_AUTO_MODE;
    stExpAttr.stAuto.stAntiflicker.enFrequency =
            antiFlicker == 50 ? AE_FREQUENCE_50HZ : AE_FREQUENCE_60HZ;
    s32Ret = CVI_ISP_SetExposureAttr(m_pipe, &stExpAttr);//设置AE曝光属性
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}