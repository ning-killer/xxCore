#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "ts_common.h"
#include "sample_comm.h"
#include "loadbmp.h"

#include "sample_comm_asj_osd.h"
static char font_path[256] = {};
extern unsigned char *getArgbDataAfterHandleStr(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,  unsigned char* inputBitmap,const char *fontPath);

ovd_osd_t g_ovd_osd = {
	.chn = {
		[0] = {
			.status = ASJ_CHN_OSD_CHAOS,
			.have_inited = 0,
			.timestamp = {
				.id = 1,
				.on = 1,
				.osdpos = {.x = 0, .y = 0},
				.DateMode = 1,
				.TimeMode = 1,
				.refresh = 0,
				},
			.logo = {
				.id = 2,
				.on = 1,
				.osdpos = {.x = 0, .y = 120},
				},
			.text = {
				.osd_count = 8,
				.osd_text = {
					[0] = {
						.on = 1,
						.mode = 0,
						.id = 3,
						.text = "chanel name undefined",
						.osdfont = OSDFONT_BIG,
						.osdpos = {.x = 0, .y = 240},
						.color = {.red = 127, .green = 1, .blue = 127}
						},
                    }
				}
			},
		[1] = {
			.status = ASJ_CHN_OSD_CHAOS,
			.have_inited = 0,
			.timestamp = {
				.id = 4,
				.on = 1,
				.osdpos = {.x = 0, .y = 0},
				.DateMode = 0,
				.TimeMode = 0,
				.refresh = 0,
				},
			.logo = {
				.id = 5,
				.on = 1,
				.osdpos = {.x = 0, .y = 120},
				},
			.text = {
				.osd_count = 1,
				.osd_text = {
					[0] = {
						.on = 0,
						.mode = 0,
						.id = 6,
						.text = "the 2rd channel",
						.osdfont = OSDFONT_BIG,
						.osdpos = {.x = 0, .y = 240},
						.color = {.red = 127, .green = 127, .blue = 127}
						}
					}
				}
			},
        [2] = {
			.status = ASJ_CHN_OSD_CHAOS,
			.have_inited = 0,
			.timestamp = {
				.id = 7,
				.on = 1,
				.osdpos = {.x = 0, .y = 0},
				.DateMode = 1,
				.TimeMode = 1,
				.refresh = 0,
				},
            .logo = {
				.id = 8,
				.on = 1,
				.osdpos = {.x = 0, .y = 30},
				},
			.text = {
				.osd_count = 1,
				.osd_text = {
					[0] = {
						.on = 1,
						.mode = 0,
						.id = 9,
						.text = "the 3rd channel",
						.osdfont = OSDFONT_BIG,
						.osdpos = {.x = 0, .y = 60},
						.color = {.red = 127, .green = 127, .blue = 127}
						}
					}
				}
            }
        }
	};

//static ovd_osd_text_t g_ovd_osd_text = {
//	.osd_count = 1,
//	.osd_text = {
//		[0] = {
//			.on = 1,
//			.mode = 2,
//			.id = 1,//Caution, RGN_HANDLE 0 is for timestamp, RGN_HANDLE = id * mode
//			.text = "test channel name",
//			.osdfont = OSDFONT_BIG,
//			.osdpos = {.x = 0, .y = 0},
//			.color = {.red = 127, .green = 127, .blue = 127}
//			}
//		}
//	};

/* ------------------------------------------------------------------------------ */
static int chn2vpssdev(int chn)
{
	return chn / ASJ_VPSS_CHNNUM_PERGRP;
}

static int chn2vpsschn(int chn)
{
	return chn % ASJ_VPSS_CHNNUM_PERGRP;
}

static int threadCreate(pthread_t *pt_id,void * proc,void * arg)
{
    pthread_attr_t attr;
    int ret;

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    int stacksize = (4 << 10 ) << 10;
    pthread_attr_setstacksize(&attr, stacksize);

    ret = pthread_create(pt_id,&attr,(void *)proc,arg);
    if (ret != 0)
    {
        pthread_attr_destroy (&attr);
        printf("pthread_create error %s\n", strerror(ret));
        return -1;
    }
    pthread_attr_destroy (&attr);

    return 0;
}

ovd_osd_text_t *getOvdOsdTextInfo(int chn)
{
    return &g_ovd_osd.chn[chn].text;
}

/* --------------------------------------------------------------------------- */
static char *weekstr_map[7] = {
	"Sun ",
	"Mon ",
	"Tues",
	"Wed ",
	"Thur",
	"Fri ",
	"Sun ",
};

char **int2weekstr(int week_day)
{
	if (week_day < 0 || week_day >= 7) {
		return &weekstr_map[0];
	} else {
		return &weekstr_map[week_day];
	}
}

static int getCurrentTime(unsigned char *pSzGetTimeBuff, const unsigned char inLen, unsigned char *pOutLen, int datemode, int timemode)
{
    if (NULL == pSzGetTimeBuff) {
        SAMPLE_PRT(" Param Is Null !.");
        return -1;
    }
    time_t timep;
    struct tm *tm_now = TS_NULL;
    time (&timep);
    tm_now = localtime(&timep); /** local time */

	if (datemode == 1 && timemode == 1) {
	    snprintf((char *)pSzGetTimeBuff, inLen, "%04d-%02d-%02d %02d:%02d:%02d    %s",
	        tm_now->tm_year + 1900,
	        tm_now->tm_mon + 1,
	        tm_now->tm_mday,
	        tm_now->tm_hour,
	        tm_now->tm_min,
	        tm_now->tm_sec,
	        *int2weekstr(tm_now->tm_wday));
	} else if (datemode == 0 && timemode == 0) {
	    snprintf((char *)pSzGetTimeBuff, inLen, "%02d-%02d-%04d %02d:%02d:%02d %s %s",
	        tm_now->tm_mon + 1,
	        tm_now->tm_mday,
	        tm_now->tm_year + 1900,
	        tm_now->tm_hour % 12,
	        tm_now->tm_min,
	        tm_now->tm_sec,
	        tm_now->tm_hour / 12?"PM":"AM",
	        *int2weekstr(tm_now->tm_wday));
	} else if (datemode == 0 && timemode == 1) {
	    snprintf((char *)pSzGetTimeBuff, inLen, "%02d-%02d-%04d %02d:%02d:%02d    %s",
	        tm_now->tm_mon + 1,
	        tm_now->tm_mday,
	        tm_now->tm_year + 1900,
	        tm_now->tm_hour,
	        tm_now->tm_min,
	        tm_now->tm_sec,
	        *int2weekstr(tm_now->tm_wday));
	} else if (datemode == 1 && timemode == 0) {
		snprintf((char *)pSzGetTimeBuff, inLen, "%04d-%02d-%02d %02d:%02d:%02d %s %s",
			tm_now->tm_year + 1900,
			tm_now->tm_mon + 1,
			tm_now->tm_mday,
			tm_now->tm_hour % 12,
			tm_now->tm_min,
			tm_now->tm_sec,
			tm_now->tm_hour / 12?"PM":"AM",
			*int2weekstr(tm_now->tm_wday));
	} else {
        SAMPLE_PRT("Unknown datemode %d and timemode %d !\n", datemode, timemode);
	}

    *pOutLen = strlen((char *)pSzGetTimeBuff);
    return 0;
}

static void getPixelWordH(int u32PicWidth, float *pixels, int *one_word_h)
{
    if(2560 <= u32PicWidth ) {
        *pixels = 44.0;
        *one_word_h = 44;
    } else if(2304 <= u32PicWidth ) {
        *pixels = 40.0;
        *one_word_h = 40;
    } else if(1280 <= u32PicWidth ) {
        *pixels = 27.0;
        *one_word_h = 27;
    } else if(960 <= u32PicWidth ) {
        *pixels = 19.0;
        *one_word_h = 19;
    } else if(640 <= u32PicWidth ) {
        *pixels = 16.0;
        *one_word_h = 16;
    } else {
		SAMPLE_PRT("Video width %d is too small!\n", u32PicWidth);
	}
}

static int getSuitableFontSize(int videoW, int videoH, float *fontW, int *fontH)
{
    if(2560 == videoW ) {
        *fontW = 64.0;
        *fontH = 64;
    } else if(2304 == videoW ) {
        *fontW = 60.0;
        *fontH = 60;
    } else if(1280 == videoW ) {
        *fontW = 32.0;
        *fontH = 32;
    } else if(960 == videoW ) {
        *fontW = 24.0;
        *fontH = 24;
    } else if(640 == videoW ) {
        *fontW = 16.0;
        *fontH = 16;
    }
    return TS_SUCCESS;
}

int getTimeStringBitmapSize(float pixels, int bitmap_h, int *strW, int *strH)
{
    int width, height;
    unsigned char *pStream = TS_NULL;

    pStream = getArgbDataAfterHandleStr((unsigned char *)ASJ_TIMESTAMP_TEMPLATE, &width, &height, pixels, bitmap_h, TS_NULL,font_path);
    if(TS_NULL == pStream) {
        SAMPLE_PRT(" pBitMap null");
        return TS_FAILURE;
    }
    *strW = width;
    *strH = height;
    free(pStream);
    pStream = TS_NULL;
    return TS_SUCCESS;
}

int processStride16Bitmap(BITMAP_S *pBitMap)
{
    char *pStream = TS_NULL;
    char *newBitmap = TS_NULL;
    int width;
    int height;
    int gap = 0;
    if(!pBitMap) {
        SAMPLE_PRT(" pBitMap null");
        return TS_FAILURE;
    }
    width = pBitMap->u32Width;
    height = pBitMap->u32Height;
    pStream = pBitMap->pData;
    gap = 8 - width % 8;

	if(gap != 8 && pStream != TS_NULL) {
        newBitmap = malloc((width + gap) * height * 2);
		if(NULL == newBitmap){
			SAMPLE_PRT("alloc newBitmap is fail!\n");
			return TS_FAILURE;
		}
        memset(newBitmap, 0x0, (width + gap) * height * 2);
        for(int i = 0; i < height; i++) {
            memcpy(newBitmap + (width + gap) * i * 2, pStream + width * i * 2, width * 2);
        }
        pBitMap->pData = newBitmap;
        pBitMap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;
        pBitMap->u32Width = width + gap;
        pBitMap->u32Height = height;
        free(pStream);
        pStream = NULL;
    }
    return TS_SUCCESS;
}

/* ---------------------------------------------------------------------------- */

static int rgnSetTitles(unsigned char *pLoadInfo, TITLE_PARAM_CTX_S *param)
{
    BITMAP_S stBitMap;
    unsigned char *pStream = TS_NULL;
    //MPP_CHN_S stMppChn = {0};
    int s32Ret = TS_FAILURE;
    int width, height;
    float pixels = 0.0;
    int bitmap_h = 0;
    //int gap = 0;
    //char *newBitmap = TS_NULL;
	RGN_CANVAS_INFO_S stCanvasInfo = {0};
	RGN_HANDLE handle = 0;

	if (!param || !pLoadInfo) {
		SAMPLE_PRT("NULL !\n");
		return -1;
	}

	if (0 > param->channel || param->channel > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", param->channel);
		return -1;
	}
	
	if (!g_ovd_osd.chn[param->channel].timestamp.refresh) {
		//SAMPLE_PRT("Not refresh, skip update chn %d 's timestamp.\n", param->channel);
		return 0;
	}

	handle = g_ovd_osd.chn[param->channel].timestamp.id;

    memset(&stBitMap, 0, sizeof(BITMAP_S));

    getSuitableFontSize(param->width, param->height, &pixels, &bitmap_h);

	unsigned char** globalTimeBitmapBuf = &g_ovd_osd.chn[param->channel].globalTimeBitmapBuf;

	if(TS_NULL == *globalTimeBitmapBuf){
		*globalTimeBitmapBuf = malloc((sizeof(ASJ_TIMESTAMP_TEMPLATE)+1) * 32 * 64 * 3);//(sizeof(ASJ_TIMESTAMP_TEMPLATE)+1) * 32 * 64 * 3, 928*64*3
		if(TS_NULL == *globalTimeBitmapBuf){
			SAMPLE_PRT("malloc globalTimeBitmapBuf Error !.");
			return s32Ret;
		}
	}

	pthread_mutex_t *osd_mutex_lock = &g_ovd_osd.chn[param->channel].osd_mutex_lock;

	memset(*globalTimeBitmapBuf, 0, (sizeof(ASJ_TIMESTAMP_TEMPLATE)+1) * 32 * 64 * 3);

    pthread_mutex_lock(osd_mutex_lock);

    pStream = getArgbDataAfterHandleStr(pLoadInfo, &width, &height, pixels, bitmap_h, *globalTimeBitmapBuf,font_path);
    if (NULL == pStream) {
        SAMPLE_PRT(" PDT_MEDIA_RgnLoadIniInfo Error !.");
		pthread_mutex_unlock(osd_mutex_lock);
        return -1;
    }

    stBitMap.pData = pStream;
    stBitMap.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
    stBitMap.u32Width = width;
    stBitMap.u32Height = height;

    //processStride16Bitmap(&stBitMap);

#if  0
	//SAMPLE_PRT("TS_MPI_RGN_SetBitMap stBitMap.u32Width [%d], stBitMap.u32Height: %d", stBitMap.u32Width, stBitMap.u32Height);
    s32Ret = TS_MPI_RGN_SetBitMap(handle, &stBitMap);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_SetBitMap [%d] failed with %#x", handle, s32Ret);
    }
#else
	TS_MPI_RGN_GetCanvasInfo(handle, &stCanvasInfo);
    //SAMPLE_PRT("date string width [%d], height: %d, phy:%llx\n", width, height, stCanvasInfo.u64PhyAddr);
	memcpy((TS_VOID*)(TS_UL)(stCanvasInfo.u64VirtAddr), stBitMap.pData, stBitMap.u32Width * stBitMap.u32Height * 2);
	s32Ret = TS_MPI_RGN_UpdateCanvas(handle);
	//SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas \n");
#endif

    if(NULL == stBitMap.pData) {
        SAMPLE_PRT("param is free");
    } else {
       // free(stBitMap.pData);
       // stBitMap.pData = TS_NULL;
    }

    pthread_mutex_unlock(osd_mutex_lock);
    return s32Ret;
}

static void *rgnRefreshTime(void *args)
{
	if (!args){
		SAMPLE_PRT("args is null null!\n");
		return NULL;
	}
    prctl(PR_SET_NAME, (unsigned long)"rgnRefreshTime", 0, 0, 0);
    unsigned char length = 0;
    //RGN_HANDLE handle = 0;
    unsigned char szGetTimeInfo[128] = {0};
    TITLE_PARAM_CTX_S *p = (TITLE_PARAM_CTX_S *)args;
	int datemode = 1;
	int timemode = 1;

    SAMPLE_PRT("rgnRefreshTime height: %d, width: %d, chn: %d\n", p->height, p->width, p->channel);

    while(1) {
		if (0 > p->channel || p->channel > ASJ_CHN_NUM - 1) {
            usleep(1000 * 1000 * 3);
            continue;
		}

		datemode = g_ovd_osd.chn[p->channel].timestamp.DateMode;
		timemode = g_ovd_osd.chn[p->channel].timestamp.TimeMode;
        memset(szGetTimeInfo, 0, sizeof(szGetTimeInfo));
        if (0 != getCurrentTime(szGetTimeInfo, sizeof(szGetTimeInfo), &length, datemode, timemode)) {
            usleep(1000 * 1000 * 3);
            continue;
        }
		
        rgnSetTitles(szGetTimeInfo, p);
        usleep(990 * 1000);
    }
    return NULL;
}

TS_S32 APP_COMM_VENC_Open_OSD_TimeStamp(int chn, TS_BOOL bCreateThread,const char *fontPath)
{
    RGN_HANDLE handle = 0;
    //SIZE_S stPicSize;
    RGN_ATTR_S stRegion;
    MPP_CHN_S stMppChn;
    int s32Ret = -1;
    RGN_CHN_ATTR_S stChnAttr;
    float pixels = 32.0;
    int one_word_h = 0;
    int bitmap_w;
    int bitmap_h;
    //TS_S32 i;
	BITMAP_S stBitMap;
    unsigned char *pStream = TS_NULL;
	int u32CurVideoWidth = 0;
	int u32CurVideoHeight = 0;
    VENC_CHN_ATTR_S stVencChnAttr;

	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (!g_ovd_osd.chn[chn].timestamp.on) {
		SAMPLE_PRT("Not enable, skip create chn %d 's timestamp.\n", chn);
		return 0;
	}

    s32Ret = TS_MPI_VENC_GetChnAttr(chn, &stVencChnAttr);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_VENC_GetChnAttr failed! chn %d 0x%x", chn , s32Ret);
        return -1;
    }
    u32CurVideoWidth = stVencChnAttr.stVencAttr.u32PicWidth;
    u32CurVideoHeight  = stVencChnAttr.stVencAttr.u32PicHeight;

	handle = g_ovd_osd.chn[chn].timestamp.id;

    //printf("##u32CurVideoWidth=%d, u32CurVideoHeight=%d\n", u32CurVideoWidth, u32CurVideoHeight);
    getSuitableFontSize(u32CurVideoWidth, u32CurVideoHeight, &pixels, &one_word_h);
    //SAMPLE_PRT("##pixels=%f, one_word_h=%d\n", pixels, one_word_h);
    getTimeStringBitmapSize(pixels, one_word_h, &bitmap_w, &bitmap_h);
    SAMPLE_PRT("rgnSetTimeInit bitmap_w=%d, bitmap_h=%d\n", bitmap_w, bitmap_h);
    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
    stRegion.unAttr.stOverlay.stSize.u32Height = bitmap_h;
    stRegion.unAttr.stOverlay.stSize.u32Width  = (bitmap_w % 8) ? ((bitmap_w / 8 + 1) * 8) : bitmap_w;
    stRegion.unAttr.stOverlay.u32BgColor = 0x00ff00ff;
    stRegion.unAttr.stOverlay.u32CanvasNum = 2;
    SAMPLE_PRT("TS_MPI_RGN_Create u32Height=%d, u32Width=%d\n", stRegion.unAttr.stOverlay.stSize.u32Height,
        stRegion.unAttr.stOverlay.stSize.u32Width);
    s32Ret = TS_MPI_RGN_Create(handle, &stRegion);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
	//bitmap
    unsigned char length = 0;
    unsigned char szGetTimeInfo[128] = {0};
    int width, height;
	getCurrentTime(szGetTimeInfo, sizeof(szGetTimeInfo), &length, 1, 1);

    pStream = getArgbDataAfterHandleStr((unsigned char *)&szGetTimeInfo, &width, &height, pixels, bitmap_h, TS_NULL,fontPath);
    stBitMap.pData = pStream;
    stBitMap.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
    stBitMap.u32Width = width;
    stBitMap.u32Height = height;

    s32Ret = TS_MPI_RGN_SetBitMap(handle, &stBitMap);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_SetBitMap [%d] failed with %#x", handle, s32Ret);
        return TS_FAILURE;
    }

    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;
    memset(&stChnAttr, 0, sizeof(RGN_CHN_ATTR_S));
    stChnAttr.bShow = TS_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = g_ovd_osd.chn[chn].timestamp.osdpos.x;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = g_ovd_osd.chn[chn].timestamp.osdpos.y;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    if(g_ovd_osd.chn[chn].timestamp.osdhoralign == hleft){
        stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 0 + g_ovd_osd.chn[chn].timestamp.osdpos.x;
    }else if(g_ovd_osd.chn[chn].timestamp.osdhoralign == hright){
        stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = u32CurVideoWidth - stRegion.unAttr.stOverlay.stSize.u32Width - 10;
    }else{
        stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = u32CurVideoWidth / 2 - stRegion.unAttr.stOverlay.stSize.u32Width / 2 + g_ovd_osd.chn[chn].timestamp.osdpos.x;
        //stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = u32CurVideoHeight / 2 + 200;
    }
    s32Ret = TS_MPI_RGN_AttachToChn(handle, &stMppChn, &stChnAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

	if(pStream){
		free(pStream);
		pStream = TS_NULL;
	}

	TITLE_PARAM_CTX_S *stPatam = &g_ovd_osd.chn[chn].stPatam;
    stPatam->channel = chn;
    stPatam->width   = u32CurVideoWidth;
    stPatam->height  = u32CurVideoHeight;
    SAMPLE_PRT("osd timestamp height: %d, width: %d, chn: %d\n", stPatam->height, stPatam->width, stPatam->channel);

    if(bCreateThread) {
        pthread_t rgnTimethread;
        s32Ret = threadCreate(&rgnTimethread,  rgnRefreshTime, (void *)stPatam);
        if (0 != s32Ret) {
            SAMPLE_PRT("create rgnRefreshTime thread failed !.");
            return -1;
        }
    }

	g_ovd_osd.chn[chn].timestamp.refresh = TS_TRUE;

    return 0;
}

TS_S32 APP_COMM_VENC_Close_OSD_TimeStamp(int chn)
{
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (!g_ovd_osd.chn[chn].timestamp.on) {
		SAMPLE_PRT("skip chn %d !\n", chn);
		return 0;
	}

	g_ovd_osd.chn[chn].timestamp.refresh = TS_FALSE;

    MPP_CHN_S stMppChn;
    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;

    SAMPLE_REGION_Close_One_Rgn(g_ovd_osd.chn[chn].timestamp.id, OVERLAY_RGN, &stMppChn);
    return TS_SUCCESS;
}

int osdStartOneOverlayRGN(RGN_HANDLE handle, MPP_CHN_S *mpp_chn, int x, int y, BITMAP_S *pBitmap)
{
    RGN_ATTR_S stRegion;
    MPP_CHN_S stMppChn;
    int s32Ret = -1;
    RGN_CHN_ATTR_S stChnAttr;
    //SIZE_S stPicSize;
    memset(&stRegion, 0, sizeof(RGN_ATTR_S));
    SAMPLE_PRT("start handle %d, %d, %d, %d, %d, %d\n", handle, x, y, pBitmap->enPixelFormat, pBitmap->u32Width,
        pBitmap->u32Height);
    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.enPixelFmt = pBitmap->enPixelFormat;
    stRegion.unAttr.stOverlay.stSize.u32Height = pBitmap->u32Height;
    stRegion.unAttr.stOverlay.stSize.u32Width  = pBitmap->u32Width;
    stRegion.unAttr.stOverlay.u32BgColor = 0;//0x00ff00ff;
    stRegion.unAttr.stOverlay.u32CanvasNum = 1;//just need one
    //SAMPLE_PRT("TS_MPI_RGN_Create: %d, %d, %d\n", stRegion.unAttr.stOverlay.enPixelFmt, stRegion.unAttr.stOverlay.stSize.u32Width, stRegion.unAttr.stOverlay.stSize.u32Height);
    s32Ret = TS_MPI_RGN_Create(handle, &stRegion);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
    s32Ret = TS_MPI_RGN_SetBitMap(handle, pBitmap);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_SetBitMap [%d] failed with %#x", handle, s32Ret);
    }
	memcpy(&stMppChn, mpp_chn, sizeof(MPP_CHN_S));

    memset(&stChnAttr, 0, sizeof(RGN_CHN_ATTR_S));
    stChnAttr.bShow = TS_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = x;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = y;
    s32Ret = TS_MPI_RGN_AttachToChn(handle, &stMppChn, &stChnAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
   // SAMPLE_PRT("success with ret %#x!\n", s32Ret);
    return TS_SUCCESS;
}


TS_S32 SAMPLE_REGION_Close_One_Rgn(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn)
{
    //TS_S32         i;
    TS_S32         s32Ret;
    //TS_S32         MinHandle;
    SAMPLE_PRT("SAMPLE_REGION_Close_One_Rgn enter HandleNum=%d\n", HandleNum);
    s32Ret = SAMPLE_REGION_DetachFromChn(HandleNum, pstChn);
    if (TS_SUCCESS != s32Ret)
        SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", HandleNum);
    s32Ret = SAMPLE_REGION_Destroy(HandleNum);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("SAMPLE_REGION_Destroy failed!\n");
        return TS_FAILURE;
    }
    SAMPLE_PRT("SAMPLE_REGION_Close_One_Rgn success\n");
    return TS_SUCCESS;
}

int getOsdLogoPositionPhotoName(TS_CHAR *fileNname, int u32PicWidth, int u32PicHeight, int *posX,
    int *posY)
{
    int photoHeight = 0;
    if(2560 == u32PicWidth ) {
        strcpy(fileNname, "hjq_2560x1440.bmp");
        *posX = 20;
        *posY = 20;
        photoHeight = 68;
    } else if(2304 == u32PicWidth ) {
        strcpy(fileNname, "hjq_2304x1296.bmp");
        *posX = 20;
        *posY = 20;
        photoHeight = 60;
    } else if(1280 == u32PicWidth) {
        strcpy(fileNname, "hjq_1280x720.bmp");
        *posX = 20;
        *posY = 20;
        photoHeight = 34;
    } else if(960  == u32PicWidth ) {
        strcpy(fileNname, "hjq_960x540.bmp");
        *posX = 10;
        *posY = 10;
        photoHeight = 28;
    } else if(640 == u32PicWidth ) {
        strcpy(fileNname, "hjq_640x360.bmp");
        *posX = 10;
        *posY = 10;
        photoHeight = 18;
    }
    return photoHeight;
}

int osdCustomizeInitOneOsd(
	RGN_HANDLE hdl,
	MPP_CHN_S *mpp_chn,
	pthread_mutex_t *mutex,
	int chn_w, int chn_h,
	int posX, int posY,
	unsigned char *text)
{
    //int iRet;
    float pixels = 40.0;
    int width = 0 , height = 0, x = 0, y = 0;
    int u32PicWidth = 2560;
    //int u32PicHeight = 1440;
    int one_word_h = 0;
    RGN_HANDLE handle;
    BITMAP_S stBitmap;
    //SAMPLE_PRT("posX is %d,posY is %d\n", posX, posY);

    u32PicWidth  = chn_w;
    //u32PicHeight = chn_h;

	getPixelWordH(u32PicWidth, &pixels, &one_word_h);

	if(text != TS_NULL) {
		SAMPLE_PRT("osd_text is %s\n", text);
	} else {
		SAMPLE_PRT("osd_text  is null\n");
		return TS_FAILURE;
	}

    pthread_mutex_lock(mutex);
    unsigned char *bitmap = getArgbDataAfterHandleStr(text, &width, &height, pixels, one_word_h, TS_NULL, font_path);
    if(NULL == bitmap) {
        SAMPLE_PRT("stb_truetypeStart error");
		pthread_mutex_unlock(mutex);
        return -1;
    }
    stBitmap.pData = bitmap;
    stBitmap.u32Width = width;
    stBitmap.u32Height = height;
    stBitmap.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
    //SAMPLE_PRT("width is %d,height is %d,stBitmap.u32Width=%d, stBitmap.u32Height=%d\n",width,height, stBitmap.u32Width, stBitmap.u32Height);
    processStride16Bitmap(&stBitmap);
    //SAMPLE_PRT("width is %d,height is %d,stBitmap.u32Width=%d, stBitmap.u32Height=%d\n",width,height, stBitmap.u32Width, stBitmap.u32Height);
    //int bitmap_w_one = roundf(pixels);
    //SAMPLE_PRT("bitmap_w_one is %d\n",bitmap_w_one);
    y = posY;
	x = posX;
    handle = hdl;
    osdStartOneOverlayRGN(handle, mpp_chn, x, y, &stBitmap);
    if(stBitmap.pData)
        free(stBitmap.pData);
    pthread_mutex_unlock(mutex);

    return TS_SUCCESS;
}

int osdCustomizeDeInitOneOsd(RGN_HANDLE handle, MPP_CHN_S *mpp_chn)
{
    int iRet = 0;
    iRet = SAMPLE_REGION_Close_One_Rgn(handle, OVERLAY_RGN, mpp_chn);
    return iRet;
}

TS_S32 APP_COMM_VENC_Open_OSD_Text(int chn)
{
    //文案1~5
    //文案6左边s
    //TS_BOOL hasLeftOsdText = TS_FALSE;
    //TS_S32 leftOsdTextIndex = 0;
    //TS_BOOL needCombineTwoStr = TS_FALSE;
    float pixels = 0.0;
    int one_word_h = 0;
    //int left_osd_w;
    //int left_osd_h;
    //int last_right_osd_w;
    //int last_right_osd_h;
    //unsigned char *bitmap;
    int i;
    int u32PicWidth = 2304;
    int u32PicHeight = 1296;
    int directAddOsdNum = 0;
    int posX;
    int posY;
    //int posX_to_edge = 0;
    //int posY_to_edge = 0;
    //TS_CHAR fileNname[20] = {0};
	MPP_CHN_S stMppChn;


	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;

    ovd_osd_text_t *osd_text = getOvdOsdTextInfo(chn);
    //SAMPLE_PRT("osd_text->osd_count is %d\n", osd_text->osd_count);
    directAddOsdNum =   osd_text->osd_count;
    VENC_CHN_ATTR_S stVencChnAttr;
    int s32Ret = TS_MPI_VENC_GetChnAttr(chn, &stVencChnAttr);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_VENC_GetChnAttr failed! 0x%x", s32Ret);
        return -1;
    }
    u32PicWidth  = stVencChnAttr.stVencAttr.u32PicWidth;
    u32PicHeight = stVencChnAttr.stVencAttr.u32PicHeight;

	getPixelWordH(u32PicWidth, &pixels, &one_word_h);

    for(i = 0; i < directAddOsdNum; i++) {
        unsigned short u16Len = strlen(osd_text->osd_text[i].text);//sizeof(osd_text->osd_text[i].text);
        //SAMPLE_PRT("osd_text->osd_text[%d].on ========= %d\n", i, osd_text->osd_text[i].on);
        if (osd_text->osd_text[i].on && strlen(osd_text->osd_text[i].text) > 0) {
            posX = osd_text->osd_text[i].osdpos.x;
            posY = osd_text->osd_text[i].osdpos.y;
            //SAMPLE_PRT("osd_text->osd_text[%d].mode ========= %d\n",i,osd_text->osd_text[i].mode);
            //SAMPLE_PRT("###i: %d, osd_text->osd_count: %d, one_word_h=%d\n",i,osd_text->osd_count, one_word_h);
            //SAMPLE_PRT("u32CurVideoWidth: %d, u32PicHeight: %d\n",u32CurVideoWidth,u32PicHeight);
            //SAMPLE_PRT("posX: %d, posY: %d\n",posX,posY);
        //Calculate Horizon Positions
        if(g_ovd_osd.chn[chn].text.osdhoralign == hleft){
            posX = osd_text->osd_text[i].osdpos.x;
        }else if(g_ovd_osd.chn[chn].text.osdhoralign == hright){
            posX = u32PicWidth - u16Len * pixels / 2 - osd_text->osd_text[i].osdpos.x;
        }else{
            posX = u32PicWidth / 2 - u16Len * pixels / 4;//- ((directAddOsdNum-i)*pixels) / 2
        }
        //Calculate Vertical Positions
        if(g_ovd_osd.chn[chn].text.osdveralign == vtop){
            posY = osd_text->osd_text[i].osdpos.y;
        }else if(g_ovd_osd.chn[chn].text.osdveralign == vbottom){
            posY = u32PicHeight - one_word_h - osd_text->osd_text[i].osdpos.y;
        }else{
            posY = u32PicHeight / 2 - one_word_h / 2;//- ((directAddOsdNum-i)*pixels) / 2
        }

            //for(int i1=0; i1<2; i1++){
                osdCustomizeInitOneOsd(osd_text->osd_text[i].id,
		        &stMppChn,
            	&g_ovd_osd.chn[chn].osd_mutex_lock,
            	u32PicWidth, u32PicHeight,
            	posX, posY,
                (unsigned char *)osd_text->osd_text[i].text);
            //}
        }
    }

    return TS_SUCCESS;
}

TS_S32 APP_COMM_VENC_Stop_OSD_Text(int chn,int i)
{
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	MPP_CHN_S stMppChn;
    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;

    ovd_osd_text_t *osd_text = getOvdOsdTextInfo(chn);
    //for(int i = 0; i < osd_text->osd_count; i++) {
        SAMPLE_PRT("osd_text->osd_text[%d].on ========= %d", i, osd_text->osd_text[i].on);
        //if(strlen(osd_text->osd_text[i].text) > 0) { //osd_text->osd_text[i].on && 
            SAMPLE_PRT("osd_text->osd_text[%d].mode ========= %d", i, osd_text->osd_text[i].mode);
            if(9 >= i){
                osdCustomizeDeInitOneOsd(osd_text->osd_text[i].id, &stMppChn);//osd_text->osd_text[i].id + i + 3
            }else{
                osdCustomizeDeInitOneOsd(i+3, &stMppChn);//osd_text->osd_text[i].id + i + 3
            }
            
        //}
    //}
    return TS_SUCCESS;
}

TS_S32 APP_COMM_VENC_Open_OSD_Logo(int chn,const char *imagePath)
{
    RGN_HANDLE handle = 11;
    TS_CHAR  fileNname[20] = {0};
    TS_CHAR  picFilePath[50] = {0};
    int posX = 0;
    int posY = 0;
    int startX = 0;
    int startY = 0;
    int u32PicWidth = 2560;
    int u32PicHeight = 1440;
    VENC_CHN_ATTR_S stVencChnAttr;
    BITMAP_S stBitmap;
    unsigned short *ptr16 = TS_NULL;
	MPP_CHN_S stMppChn;

	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (!g_ovd_osd.chn[chn].logo.on) {
		SAMPLE_PRT("Not enable, skip create chn %d 's logo.\n", chn);
		return 0;
	}

    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;

    int s32Ret = TS_MPI_VENC_GetChnAttr(chn, &stVencChnAttr);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_MPI_VENC_GetChnAttr failed! 0x%x", s32Ret);
        return -1;
    }
    u32PicWidth  = stVencChnAttr.stVencAttr.u32PicWidth;
    u32PicHeight = stVencChnAttr.stVencAttr.u32PicHeight;

    //getOsdLogoPositionPhotoName(fileNname, u32PicWidth, u32PicHeight, &posX, &posY);
    //sprintf((TS_CHAR *)picFilePath, "/tmp/%s", fileNname);
    if(NULL != imagePath){
        REGION_MST_LoadBmp(imagePath, &stBitmap, TS_FALSE, 0, PIXEL_FORMAT_ARGB_1555);
    }else{
        getOsdLogoPositionPhotoName(fileNname, u32PicWidth, u32PicHeight, &posX, &posY);
        sprintf((TS_CHAR *)picFilePath, "/tmp/%s", fileNname);
    }
    ptr16 = stBitmap.pData;
    for(int i = 0; i < stBitmap.u32Width * stBitmap.u32Height; i++) {
        if(ptr16[i] && ((64 == stBitmap.u32Width && 18 == stBitmap.u32Height) || (96 == stBitmap.u32Width && 28 == stBitmap.u32Height)))
            ptr16[i] = 0xFFFF;
        else if(ptr16[i])
            ptr16[i] = 0x8000 | ptr16[i];
    }

	handle = g_ovd_osd.chn[chn].logo.id;
    startX = g_ovd_osd.chn[chn].logo.osdpos.x;
    startY = g_ovd_osd.chn[chn].logo.osdpos.y;
    /*Make logo align*/
    if(g_ovd_osd.chn[chn].logo.osdhoralign == hleft){
        startX = 0 + g_ovd_osd.chn[chn].logo.osdpos.x;
    }else if(g_ovd_osd.chn[chn].logo.osdhoralign == hright){
        startX = u32PicWidth - stBitmap.u32Width - 10;
    }else{
        startX = u32PicWidth / 2  - stBitmap.u32Width / 2 + g_ovd_osd.chn[chn].logo.osdpos.x;
    }
    osdStartOneOverlayRGN(handle, &stMppChn, startX, startY, &stBitmap);
    return TS_SUCCESS;
}

TS_S32 APP_COMM_VENC_Close_OSD_Logo(int chn)
{
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (!g_ovd_osd.chn[chn].logo.on) {
		SAMPLE_PRT("skip chn %d !\n", chn);
		return 0;
	}

    MPP_CHN_S stMppChn;
    memset(&stMppChn, 0, sizeof(MPP_CHN_S));
    stMppChn.s32DevId = chn2vpssdev(chn);
    stMppChn.s32ChnId = chn2vpsschn(chn);
    stMppChn.enModId = TS_ID_VPSS;

    SAMPLE_REGION_Close_One_Rgn(g_ovd_osd.chn[chn].logo.id, OVERLAY_RGN, &stMppChn);
    return TS_SUCCESS;
}

TS_S32 osdRestartInit(int chn, const char *imagepath, const char *fontpath)
{
    int s32Ret = 0;

	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	s32Ret = APP_COMM_VENC_Open_OSD_TimeStamp(chn, TS_FALSE, fontpath);
    if (0 != s32Ret) {
		SAMPLE_PRT("rgnSetTimeInit chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    s32Ret = APP_COMM_VENC_Open_OSD_Logo(chn,imagepath);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Open_OSD_Logo chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    s32Ret = APP_COMM_VENC_Open_OSD_Text(chn);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Start_OSD_Text chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    return 0;
}

TS_S32 osdDeinit(int chn)
{
    int s32Ret = 0;

	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

    ovd_osd_text_t *osd_text = getOvdOsdTextInfo(chn);
    for(int i = 0; i < osd_text->osd_count; i++){
        s32Ret = APP_COMM_VENC_Stop_OSD_Text(chn,i);
        if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Stop_OSD_Text chn %d failed %d !\n", chn, s32Ret);
		return -1;
	    }
    }

    s32Ret = APP_COMM_VENC_Close_OSD_Logo(chn);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Close_OSD_Logo chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    s32Ret = APP_COMM_VENC_Close_OSD_TimeStamp(chn);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Close_Time_Osd chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    return 0;
}


TS_S32 osdInit(int chn,const char *imagePath,const char *fontpath)
{
    int s32Ret = 0;
    snprintf(font_path, sizeof(font_path), "%s", fontpath);
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	s32Ret = pthread_mutex_init(&g_ovd_osd.chn[chn].osd_mutex_lock, NULL);
	if (s32Ret != 0) {
		SAMPLE_PRT("g_osd_mutex_lock init failed chn %d %d\n", chn, s32Ret);
		return -1;
	}
	
	s32Ret = APP_COMM_VENC_Open_OSD_TimeStamp(chn, TS_TRUE,fontpath);
    if (0 != s32Ret) {
		SAMPLE_PRT("rgnSetTimeInit chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    s32Ret = APP_COMM_VENC_Open_OSD_Logo(chn,imagePath);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Open_OSD_Logo chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    s32Ret = APP_COMM_VENC_Open_OSD_Text(chn);
    if (0 != s32Ret) {
		SAMPLE_PRT("APP_COMM_VENC_Start_OSD_Text chn %d failed %d !\n", chn, s32Ret);
		return -1;
	}

    return 0;
}
