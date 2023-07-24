//
// Created by cx on 2023/2/16.
//

#include <core/EmxLog.hpp>
#include <json/value.h>
#include <core/utils/Misc.hpp>
#include "MediaChipDouLensStitch.hpp"

using namespace Emx;


ErrCodeE SetDwaWarpOut(const Json::Value &json) {
    VI_LDC_ATTR_S stLDCAttr;
    CVI_S32 s32Ret = CVI_SUCCESS;

    stLDCAttr.bEnable = 1;
    stLDCAttr.stAttr.stHomoGraphAttr.Enable = 1;
    stLDCAttr.stAttr.stHomoGraphAttr.isBlending = 1;
    stLDCAttr.stAttr.stHomoGraphAttr.RgnNum = 1;

    sprintf(stLDCAttr.stAttr.stHomoGraphAttr.homo_regions[0].grid_name, "%s", "region0");
    stLDCAttr.stAttr.bAspect = 1;
    stLDCAttr.stAttr.s32XYRatio = 50;
    stLDCAttr.stAttr.s32CenterXOffset = 0;
    stLDCAttr.stAttr.s32CenterYOffset = 0;
    stLDCAttr.stAttr.s32DistortionRatio = 0;
    s32Ret = CVI_VI_SetChnLDCAttr(0, 0, &stLDCAttr);
    if (s32Ret != CVI_SUCCESS) {
        return ErrCodeE::Failure;
    }
    stLDCAttr.stAttr.stHomoGraphAttr.RgnNum = 1;
    sprintf(stLDCAttr.stAttr.stHomoGraphAttr.homo_regions[0].grid_name, "%s", "region1");
    s32Ret = CVI_VI_SetChnLDCAttr(0, 1, &stLDCAttr);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_VPSS_SetChnLDCAttr fail\n");
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

ErrCodeE LoadGridinfo(const char *grid_file_name, const char *grid_name, SIZE_S *dwaOutPutSize, int coners[]) {

    int *pgrid_src, *pgrid_dst;
    int *pmesh_src, *pmesh_dst;
    int *pnode_src, *pnode_dst;

    emxlogi("grid info name : [%s], region :[%s]\n", grid_file_name, grid_name);
    FILE *fpGrid = fopen(grid_file_name, "rb");
    if (!fpGrid) {
        emxlogi("fopen file %s failed\n", grid_file_name);
        return ErrCodeE::OpenFailed;
    }

    int info[100] = {0};

    fread(info, sizeof(int), 100, fpGrid);

    emxlogi("%d %d %d %d %d\n", info[0], info[1], info[2], info[3], info[4]);

    dwaOutPutSize->u32Width = info[3];
    dwaOutPutSize->u32Height = info[4];
    emxlogi("dewap output size: %d %d\n", dwaOutPutSize->u32Width, dwaOutPutSize->u32Height);


    memcpy(coners, info + 11, sizeof(int) * 8);
    SAMPLE_PRT("coners: %d %d %d %d %d %d %d %d\n", coners[0], coners[1], coners[2],
               coners[3], coners[4], coners[5], coners[6], coners[7]);

    pgrid_src = (int *) calloc(info[2] * 2, sizeof(int));
    pgrid_dst = (int *) calloc(info[2] * 2, sizeof(int));
    pmesh_src = (int *) calloc(info[2] * 8, sizeof(int));
    pmesh_dst = (int *) calloc(info[2] * 8, sizeof(int));
    pnode_src = (int *) calloc((info[0] + 1) * (info[1] + 1) * 2, sizeof(int));
    pnode_dst = (int *) calloc((info[0] + 1) * (info[1] + 1) * 2, sizeof(int));

    fread(pgrid_src, sizeof(int), (info[2] * 2), fpGrid);
    fread(pgrid_dst, sizeof(int), (info[2] * 2), fpGrid);
    // hw mesh
    fread(pmesh_src, sizeof(int), (info[2] * 8), fpGrid);
    fread(pmesh_dst, sizeof(int), (info[2] * 8), fpGrid);
    // hw node
    fread(pnode_src, sizeof(int), ((info[0] + 1) * (info[1] + 1) * 2), fpGrid);
    fread(pnode_dst, sizeof(int), ((info[0] + 1) * (info[1] + 1) * 2), fpGrid);

    CVI_GDC_SetGridInfo((char *) grid_name, info, pgrid_src, pgrid_dst, pmesh_src, pmesh_dst, pnode_src, pnode_dst);
    free(pgrid_src);
    free(pgrid_dst);
    free(pmesh_src);
    free(pmesh_dst);
    free(pnode_src);
    free(pnode_dst);
    fclose(fpGrid);
    return ErrCodeE::Success;
}

ErrCodeE InitStitchParam(SIZE_S dwaOutPutSize, int conersLeft[], int conersRight[], int len) {
    int out_blend_start = 0, out_blend_end = 0;
    CVI_S32 Ret = CVI_SUCCESS;
    StitchParam param;
    memset(&param, 0, sizeof(STIT_PARAM));

    param.mode = 2;
    param.stSize[0] = dwaOutPutSize.u32Width;
    param.stSize[1] = dwaOutPutSize.u32Height;
    param.stSize[2] = dwaOutPutSize.u32Width;
    param.stSize[3] = dwaOutPutSize.u32Height;
    param.stit_nbr = 2;

    memcpy(param.corners, conersRight, len);
    memcpy(param.corners_left, conersLeft, len);

    Ret = MediaChipDouLensStitch::GetInst()->StitchInit(&param);
    if (Ret != CVI_SUCCESS) {
        emxloge("StitchInit Failed!!!\n");
        return ErrCodeE::Failure;
    }

    out_blend_start = param.blend_start_line;
    out_blend_end = param.blend_end_line;
    Ret = MediaChipDouLensStitch::GetInst()->StitchSetRegion(out_blend_start, out_blend_end, &param);
    if (Ret != CVI_SUCCESS) {
        emxloge("StitchInit Failed!!!\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void DeInitStitch(void) {
    MediaChipDouLensStitch::GetInst()->StitchFree();
}

ErrCodeE DumpViDwaMesh(char *mesh_file_name, int vichn) {
    int tmp;
    CVI_S32 s32Ret = 0;
    MESH_DUMP_ATTR_S meshDumpAttr = {0};

    snprintf(meshDumpAttr.binFileName, 128, "%s", mesh_file_name);
    emxlogi("binFileName: %s \n", meshDumpAttr.binFileName);
    tmp = CVI_ID_VI;        //use vi
    meshDumpAttr.enModId = (MOD_ID_E) tmp;
    meshDumpAttr.viMeshAttr.chn = vichn;

    s32Ret = CVI_GDC_DumpMesh(&meshDumpAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge(" CVI_GDC_DumpMesh fail\n ");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

CVI_S32 LoadViDwaMesh(char *mesh_file_name, char *grid_name, int vichn) {
    int tmp;
    CVI_S32 s32Ret = 0;
    MESH_DUMP_ATTR_S meshDumpAttr = {0};

    snprintf(meshDumpAttr.binFileName, 128, "%s", mesh_file_name);
    tmp = CVI_ID_VI;        //use vi
    meshDumpAttr.enModId = (MOD_ID_E) tmp;
    meshDumpAttr.viMeshAttr.chn = vichn;

    s32Ret = CVI_GDC_LoadMesh(&meshDumpAttr, grid_name, CVI_TRUE);
    if (s32Ret != CVI_SUCCESS) {
        SAMPLE_PRT(" CVI_GDC_DumpMesh fail\n ");
    }
    return s32Ret;
}

ErrCodeE InitStitch(int stitchMode, const Json::Value &json) {

    ErrCodeE e = ErrCodeE::Success;
    SIZE_S dwaOutPutSize;
    int conersLeft[8] = {0}, conersRight[8] = {0};
    char leftpath[EMX_MAX_PATH_SIZE] = {};
    char rightpath[EMX_MAX_PATH_SIZE] = {};
    snprintf(leftpath, sizeof(leftpath),
             "/root/configs/dat/L_%d.dat", stitchMode);
    snprintf(rightpath, sizeof(rightpath),
             "/root/configs/dat/R_%d.dat", stitchMode);

    e = LoadGridinfo(leftpath, "region0", &dwaOutPutSize, conersLeft);
    if (e != ErrCodeE::Success)
        return e;

    e = LoadGridinfo(rightpath, "region1", &dwaOutPutSize, conersRight);
    if (e != ErrCodeE::Success)
        return e;


    snprintf(leftpath, sizeof(leftpath),
             "%s/dat/meshL_%d.bin",Misc::GetDataDir(),stitchMode);
    snprintf(rightpath, sizeof(rightpath),
             "%s/dat/meshR_%d.bin",Misc::GetDataDir(),stitchMode);
    if (access(leftpath, F_OK) || access(rightpath, F_OK)) {
    e = SetDwaWarpOut(json);
        if (e != ErrCodeE::Success)
            return e;
        DumpViDwaMesh(leftpath, 0);
        DumpViDwaMesh(rightpath, 1);
    } else {
        LoadViDwaMesh(leftpath, (char *) "region0", 0);
        LoadViDwaMesh(rightpath, (char *) "region1", 1);
    }

    CVI_S32 ret = 0;
    VPSS_CROP_INFO_S stCropInfo;
    memset(&stCropInfo, 0, sizeof(VPSS_CROP_INFO_S));
    stCropInfo.bEnable = CVI_TRUE;
    stCropInfo.enCropCoordinate = VPSS_CROP_RATIO_COOR;
    stCropInfo.stCropRect.s32X = conersLeft[0];
    stCropInfo.stCropRect.s32Y = conersLeft[1];
    stCropInfo.stCropRect.u32Width = conersRight[2] - conersLeft[0];
    stCropInfo.stCropRect.u32Height = conersRight[5] - conersLeft[1];
    ret = CVI_VPSS_SetChnCrop(0, 0, &stCropInfo);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_VPSS_SetChnCrop fail\n");
    }

    e = InitStitchParam(dwaOutPutSize, conersLeft, conersRight, sizeof(conersLeft));
    if (e != ErrCodeE::Success)
        return e;

    return ErrCodeE::Success;
}

MediaChipDouLensStitch::MediaChipDouLensStitch() {
    m_imgSize.u32Width = 0;
    m_imgSize.u32Height = 0;
    m_mode = 1;
}

MediaChipDouLensStitch *MediaChipDouLensStitch::GetInst() {
    static MediaChipDouLensStitch *_instance = NULL;
    if (_instance == NULL) {
        _instance = new MediaChipDouLensStitch();
    }
    return _instance;
}

CVI_S32 MediaChipDouLensStitch::StitchInit(StitchParam *param) {
    int img_width = 0;
    int img_height = 0;

    CVI_S32 Ret = 0;
    PointXY leftTop, leftBottom, rightTop_left;

    m_imgSize.u32Width = param->stSize[0];
    m_imgSize.u32Height = param->stSize[1];
    m_mode = param->mode;
    memcpy(m_cornersRight, param->corners, sizeof(CVI_S32) * 8);
    memcpy(m_cornersLeft, param->corners_left, sizeof(CVI_S32) * 8);

    img_width = m_imgSize.u32Width;
    img_height = m_imgSize.u32Height;

    emxlogi("stitch,img_width,img_height: %d %d\n", img_width, img_height);

    leftTop.x = param->corners[0];
    leftTop.y = param->corners[1];
    leftBottom.x = param->corners[2];
    leftBottom.y = param->corners[3];
    rightTop_left.x = param->corners_left[2];
    rightTop_left.y = param->corners_left[3];

    m_handleIve = CVI_IVE_CreateHandle();
    Ret = CVI_IVE_CreateImage(m_handleIve, &m_alpha, IVE_IMAGE_TYPE_YUV420P, img_width * 1, img_height);
    if (Ret != CVI_SUCCESS) {
        emxloge("sample_stitch_param_init fail\n");
        return CVI_FAILURE;
    }

    param->out_blend_start = 0;
    param->out_blend_end = 0;
    param->blend_start_line = 0;
    param->blend_end_line = 0;

    for (int c = 0; c < 3; c++) {
        uint32_t plane_width = c < 1 ? img_width : img_width / 2;
        uint32_t plane_height = c < 1 ? img_height : img_height / 2;

        uint32_t gradient_start = std::min(leftTop.x, leftBottom.x);
        gradient_start = (c < 1) ? gradient_start : (gradient_start / 2);

        uint32_t gradient_end = c < 1 ? rightTop_left.x : rightTop_left.x / 2;
        gradient_end = (gradient_end > plane_width) ? plane_width : gradient_end;
        if (c == 0) {
            param->out_blend_start = gradient_start;
            param->out_blend_end = gradient_end;
            param->blend_start_line = gradient_start;
            param->blend_end_line = gradient_end;
            emxlogi("%s, blend_start, blend_end of Y plane: %d %d\n", __func__, gradient_start, gradient_end);
        }

        uint32_t gradient_length = gradient_end - gradient_start;
        uint32_t strideWidth = plane_width;
        if (gradient_length == 0) {
            Ret = CVI_FAILURE;
            break;
        }

        //uint32_t over_width = gradient_length + gradient_start;
        //float fval_a = 255.0/gradient_length;
        //float fval_b = (plane_width-gradient_start);
        uint8_t alpha_pixel/*, beta_pixel*/;

        for (uint32_t j = 0; j < plane_height; j++) {
            for (uint32_t i = 0; i < plane_width; i++) {
                if (i < gradient_start) {
                    //m_alpha.pu8VirAddr[c][i + j * strideWidth] = 255;
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = 0;
                } else if (i > gradient_end) {
                    //m_alpha.pu8VirAddr[c][i + j * strideWidth] = 0;
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = 255;
                } else {
                    //beta_pixel = (uint8_t)(((float)(over_width - i) * fval_a) );
                    //alpha_pixel = 255 - beta_pixel;
                    //m_alpha.pu8VirAddr[c][i + j * strideWidth] = beta_pixel;//alpha_pixel;
                    alpha_pixel = (uint8_t) (((float) (i - gradient_start) / gradient_length) * 255);
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = alpha_pixel;
                }
            }
        }
    }


    Ret = CVI_IVE_BufFlush(m_handleIve, &m_alpha);
    if (Ret != CVI_SUCCESS) {
        emxloge("CVI_IVE_BufFlush fail\n");
        return CVI_FAILURE;
    }

    emxlogi("Stitch Setting is done\n");
    return CVI_SUCCESS;
}

CVI_S32 MediaChipDouLensStitch::Stitch(VIDEO_FRAME_INFO_S *dstVideoFrameLft,
                                VIDEO_FRAME_INFO_S *dstVideoFrameRht,
                                VIDEO_FRAME_INFO_S *dstVideoFrameOut) {
    CVI_S32 Ret = CVI_SUCCESS;
    CVI_IVE_VideoFrameInfo2Image(dstVideoFrameLft, &m_src1yuv);
    CVI_IVE_VideoFrameInfo2Image(dstVideoFrameRht, &m_src2yuv);
    CVI_IVE_VideoFrameInfo2Image(dstVideoFrameOut, &m_dstyuv);
    if (m_mode) {
        if (m_mode == 2) {
            CVI_IVE_Blend_Pixel(m_handleIve, &m_src1yuv, &m_src2yuv, &m_alpha, &m_dstyuv, false);
            CVI_IVE_BufFlush(m_handleIve, &m_dstyuv);
        }
    }

    CVI_SYS_FreeI(m_handleIve, &m_src1yuv);
    CVI_SYS_FreeI(m_handleIve, &m_src2yuv);
    CVI_SYS_FreeI(m_handleIve, &m_dstyuv);

    return Ret;
}

CVI_S32 MediaChipDouLensStitch::StitchSetRegion(int blending_start, int blending_end, StitchParam *param) {
    if (blending_start > blending_end) {
        emxloge("error! Region is incorrect.\n");
        return CVI_FAILURE;
    }

    CVI_S32 Ret = CVI_SUCCESS;

    m_imgSize.u32Width = param->stSize[0];
    m_imgSize.u32Height = param->stSize[1];
    m_mode = param->mode;

    memcpy(m_cornersRight, param->corners, sizeof(CVI_S32) * 8);
    memcpy(m_cornersLeft, param->corners_left, sizeof(CVI_S32) * 8);

    int img_width = m_imgSize.u32Width;
    int img_height = m_imgSize.u32Height;

    for (int c = 0; c < 3; c++) {
        uint32_t plane_width = c < 1 ? img_width : img_width / 2;
        uint32_t plane_height = c < 1 ? img_height : img_height / 2;

        uint32_t gradient_start = blending_start;//min(_leftTop.x,_leftBottom.x) ;
        gradient_start = (c < 1) ? gradient_start : (gradient_start / 2);

        uint32_t gradient_end = blending_end;//c < 1 ?_rightTop_left.x:_rightTop_left.x/2;
        gradient_end = (c < 1) ? gradient_end : (gradient_end / 2);
        gradient_end = (gradient_end > plane_width) ? plane_width : gradient_end;

        uint32_t gradient_length = gradient_end - gradient_start;
        uint32_t strideWidth = plane_width;

        if (gradient_length == 0) {
            Ret = CVI_FAILURE;
            break;
        }

        uint32_t over_width = gradient_length + gradient_start;
        float fval_a = 255.0 / gradient_length;
        //float fval_b = (plane_width-gradient_start);
        uint8_t /*alpha_pixel,*/ beta_pixel;

        for (uint32_t j = 0; j < plane_height; j++) {
            for (uint32_t i = 0; i < plane_width; i++) {
                if (i < gradient_start) {
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = 255;
                } else if (i > gradient_end) {
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = 0;
                } else {
                    beta_pixel = (uint8_t) (((float) (over_width - i) * fval_a));
                    //alpha_pixel = 255 - beta_pixel;
                    m_alpha.pu8VirAddr[c][i + j * strideWidth] = beta_pixel;//alpha_pixel;
                }
            }
        }
    }

    CVI_IVE_BufFlush(m_handleIve, &m_alpha);
    emxlogi("Stitch Setting is done\n");
    return Ret;
}

CVI_S32 MediaChipDouLensStitch::StitchFree() {
    CVI_SYS_FreeI(m_handleIve, &m_alpha);
    CVI_IVE_DestroyHandle(m_handleIve);

    return CVI_SUCCESS;
}