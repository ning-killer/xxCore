/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_isp.c
 * Description:
 *   Common ctrl code for isp.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <dlfcn.h>

#include "sample_comm.h"
#include "cvi_awb.h"
#include "cvi_af.h"

#include "cvi_sns_ctrl.h"
#include "cvi_ae.h"
#include "cvi_isp.h"

#ifdef SUPPORT_ISP_PQTOOL
#include <dlfcn.h>
static CVI_BOOL g_ISPDaemon = CVI_FALSE;
static void *g_ISPDHandle;
#define ISPD_LIBNAME "libcvi_ispd.so"
#define ISPD_CONNECT_PORT 5566
#endif //

static pthread_t g_IspPid[VI_MAX_DEV_NUM];
static CVI_U32 g_au32IspSnsId[ISP_MAX_DEV_NUM] = { 0, 1 };

SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM] = {
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX290_MIPI_1M_30FPS_12BIT,
};

static ISP_INIT_ATTR_S gstInitAttr[ISP_MAX_DEV_NUM];

#ifdef FPGA_PORTING
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_1M_30FPS = { { 0, 0, 1280, 720 }, { 1280, 720 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};
#endif
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_60FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 60, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_5M_30FPS = { { 0, 0, 2592, 1944 }, { 2592, 1944 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_5M_60FPS = { { 0, 0, 2592, 1944 }, { 2592, 1944 }, 60, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_4M_60FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 }, 60, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_4M_1600P_30FPS = { { 0, 0, 2560, 1600 }, { 2560, 1600 },
						25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_2M_60FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 60, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX334_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_2M_60FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 60, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX385_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_RGGB, WDR_MODE_NONE, 0};

#ifdef ARCH_CV182X
ISP_PUB_ATTR_S ISP_PUB_ATTR_F23_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_BGRGI, WDR_MODE_NONE, 0};
#else
ISP_PUB_ATTR_S ISP_PUB_ATTR_F23_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_BGGR, WDR_MODE_NONE, 0};
#endif
ISP_PUB_ATTR_S ISP_PUB_ATTR_F35_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_H65_1M_30FPS = { { 0, 0, 1280, 720 }, { 1280, 720 }, 25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_K04_4M_30FPS = { { 0, 0, 2688, 1520 }, { 2688, 1520 }, 25, BAYER_BGGR, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OV4689_4M_30FPS = { { 0, 0, 2688, 1520 }, { 2688, 1520 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A20_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A20_5M_30FPS = { { 0, 0, 2592, 1944 }, { 2592, 1944 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A20_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_PICO_640 = { { 0, 0, 632, 479 }, { 632, 479 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};


ISP_PUB_ATTR_S ISP_PUB_ATTR_PICO_384 = { { 0, 0, 384, 288 }, { 384, 288 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MCS369Q = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MCS369 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MM308M2 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_NEXTCHIP_N5 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_NEXTCHIP_N5_1M = { { 0, 0, 1280, 720 }, { 1280, 720 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_NEXTCHIP_N6 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC3335_3M_30FPS = { { 0, 0, 2304, 1296 }, { 2304, 1296 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC3336_3M_30FPS = { { 0, 0, 2304, 1296 }, { 2304, 1296 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC2335_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC2336_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4336_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC200AI_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC500AI_5M_30FPS = { { 0, 0, 2880, 1620 }, { 2880, 1620 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC500AI_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC501AI_2L_5M_30FPS = { { 0, 0, 2880, 1620 }, { 2880, 1620 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC401AI_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2020_1M = { { 0, 0, 1280, 720 }, { 1280, 720 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2020_2M = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2100_2M = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2100_2M_2CH = { { 0, 0, 3844, 1124 }, { 3844, 1124 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2100_2M_4CH = { { 0, 0, 7688, 1124 }, { 7688, 1124 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC8238_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4210_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2053_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2093_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_GC4653_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						25, BAYER_GRBG, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_GC1054_1M_30FPS = { { 0, 0, 1280, 720 }, { 1280, 720 },
						25, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX347_4M_30FPS = { { 0, 0, 2688, 1520 }, { 2688, 1520 },
						30, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX347_4M_60FPS = { { 0, 0, 2688, 1520 }, { 2688, 1520 },
						60, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC035HGS_480P_120FPS = { { 0, 0, 640, 480 }, { 640, 480 },
						120, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC035GS_480P_120FPS = { { 0, 0, 640, 480 }, { 640, 480 },
						120, BAYER_BGGR, WDR_MODE_NONE, 0};
void callback_FPS(int fps)
{
	static CVI_FLOAT uMaxFPS[VI_MAX_DEV_NUM] = {0};
	int i;

	for (i = 0; i < VI_MAX_DEV_NUM && g_IspPid[i]; i++) {
		ISP_PUB_ATTR_S pubAttr = {0};

		CVI_ISP_GetPubAttr(i, &pubAttr);
		if (uMaxFPS[i] == 0) {
			uMaxFPS[i] = pubAttr.f32FrameRate;
		}
		if (fps == 0) {
			pubAttr.f32FrameRate = uMaxFPS[i];
		} else {
			pubAttr.f32FrameRate = (CVI_FLOAT) fps;
		}
		CVI_ISP_SetPubAttr(i, &pubAttr);
	}
}

static CVI_VOID *SAMPLE_COMM_ISP_Thread(void *arg)
{
	CVI_S32 s32Ret = 0;
	CVI_U8 IspDev = *(CVI_U8 *)arg;
	char szThreadName[20];

	free(arg);
	snprintf(szThreadName, sizeof(szThreadName), "ISP%d_RUN", IspDev);
	prctl(PR_SET_NAME, szThreadName, 0, 0, 0);

	if (IspDev > 0) {
		SAMPLE_PRT("ISP Dev %d return\n", IspDev);
		return NULL;
	}

	CVI_SYS_RegisterThermalCallback(callback_FPS);

	SAMPLE_PRT("ISP Dev %d running!\n", IspDev);
	s32Ret = CVI_ISP_Run(IspDev);
	if (s32Ret != 0)
		SAMPLE_PRT("CVI_ISP_Run failed with %#x!\n", s32Ret);

	return NULL;
}

CVI_S32 SAMPLE_COMM_ISP_Run(CVI_U8 IspDev)
{
	CVI_S32 s32Ret = 0;
	CVI_U8 *arg = malloc(sizeof(*arg));
	struct sched_param param;
	pthread_attr_t attr;

	if (arg == NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "malloc failed\n");
		goto out;
	}

	*arg = IspDev;
	param.sched_priority = 80;

	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	s32Ret = pthread_create(&g_IspPid[IspDev], &attr, SAMPLE_COMM_ISP_Thread, arg);
	if (s32Ret != 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "create isp running thread failed!, error: %d, %s\r\n",
					s32Ret, strerror(s32Ret));
		goto out;
	}

#ifdef SUPPORT_ISP_PQTOOL
	if (!g_ISPDaemon) {
		g_ISPDHandle = dlopen(ISPD_LIBNAME, RTLD_NOW);

		if (g_ISPDHandle) {
			char *error = NULL;
			void (*daemon_init)(unsigned int port);

			printf("Load dynamic library %s success\n", ISPD_LIBNAME);

			dlerror();
			daemon_init = dlsym(g_ISPDHandle, "isp_daemon_init");
			error = dlerror();
			if (error == NULL) {
				(*daemon_init)(ISPD_CONNECT_PORT);
				g_ISPDaemon = CVI_TRUE;
			} else {
				printf("Run daemon initial fail\n");
				dlclose(g_ISPDHandle);
			}
		} else {
			printf("Load dynamic library %s fail\n", ISPD_LIBNAME);
		}
	}
#endif //

out:

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	switch (enSnsType) {
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
#endif
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;

	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_1600P_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_1600P_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX385_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX385_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_F23_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_F23_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_F35_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_F35_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;

	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_H65_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
		
	case SOI_K04_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_K04_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case OV_OV4689_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OV4689_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PICO_640, sizeof(ISP_PUB_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PICO_384, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MCS369Q, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MCS369, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MM308M2, sizeof(ISP_PUB_ATTR_S));
		break;
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_NEXTCHIP_N5, sizeof(ISP_PUB_ATTR_S));
		break;
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_NEXTCHIP_N6, sizeof(ISP_PUB_ATTR_S));
		break;
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_NEXTCHIP_N5_1M, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC3335_3M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC3336_3M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC2335_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC2336_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4336_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC200AI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC200AI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC500AI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC500AI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC500AI_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC500AI_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC501AI_2L_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC401AI_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2020_1M, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2020_1M, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->f32FrameRate = 30;
		break;
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2020_2M, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2020_2M, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->f32FrameRate = 30;
		break;
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
#ifdef ARCH_CV183X
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2100_2M, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2100_2M_2CH, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2100_2M_4CH, sizeof(ISP_PUB_ATTR_S));
		break;
#else
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2100_2M, sizeof(ISP_PUB_ATTR_S));
		break;
#endif
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC8238_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC8238_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		pstPubAttr->f32FrameRate = 15;
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2053_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2093_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2093_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_GC4653_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_5M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_2M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_GC1054_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX347_4M_60FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX347_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC035HGS_480P_120FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC035GS_480P_120FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	return s32Ret;
}

/******************************************************************************
 * funciton : stop ISP, and stop isp thread
 ******************************************************************************/
CVI_VOID SAMPLE_COMM_ISP_Stop(CVI_U8 IspDev)
{
	CVI_S32 s32Ret = CVI_FAILURE;
#ifdef SUPPORT_ISP_PQTOOL
	if (g_ISPDaemon) {
		char *error = NULL;
		void (*daemon_uninit)(void);

		daemon_uninit = dlsym(g_ISPDHandle, "isp_daemon_uninit");
		error = dlerror();
		if (error == NULL)
			(*daemon_uninit)();

		dlclose(g_ISPDHandle);
		g_ISPDHandle = NULL;
		g_ISPDaemon = CVI_FALSE;
	}
#endif //

	if (g_IspPid[IspDev]) {
		s32Ret = CVI_ISP_Exit(IspDev);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_ISP_Exit fail with %#x!\n", s32Ret);
			return;
		}
		pthread_join(g_IspPid[IspDev], NULL);
		g_IspPid[IspDev] = 0;
		SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
		SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
		SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
		#if ENABLE_AF_LIB
		SAMPLE_COMM_ISP_Aflib_UnCallback(IspDev);
		#endif
	}
}

CVI_VOID SAMPLE_COMM_All_ISP_Stop(CVI_VOID)
{
	for (ISP_DEV IspDev = 0; IspDev < VI_MAX_DEV_NUM; IspDev++)
		SAMPLE_COMM_ISP_Stop(IspDev);
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAwbLib;
	CVI_S32 s32Ret = 0;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	s32Ret = CVI_AWB_Register(IspDev, &stAwbLib);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AWB Algo register failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAwbLib;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	s32Ret = CVI_AWB_UnRegister(IspDev, &stAwbLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AWB Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAeLib;

	stAeLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	s32Ret = CVI_AE_Register(IspDev, &stAeLib);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AE Algo register failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAeLib;

	stAeLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	s32Ret = CVI_AE_UnRegister(IspDev, &stAeLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AE Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aflib_Callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAfLib;
	CVI_S32 s32Ret = 0;

	stAfLib.s32Id = IspDev;
	strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));
	s32Ret = CVI_AF_Register(IspDev, &stAfLib);

	if (s32Ret != CVI_SUCCESS) {
		printf("AF Algo register failed!, error: %d\n", s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aflib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAfLib;

	stAfLib.s32Id = IspDev;
	strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));
	s32Ret = CVI_AF_UnRegister(IspDev, &stAfLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AF Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}


CVI_S32 SAMPLE_COMM_ISP_SetSnsObj(CVI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType)
{
	if (u32SnsId >= ARRAY_SIZE(g_enSnsType))
		return CVI_FAILURE;

	g_enSnsType[u32SnsId] = enSnsType;
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_SetSnsInit(CVI_U32 u32SnsId, CVI_U8 u8HwSync)
{
	if (u32SnsId >= ARRAY_SIZE(g_enSnsType))
		return CVI_FAILURE;

	gstInitAttr[u32SnsId].u16UseHwSync = u8HwSync;

	return CVI_SUCCESS;
}
static void *g_sensor_lib_handle = NULL;
static int SAMPLE_COMM_ISP_Dlopen()
{
    g_sensor_lib_handle= dlopen ("/root/firmware/lib/libsns_full.so", RTLD_LAZY);
    if (!g_sensor_lib_handle) {
        printf ("dlopen err:%s \n", dlerror());
        return -1;
    }
    return 0;
}

static char *SAMPLE_COMM_ISP_BbjName(SAMPLE_SNS_TYPE_E enSnsType)
{
    switch (enSnsType) {
        case SOI_F23_MIPI_2M_30FPS_10BIT:
		    return "stSnsF23_Obj";
		case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	    case SONY_IMX290_MIPI_2M_60FPS_12BIT:
            return "stSnsImx290_2l_Obj";
		case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        /* fallthrough */
        case SONY_IMX327_MIPI_2M_60FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx327_Obj";
		case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx327_2l_Obj";
		case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	    case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx327_Slave_Obj";
		case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        /* fallthrough */
        case SONY_IMX307_MIPI_2M_60FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "&stSnsImx307_Obj";
		case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx307_2l_Obj";
		case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	    case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return  "stSnsImx307_Slave_Obj";
		case SOI_F35_MIPI_2M_30FPS_10BIT:
	    case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
            return "stSnsF35_Obj";
		case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	    case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
            return "stSnsF35_Slave_Obj";
		case SOI_H65_MIPI_1M_30FPS_10BIT:
            return "stSnsH65_Obj";
        case SOI_K04_MIPI_4M_30FPS_10BIT:
		    return "stSnsK04_Obj";
        case OV_OV4689_MIPI_4M_30FPS_10BIT:
            return "stSnsOv4689_Obj";
		case OV_OS08A20_MIPI_8M_30FPS_10BIT:
        case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
        case OV_OS08A20_MIPI_5M_30FPS_10BIT:
        case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case OV_OS08A20_MIPI_4M_30FPS_10BIT:
        case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return "stSnsOs08a20_Obj";
		case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
        case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
        case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
        case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
        case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return "stSnsOs08a20_Slave_Obj";
		case PICO640_THERMAL_479P:
            return "stSnsPICO640_Obj";
		case PICO384_THERMAL_384X288:
            return "stSnsPICO384_Obj";
		case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	    case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx327_Sublvds_Obj";
		case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
        /* fallthrough */
        case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
        case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
            return  "stSnsImx307_Sublvds_Obj";
		case VIVO_MCS369Q_4M_30FPS_12BIT:
            return "stSnsMCS369Q_Obj";
		case VIVO_MCS369_2M_30FPS_12BIT:
            return "stSnsMCS369_Obj";
		case VIVO_MM308M2_2M_25FPS_8BIT:
            return "stSnsMM308M2_Obj";
		case NEXTCHIP_N5_2M_25FPS_8BIT:
	    case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
            return "stSnsN5_Obj";
		case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
            return "stSnsN6_Obj";
		case SMS_SC3335_MIPI_3M_30FPS_10BIT:
            return "stSnsSC3335_Obj";
        case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
            return "stSnsSC3335_Slave_Obj";
		case SMS_SC3336_MIPI_3M_30FPS_10BIT:
            return "stSnsSC3336_Obj";
        case SMS_SC2335_MIPI_2M_30FPS_10BIT:
            return "stSnsSC2335_Obj";
        case SMS_SC2336_MIPI_2M_30FPS_10BIT:
            return "stSnsSC2336_Obj";
        case SMS_SC4336_MIPI_4M_30FPS_10BIT:
            return "stSnsSC4336_Obj";
		case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	    case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
            return "stSnsSC200AI_Obj";
		case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
        case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
        case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return "stSnsSC500AI_Obj";
		case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
            return "stSnsSC501AI_2L_Obj";
		case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
            return "stSnsSC401AI_Obj";
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	    case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx334_Obj";
		case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_5M_60FPS_10BIT:
        case SONY_IMX335_MIPI_4M_60FPS_10BIT:
        case SONY_IMX335_MIPI_2M_60FPS_10BIT:
            return "stSnsImx335_Obj";
		case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	    case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx385_Obj";
		case PIXELPLUS_PR2020_1M_25FPS_8BIT:
        case PIXELPLUS_PR2020_1M_30FPS_8BIT:
        case PIXELPLUS_PR2020_2M_25FPS_8BIT:
        case PIXELPLUS_PR2020_2M_30FPS_8BIT:
            return "stSnsPR2020_Obj";
		case PIXELPLUS_PR2100_2M_25FPS_8BIT:
            return "stSnsPR2100_Obj";
		case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
            return "stSnsPR2100_Obj";
		case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
            return "stSnsPR2100_Obj";
		case SMS_SC8238_MIPI_8M_30FPS_10BIT:
        case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
            return "stSnsSC8238_Obj";
		case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	    case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return "stSnsSC4210_Obj";
#ifdef FPGA_PORTING
        case SONY_IMX327_MIPI_1M_30FPS_10BIT:
        case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		    return "SnsImx327_fpga_Obj";
#endif
		case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
		    return "stSnsGc2053_Obj";
        case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
            return "stSnsGc2053_Slave_Obj";
        case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	    case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
            return "stSnsGc2093_Obj";
        case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	    case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
            return "stSnsGc2093_Slave_Obj";
        case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
            return "stSnsGc4653_Obj";
        case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
            return "stSnsGc1054_Obj";
        case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	    case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return "stSnsImx347_Obj";
        case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
            return "stSnsGc2053_1l_Obj";
		case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
            return "stSnsSC035HGS_Obj";
		case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
            return "stSnsSC035GS_Obj";
		default:
            return "stSns_Obj";
    }
}


static CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj_Auto(SAMPLE_SNS_TYPE_E enSnsType)
{
    printf("Use autofetch dynamic libraries! \n");
    static int handle_init_flag = 0;
    if(handle_init_flag == 0){
        SAMPLE_COMM_ISP_Dlopen();
        handle_init_flag = 1;
    }
    ISP_SNS_OBJ_S *pSnsObj = NULL;
    void *handle = g_sensor_lib_handle;
    if (!handle) {
        printf ("err: dlopen is NULL \n");
        handle_init_flag = 0;
        return NULL;
    }
    char *func_name = SAMPLE_COMM_ISP_BbjName(enSnsType);
    if(func_name == NULL){
        printf ("dlsym  err func_name is NULL \n");
        return NULL;
    }
    pSnsObj = dlsym(handle, func_name);
    if (pSnsObj == NULL)  {
        printf ("dlsym[%s] err:%s \n",func_name, dlerror());
    }
    printf("SAMPLE_COMM_ISP_GetSnsObj dlopen ok!func_name=%s \n",func_name);
    return pSnsObj;
}

CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj(CVI_U32 u32SnsId)
{
	SAMPLE_SNS_TYPE_E enSnsType;
	CVI_VOID *pSnsObj;

	enSnsType = g_enSnsType[u32SnsId];
	switch (enSnsType) {
#if defined(SENSOR_SOI_F23)
	case SOI_F23_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsF23_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX290_2L)
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		pSnsObj = &stSnsImx290_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327)
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	/* fallthrough */
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_2L)
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_SLAVE)
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307)
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	/* fallthrough */
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_2L)
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_SLAVE)
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F35)
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsF35_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F35_SLAVE)
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsF35_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SOI_H65)
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		pSnsObj = &stSnsH65_Obj;
		break;
#endif
#if defined(SENSOR_SOI_K04)
	case SOI_K04_MIPI_4M_30FPS_10BIT:
		return &stSnsK04_Obj;
#endif

#if defined(SENSOR_OV_OV4689)
	case OV_OV4689_MIPI_4M_30FPS_10BIT:
		pSnsObj = &stSnsOv4689_Obj;
		break;
#endif

#if defined(SENSOR_OV_OS08A20)
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs08a20_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS08A20_SLAVE)
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs08a20_Slave_Obj;
		break;
#endif
#if defined(SENSOR_PICO_640)
	case PICO640_THERMAL_479P:
		pSnsObj = &stSnsPICO640_Obj;
		break;
#endif
#if defined(SENSOR_PICO_384)
	case PICO384_THERMAL_384X288:
		pSnsObj = &stSnsPICO384_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_SUBLVDS)
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Sublvds_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_SUBLVDS)
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	/* fallthrough */
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Sublvds_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MCS369Q)
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		pSnsObj = &stSnsMCS369Q_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MCS369)
	case VIVO_MCS369_2M_30FPS_12BIT:
		pSnsObj = &stSnsMCS369_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MM308M2)
	case VIVO_MM308M2_2M_25FPS_8BIT:
		pSnsObj = &stSnsMM308M2_Obj;
		break;
#endif
#if defined(SENSOR_NEXTCHIP_N5)
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
		pSnsObj = &stSnsN5_Obj;
		break;
#endif
#if defined(SENSOR_NEXTCHIP_N6)
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
		pSnsObj = &stSnsN6_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3335)
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3335_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3335_SLAVE)
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3335_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3336)
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC2335)
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsSC2335_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC2336)
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsSC2336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC4336)
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		pSnsObj = &stSnsSC4336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC200AI)
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC200AI_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC500AI)
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
	/* fallthrough */
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	/* fallthrough */
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC500AI_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC501AI_2L)
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		pSnsObj = &stSnsSC501AI_2L_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC401AI)
		case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
			pSnsObj = &stSnsSC401AI_Obj;
			break;
#endif
#if defined(SENSOR_SONY_IMX334)
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx334_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX335)
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
		pSnsObj = &stSnsImx335_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX385)
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx385_Obj;
		break;
#endif
#if defined(SENSOR_PIXELPLUS_PR2020)
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
		pSnsObj = &stSnsPR2020_Obj;
		break;
#endif
#if defined(SENSOR_PIXELPLUS_PR2100)
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
		pSnsObj = &stSnsPR2100_Obj;
		break;
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		pSnsObj = &stSnsPR2100_Obj;
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		pSnsObj = &stSnsPR2100_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC8238)
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC8238_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC4210)
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC4210_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_FPGA) && defined(FPGA_PORTING)
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_fpga_Obj;
		break;
#endif
#if defined(SENSOR_GCORE_GC2053)
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_Obj;
#endif
#if defined(SENSOR_GCORE_GC2053_SLAVE)
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_Slave_Obj;
#endif
#if defined(SENSOR_GCORE_GC2093)
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsGc2093_Obj;
#endif
#if defined(SENSOR_GCORE_GC2093_SLAVE)
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsGc2093_Slave_Obj;
#endif
#if defined(SENSOR_GCORE_GC4653)
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		return &stSnsGc4653_Obj;
#endif
#if defined(SENSOR_GCORE_GC1054)
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		return &stSnsGc1054_Obj;
#endif
#if defined(SENSOR_SONY_IMX347)
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx347_Obj;
#endif
#if defined(SENSOR_GCORE_GC2053_1L)
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsGc2053_1l_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC035HGS)
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
		pSnsObj = &stSnsSC035HGS_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC035GS)
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		pSnsObj = &stSnsSC035GS_Obj;
		break;
#endif

	default:
		pSnsObj = SAMPLE_COMM_ISP_GetSnsObj_Auto(enSnsType);
		break;
	}

	return pSnsObj;
}

CVI_S32 SAMPLE_COMM_ISP_PatchSnsObj(CVI_U32 u32SnsId, SAMPLE_SENSOR_INFO_S *pstSnsInfo)
{
	ISP_SNS_OBJ_S *pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	RX_INIT_ATTR_S stRxInitAttr;
	unsigned int i;

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	memset(&stRxInitAttr, 0, sizeof(RX_INIT_ATTR_S));

	stRxInitAttr.MipiDev = pstSnsInfo->MipiDev;
	if (pstSnsInfo->stMclkAttr.bMclkEn) {
		stRxInitAttr.stMclkAttr.bMclkEn = CVI_TRUE;
		stRxInitAttr.stMclkAttr.u8Mclk  = pstSnsInfo->stMclkAttr.u8Mclk;
	}

	for (i = 0; i < sizeof(stRxInitAttr.as16LaneId)/sizeof(CVI_S16); i++) {
		stRxInitAttr.as16LaneId[i] = pstSnsInfo->as16LaneId[i];
	}
	for (i = 0; i < sizeof(stRxInitAttr.as8PNSwap)/sizeof(CVI_S8); i++) {
		stRxInitAttr.as8PNSwap[i] = pstSnsInfo->as8PNSwap[i];
	}

	return (pstSnsObj->pfnPatchRxAttr) ? pstSnsObj->pfnPatchRxAttr(&stRxInitAttr) : CVI_SUCCESS;
}

#if 0
CVI_S32 SAMPLE_COMM_ISP_GetRxAttr(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret, i;
	ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[i];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;

		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj.pfnGetRxAttr(ViPipe, &stDevAttr);
	}
}
#endif

CVI_S32 SAMPLE_COMM_ISP_SetSensorMode(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	WDR_MODE_E wdrMode;
	ISP_PUB_ATTR_S stPubAttr;
	ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc;
	ISP_CMOS_SENSOR_IMAGE_MODE_S stSnsrMode;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		wdrMode = pstViInfo->stDevInfo.enWDRMode;
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;

		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		if (SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr) != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "Can't get sns attr!\n");
			return s32Ret;
		}
		stSnsrMode.u16Width = stPubAttr.stSnsSize.u32Width;
		stSnsrMode.u16Height = stPubAttr.stSnsSize.u32Height;
		stSnsrMode.f32Fps = stPubAttr.f32FrameRate;
		printf("stSnsrMode.u16Width %d stSnsrMode.u16Height %d %f wdrMode %d pstSnsObj %p\n",
		       stSnsrMode.u16Width, stSnsrMode.u16Height, stSnsrMode.f32Fps, wdrMode, pstSnsObj);
		pstSnsObj->pfnExpSensorCb(&stSnsrSensorFunc);
		s32Ret = stSnsrSensorFunc.pfn_cmos_set_image_mode(ViPipe, &stSnsrMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set image mode failed!\n");
			return s32Ret;
		}

		s32Ret = stSnsrSensorFunc.pfn_cmos_set_wdr_mode(ViPipe, wdrMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set wdr mode failed!\n");
			return s32Ret;
		}
	}
	return s32Ret;
}

static SNS_BDG_MUX_MODE_E SAMPLE_COMM_ISP_GetSnsBdgMode(SAMPLE_SNS_TYPE_E enSnsType)
{
	VI_DEV_ATTR_S       stViDevAttr;
	SNS_BDG_MUX_MODE_E  MuxMode;

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	switch (stViDevAttr.enWorkMode) {
	case VI_WORK_MODE_1Multiplex:
		MuxMode = SNS_BDG_MUX_NONE;
		break;
	case VI_WORK_MODE_2Multiplex:
		MuxMode = SNS_BDG_MUX_2;
		break;
	case VI_WORK_MODE_3Multiplex:
		MuxMode = SNS_BDG_MUX_3;
		break;
	case VI_WORK_MODE_4Multiplex:
		MuxMode = SNS_BDG_MUX_4;
		break;
	default:
		MuxMode = SNS_BDG_MUX_NONE;
		break;
	}

	return MuxMode;
}

CVI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, CVI_U32 u32SnsId, CVI_S32 s32BusId,
						CVI_S32 s32I2cAddr)
{
	CVI_S32 s32Ret = -1;
	SAMPLE_SNS_TYPE_E enSnsType = g_enSnsType[u32SnsId];
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;
	const ISP_SNS_OBJ_S *pstSnsObj;
	ISP_INIT_ATTR_S *pstInitAttr = &gstInitAttr[u32SnsId];
	ISP_SNS_COMMBUS_U unSnsrBusInfo = {
		.s8I2cDev = 3,
	};

	#define SNSBUS_VLD(x)		(x >= 0)

	if (u32SnsId > MAX_SENSOR_NUM) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "invalid sensor id: %d\n", u32SnsId);
		return CVI_FAILURE;
	}

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (pstSnsObj == CVI_NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d not exist!\n", u32SnsId);
		return CVI_FAILURE;
	}

	pstInitAttr->enGainMode = SNS_GAIN_MODE_SHARE;
	if ((enSnsType == SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1)) {
		pstInitAttr->enL2SMode = SNS_L2S_MODE_FIX;
	}
	pstInitAttr->enSnsBdgMuxMode = SAMPLE_COMM_ISP_GetSnsBdgMode(enSnsType);
	s32Ret = pstSnsObj->pfnSetInit(u32SnsId, pstInitAttr);
	if (s32Ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetInit error id: %d s32Ret %d\n", IspDev, s32Ret);
		return CVI_FAILURE;
	}
	/* set i2c bus info */
	if (SNSBUS_VLD(s32BusId))
		unSnsrBusInfo.s8I2cDev = (CVI_S8)s32BusId;
	s32Ret = pstSnsObj->pfnSetBusInfo(u32SnsId, unSnsrBusInfo);
	if (s32Ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetBusInfo error id: %d s32Ret %d\n", IspDev, s32Ret);
		return CVI_FAILURE;
	}
	if (pstSnsObj->pfnPatchI2cAddr)
		pstSnsObj->pfnPatchI2cAddr(s32I2cAddr);

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	//  strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));

	if (pstSnsObj->pfnRegisterCallback != CVI_NULL) {
		s32Ret = pstSnsObj->pfnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor_register_callback failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	} else {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor_register_callback failed with CVI_NULL!\n");
		return CVI_FAILURE;
	}

	g_au32IspSnsId[IspDev] = u32SnsId;

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;
	CVI_U32 u32SnsId;
	const ISP_SNS_OBJ_S *pstSnsObj;
	CVI_S32 s32Ret = -1;

	u32SnsId = g_au32IspSnsId[IspDev];

	if (u32SnsId > MAX_SENSOR_NUM) {
		SAMPLE_PRT("%s: invalid sensor id: %d\n", __func__, u32SnsId);
		return CVI_FAILURE;
	}

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	//   strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));

	if (pstSnsObj->pfnUnRegisterCallback != CVI_NULL) {
		s32Ret = pstSnsObj->pfnUnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("sensor_unregister_callback failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	} else {
		SAMPLE_PRT("sensor_unregister_callback failed with CVI_NULL!\n");
	}

	return CVI_SUCCESS;
}
