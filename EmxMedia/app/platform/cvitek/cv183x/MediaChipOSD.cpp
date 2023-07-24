#include "EmxCore.hpp"
#include "MediaChipOSD.hpp"

using namespace Emx;

ErrCodeE MediaChipOSD::Start() {
    if (m_started)
        return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    if (Config() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    m_started = true;
    emxlogi("osd chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipOSD::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    DestroyTime();
    DestroyText();
    DestroyImage();
    emxlogi("osd chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipOSD::SetParam(const std::vector<std::shared_ptr<Param>> &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    DestroyAllParam();
    DestroyAllOutline();
    m_param = param;
    for (auto &e: m_param) {
        if (e->type == MediaOSD::TypeE::Time)
            m_paramTime.push_back(&e->time);
        else if (e->type == MediaOSD::TypeE::Text)
            m_paramText.push_back(&e->text);
        else if (e->type == MediaOSD::TypeE::Image)
            m_paramImage.push_back(&e->image);
    }
    SaveParam();
    return Start();
}

ErrCodeE MediaChipOSD::GetParam(std::vector<std::shared_ptr<Param>> &param) {
    param = m_param;
    return ErrCodeE::Success;
}

ErrCodeE MediaChipOSD::Config() {
    Json::Value root;
    m_res.configs.GetCfg("osd", root);
    auto &json = root[m_chn]["attr"];
    m_vpssGrp = (VPSS_GRP) json["VpssGrp"].asInt();
    m_vpssChn = (VPSS_CHN) json["VpssChn"].asInt();
    CVI_S32 s32Ret;
    VPSS_CHN_ATTR_S stChnAttr = {};
    s32Ret = CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (s32Ret != 0) {
        emxloge("CVI_VPSS_GetChnAttr error ret=%#x !\n", s32Ret);
        return ErrCodeE::Failure;
    }
    m_chnWidth = (int) stChnAttr.u32Width;
    m_chnHeight = (int) stChnAttr.u32Height;

    m_osdIdx = 0;
    if (CreateTime() != ErrCodeE::Success)
        emxloge("CreateTime failed\n");
    if (CreateText() != ErrCodeE::Success)
        emxloge("CreateText failed\n");
    if (CreateImage() != ErrCodeE::Success)
        emxloge("CreateImage failed\n");
    return ErrCodeE::Success;
}

ErrCodeE MediaChipOSD::CreateTime() {
    if (m_paramTime.empty())
        return ErrCodeE::Success;
    for (auto e: m_paramTime) {
        OsdTime time = {};
        time.handle = m_chn * MaxOSDNum + m_osdIdx++;
        if (time.handle >= (RGN_HANDLE) ((m_chn + 1) * MaxOSDNum))
            continue;
        char path[EMX_MAX_PATH_SIZE] = {};
        JoinFontPath(e->font, path);
        time.outline = CreateOutline(e->size, path);
        time.time = e;
        char content[MaxTextSize];
        GetTime(0, e->format, MaxTextSize, content);
        int x, y, w, h;
        time.outline->Write(e->thickness, content, &w, &h);
        CalcCoordinate(e->margin, w, h, m_chnWidth, m_chnHeight, x, y);
        RegionSet(time.handle, x, y, w, h, PIXEL_FORMAT_ARGB_1555, e->layer, e->ena);
        m_osdTime.push_back(time);
    }
    if (m_osdTime.empty())
        return ErrCodeE::Success;
    m_timer.Create(m_res.loop);
    m_timer.Start(0, 200, [this]() {
        time_t t;
        time(&t);
        if (m_timeRecord != t) {
            m_timeRecord = t;
            char content[MaxTextSize];
            int w, h;
            for (auto e: m_osdTime) {
                GetTime(t, e.time->format, MaxTextSize, content);
                auto pix = e.outline->Write(e.time->thickness, content, &w, &h);
                if (pix) {
                    BITMAP_S stBitmap = {};
                    stBitmap.u32Width = w;
                    stBitmap.u32Height = h;
                    stBitmap.pData = pix;
                    stBitmap.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
                    CVI_RGN_SetBitMap(e.handle, &stBitmap);
                }
            }
        }
    });
    return ErrCodeE::Success;
}

void MediaChipOSD::DestroyTime() {
    if (m_osdTime.empty())
        return;
    m_timer.Destroy();
    for (auto e: m_osdTime) {
        CVI_RGN_Destroy(e.handle);
        DestroyOutline(e.outline);
    }
    m_osdTime.clear();
}

ErrCodeE MediaChipOSD::CreateText() {
    if (m_paramText.empty())
        return ErrCodeE::Success;
    for (auto e: m_paramText) {
        OsdText text = {};
        text.handle = m_chn * MaxOSDNum + m_osdIdx++;
        if (text.handle >= (RGN_HANDLE) ((m_chn + 1) * MaxOSDNum))
            continue;
        char path[EMX_MAX_PATH_SIZE] = {};
        JoinFontPath(e->font, path);
        text.outline = CreateOutline(e->size, path);
        text.text = e;
        int x, y, w, h;
        auto pix = text.outline->Write(e->thickness, e->content, &w, &h);
        CalcCoordinate(e->margin, w, h, m_chnWidth, m_chnHeight, x, y);
        RegionSet(text.handle, x, y, w, h, PIXEL_FORMAT_ARGB_1555, e->layer, e->ena);
        if (pix) {
            BITMAP_S stBitmap = {};
            stBitmap.u32Width = w;
            stBitmap.u32Height = h;
            stBitmap.pData = pix;
            stBitmap.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
            CVI_RGN_SetBitMap(text.handle, &stBitmap);
        }
        m_osdText.push_back(text);
    }
    return ErrCodeE::Success;
}

void MediaChipOSD::DestroyText() {
    for (auto e: m_osdText) {
        CVI_RGN_Destroy(e.handle);
        DestroyOutline(e.outline);
    }
    m_osdText.clear();
}

ErrCodeE MediaChipOSD::CreateImage() {
    if (m_paramImage.empty())
        return ErrCodeE::Success;
    for (auto e: m_paramImage) {
        OsdImage image = {};
        image.handle = m_chn * MaxOSDNum + m_osdIdx++;
        if (image.handle >= (RGN_HANDLE) ((m_chn + 1) * MaxOSDNum))
            continue;
        image.image = e;
        int x, y, w, h;
        w = e->width;
        h = e->height;
        CalcCoordinate(e->margin, w, h, m_chnWidth, m_chnHeight, x, y);
        RegionSet(image.handle, x, y, w, h, PIXEL_FORMAT_ARGB_8888, e->layer, e->ena);
        ImageToCanvas(image);
        m_osdImage.push_back(image);
    }
    return ErrCodeE::Success;
}

void MediaChipOSD::DestroyImage() {
    for (auto e: m_osdImage) {
        CVI_RGN_Destroy(e.handle);
    }
    m_osdImage.clear();
}

ErrCodeE MediaChipOSD::RegionSet(RGN_HANDLE handle,
                                 int x, int y, int w, int h,
                                 PIXEL_FORMAT_E pixFmt, int layer, bool show) {
    CVI_S32 s32Ret;
    RGN_ATTR_S stRgnAttr = {};
    RGN_CHN_ATTR_S stChnAttr = {};
    MMF_CHN_S stChn = {};
    stChn.enModId = CVI_ID_VPSS;
    stChn.s32DevId = m_vpssGrp;
    stChn.s32ChnId = m_vpssChn;
#if 1
    //OverlayEx
    stRgnAttr.unAttr.stOverlayEx.stSize.u32Width = w;
    stRgnAttr.unAttr.stOverlayEx.stSize.u32Height = h;
    stRgnAttr.unAttr.stOverlayEx.enPixelFormat = pixFmt;
    stRgnAttr.unAttr.stOverlayEx.u32BgColor = 0;
    stRgnAttr.unAttr.stOverlayEx.u32CanvasNum = 2;
    stRgnAttr.enType = OVERLAYEX_RGN;
    stChnAttr.bShow = (CVI_BOOL) show;
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = x;
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = y;
    stChnAttr.unChnAttr.stOverlayExChn.u32Layer = layer;
    stChnAttr.enType = OVERLAYEX_RGN;
#else
    //Overlay
    stRgnAttr.unAttr.stOverlay.stSize.u32Width = w;
    stRgnAttr.unAttr.stOverlay.stSize.u32Height = h;
    stRgnAttr.unAttr.stOverlay.enPixelFormat = pixFmt;
    stRgnAttr.unAttr.stOverlay.u32BgColor = 0;
    stRgnAttr.unAttr.stOverlay.u32CanvasNum = 1;
    stRgnAttr.enType = OVERLAY_RGN;
    stChnAttr.bShow = (CVI_BOOL) show;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = x;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = y;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = layer;
    stChnAttr.enType = OVERLAY_RGN;
#endif
    emxlogd("handle:%d,x:%d,y:%d,w:%d,h:%d\n", handle, x, y, w, h);
    s32Ret = CVI_RGN_Create(handle, &stRgnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_RGN_Create %d failed! s32Ret: 0x%x.\n", m_chn, handle, s32Ret);
        return ErrCodeE::Failure;
    }
    s32Ret = CVI_RGN_AttachToChn(handle, &stChn, &stChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_RGN_AttachToChn %d failed! s32Ret: 0x%x.\n", m_chn, handle, s32Ret);
        CVI_RGN_Destroy(handle);
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipOSD::ImageToCanvas(OsdImage &image) {
    CVI_S32 s32Ret;
    RGN_CANVAS_INFO_S stCanvasInfo = {};
    s32Ret = CVI_RGN_GetCanvasInfo(image.handle, &stCanvasInfo);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_RGN_GetCanvasInfo %d failed! s32Ret: 0x%x.\n", m_chn, image.handle, s32Ret);
        return ErrCodeE::Failure;
    }

    if (!stCanvasInfo.pu8VirtAddr) {
        emxloge("chn %d handle %d pu8VirtAddr null\n", m_chn, image.handle);
        return ErrCodeE::Failure;
    }
    std::string buffer;
    if (File::Read(image.image->path, buffer) != ErrCodeE::Success) {
        emxloge("read image %s failed\n", image.image->path);
        return ErrCodeE::Failure;
    }
    auto dst = (char *) stCanvasInfo.pu8VirtAddr;
    auto src = (const char *) buffer.data();

    for (int i = 0; i < (int) stCanvasInfo.stSize.u32Height; i++) {
        memcpy(dst, src, stCanvasInfo.stSize.u32Width * 4);
        dst += stCanvasInfo.u32Stride;
        src += stCanvasInfo.stSize.u32Width * 4;
    }

    s32Ret = CVI_RGN_UpdateCanvas(image.handle);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_RGN_UpdateCanvas %d failed! s32Ret: 0x%x.\n", m_chn, image.handle, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
