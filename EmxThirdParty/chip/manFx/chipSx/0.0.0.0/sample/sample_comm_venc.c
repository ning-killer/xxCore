#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "sample_comm.h"

#include "RtspServerApp.h"

#ifdef FAST_SWITCH_RESOLUTION
#include <semaphore.h>
sem_t g_switch_sem;
#endif

TS_BOOL g_getstream_pause = TS_FALSE;

//#define RTSP_DEBUG_ON_JCY
//#define OPEN_RTSP_DEBUG_LOG

#define RTSP_LIB_LIVE555
#ifndef RTSP_LIB_LIVE555
#define RTSP_LIB_FROM_GB
#endif

#ifdef RTSP_DEBUG_ON_JCY
#include "proconVideo.h"
#include "tm_rtspSever.h"
#endif

const TS_U8 g_SOI[2] = {0xFF, 0xD8};
const TS_U8 g_EOI[2] = {0xFF, 0xD9};
static pthread_t gs_VencPid;
static pthread_t gs_VencIDRPid;
static pthread_t gs_VencQpmapPid;
static pthread_t gs_VencSndFrmPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stIDRPara;
static SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S stQpMapSendFramePara;
static SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S stSendFramePara;

static TS_S32 gs_s32SnapCnt = 0;
TS_CHAR* DstBuf = NULL;
#define TEMP_BUF_LEN 8
#define MAX_THM_SIZE (64*1024)

static char g_venc_save_path[128] = {0};

#if 0//not used
static int get_time_ms(char *buff, int len)
{
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    if(buff == NULL)
    {
        printf("%s buff is NULL.\n", __func__);
        return -1;
    }

    gettimeofday(&tv, NULL);

    ptm = localtime (&(tv.tv_sec));

    strftime (time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);  //输出格式为: 2022-03-30 20:38:37
    milliseconds = tv.tv_usec / 1000;
    snprintf (buff, len, "%s.%03ld", time_string, milliseconds);            //输出格式为: 2022-03-30 20:38:37.182
    return 0;
}
#endif

#ifdef RTSP_DEBUG_ON_JCY

ulong wrFd = 0;

 void openTSingRTSPServer(PAYLOAD_TYPE_E  enType)
{

    SAMPLE_PRT("openTSingRTSPServer start!!!\n");

#ifdef RTSP_LIB_LIVE555

    /** 开启一个通道的循环缓冲区，最多30个结点，注意不能多写多读，只能一写多读 */
    ProconVideoInit(1, 30);

    tm_rtsp_server_start();

    wrFd = ProconVideoOpen(0, OPEN_WRONLY);
    SAMPLE_PRT("ProconVideoOpen wrFd %x!\n", wrFd);
#endif

#ifdef RTSP_LIB_FROM_GB
	RTSP_STREAM_TYPE_E streamType = RTSP_STREAM_TYPE_H265;
    int ret;
	if(PT_H264 == enType){
		streamType = RTSP_STREAM_TYPE_H264;
	}
	else if(PT_H265 == enType){
		streamType = RTSP_STREAM_TYPE_H265;
	}
	else{
		SAMPLE_PRT("invalid payload type %d\n", enType);
	}

	ret = gb_rtsp_server_start(streamType);

    SAMPLE_PRT("openTSingRTSPServer finish! streamType=%d, ret=%d\n",
		streamType, ret);
#endif

}
/**
 * @brief PDT_SAMPLE_VencPackageStream
 * @param pstVencStream
 * @param i
 * @return
 */
TS_S32 TS_SAMPLE_VencPackageStream(VENC_STREAM_S *pstVencStream, TS_S32 i, ulong wrFd)
{
#ifdef OPEN_RTSP_DEBUG_LOG
    printf("pstVencStream:%p, i: %d wrFd：%d\n", pstVencStream, i, wrFd);
#endif

    if (NULL == pstVencStream) {
        printf(" %s %d Param Is Null !.. \n", __func__, __LINE__);
        return TS_FAILURE;
    }

    if (0 != i) {
        return TS_SUCCESS;
    }

#ifdef RTSP_LIB_FROM_GB
    unsigned char *buffer;
    unsigned int size;
    unsigned int type;
    unsigned long long timestamp = 0;

    buffer = pstVencStream->pstPack[0].pu8Addr;
    size = pstVencStream->pstPack[0].u32Len;
    type = 1;
   // printf("pstVencStream->u32PackCount=%d\n", pstVencStream->u32PackCount);
    sendToRtspVencFrame(buffer, size, type, timestamp);
#endif


#ifdef RTSP_LIB_LIVE555

    int ii = 0;
    int len = 0;
    int index = 0;
    DATA_PIECE_T proDataInfo;
    STREAM_HEAD_T vFrameHead;
    unsigned long frameLen = 0;
    proDataInfo.count = index + 1;
    proDataInfo.buf[index] = (char *)&vFrameHead;
    proDataInfo.len[index] = sizeof(vFrameHead);
    index++;

    proDataInfo.nalInfo.nalNum = pstVencStream->u32PackCount;
    proDataInfo.nalInfo.nalStartOff = sizeof(vFrameHead);

#ifndef HIDE_PRINT_0919
    printf("=========TS_MPI_VENC_GetStream cnt=%d, type=%d\n",pstVencStream->u32PackCount,pstVencStream->pstPack->DataType);
#endif

    for (ii = 0; ii < pstVencStream->u32PackCount; ++ii) {
        proDataInfo.nalInfo.nalSize[ii] = pstVencStream->pstPack[ii].u32Len;

#ifndef HIDE_PRINT_0919
    printf("===================pstVencStream->pstPack[ii].u32Len:%d\n",pstVencStream->pstPack[ii].u32Len);
#endif

        len = pstVencStream->pstPack[ii].u32Len;
        if(len > 1000){
            proDataInfo.count = 2;
            proDataInfo.buf[index] = (char *)pstVencStream->pstPack[ii].pu8Addr;
            proDataInfo.len[index] = len;
            frameLen += len;
            index++;
            proDataInfo.nalInfo.nalSize[0] = len;
            proDataInfo.nalInfo.nalNum = 1;

#ifdef OPEN_RTSP_DEBUG_LOG
            printf("insert I frame header!\n");
            printf("1pu8Addr[0]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[0]);
            printf("1pu8Addr[1]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[1]);
            printf("1pu8Addr[2]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[2]);
            printf("1pu8Addr[3]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[3]);
            printf("1pu8Addr[4]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[4]);

            printf("1pu8Addr[37]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[37]);
            printf("1pu8Addr[38]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[38]);
            printf("1pu8Addr[39]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[39]);
            printf("1pu8Addr[40]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[40]);
            printf("1pu8Addr[41]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[41]);

            printf("1pu8Addr[73]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[73]);
            printf("1pu8Addr[74]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[74]);
            printf("1pu8Addr[75]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[75]);
            printf("1pu8Addr[76]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[76]);
            printf("1pu8Addr[77]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[77]);


            printf("1pu8Addr[84]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[84]);
            printf("1pu8Addr[85]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[85]);
            printf("1pu8Addr[86]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[86]);
            printf("1pu8Addr[87]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[87]);
            printf("1pu8Addr[88]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[88]);
#endif

        }else if (len > 0) {
            proDataInfo.count = 2;
            proDataInfo.buf[index] = (char *)pstVencStream->pstPack[ii].pu8Addr;
            proDataInfo.len[index] = len;
            frameLen += len;
            proDataInfo.nalInfo.nalNum = 1;
            index++;

#ifdef OPEN_RTSP_DEBUG_LOG
            printf("2pu8Addr[0]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[0]);
            printf("2pu8Addr[1]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[1]);
            printf("2pu8Addr[2]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[2]);
            printf("2pu8Addr[3]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[3]);
            printf("2pu8Addr[4]=0x%x\n", pstVencStream->pstPack[ii].pu8Addr[4]);
#endif
        }

        if (index >= MAX_NAL_IN_ONE_FRAME) {
            break;
        }

    }

    vFrameHead.packHead.packType = PACK_TYPE_VIDEO;
    vFrameHead.packHead.frameHeadLen = sizeof(vFrameHead.frameHead);

    if(pstVencStream->u32PackCount > 1) {//????1 ??
        vFrameHead.frameHead.frameType =1;
    }
    else {
        vFrameHead.frameHead.frameType = 0;
    }

    vFrameHead.frameHead.frameLen = frameLen;

    int ret = 0;

#ifdef OPEN_RTSP_DEBUG_LOG
    printf("proDataInfo.count = %d\n", proDataInfo.count);
    printf("proDataInfo.len[0] = %d\n", proDataInfo.len[0]);
    printf("proDataInfo.len[1] = %d\n", proDataInfo.len[1]);
    printf("proDataInfo.nalInfo.nalNum = %d\n", proDataInfo.nalInfo.nalNum);
    printf("proDataInfo.nalInfo.nalSize[0] = %d\n", proDataInfo.nalInfo.nalSize[0]);
    printf("proDataInfo.nalInfo.nalSize[1] = %d\n", proDataInfo.nalInfo.nalSize[1]);
    printf("proDataInfo.nalInfo.nalStartOff = %d\n", proDataInfo.nalInfo.nalStartOff);
#endif

    ret = ProconVideoWrite(wrFd, proDataInfo);
#ifndef HIDE_PRINT_0919
    printf("**##########ProconVideoWrite ret:%d\n",ret);
#endif

#endif

    return TS_SUCCESS;
}
#endif


#ifdef __READ_ALL_FILE__
static TS_S32 FileTrans_GetThmFromJpg(TS_CHAR* JPGPath, TS_U32* DstSize)
{
    TS_S32 s32RtnVal = 0;
    FILE* fpJpg = NULL;
    TS_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    TS_S32 bufpos = 0;
    TS_CHAR startflag[2] = {0xff, 0xd8};
    TS_S32 startpos = 0;
    TS_CHAR endflag[2] = {0xff, 0xd9};
    TS_S32 endpos = 0;
    fpJpg = fopen(JPGPath, "rb");
    TS_CHAR* pszFile = NULL;
    TS_S32 fd = 0;
    TS_S32 s32I = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));
    if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return TS_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);
        pszFile = (TS_CHAR*)malloc(stStat.st_size);
        if ((NULL == pszFile) || (stStat.st_size < 6))
        {
            fclose(fpJpg);
            printf("memory malloc fail!\n");
            return TS_FAILURE;
        }

        if (fread(pszFile, stStat.st_size , 1, fpJpg) <= 0)
        {
            fclose(fpJpg);
            free(pszFile);
            printf("fread jpeg src fail!\n");
            return TS_FAILURE;
        }

        fclose(fpJpg);
        TS_U16 u16THMLen = 0;
        u16THMLen = (pszFile[4] << 8) + pszFile[5];
        while (s32I < stStat.st_size)
        {
            tempbuf[bufpos] = pszFile[s32I++];
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = s32I - 2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    if (u16THMLen == s32I)
                    {
                        endpos = s32I;
                        break;
                    }
                    else
                    {
                        endpos = s32I;
                        break;
                    }
                }
            }
            bufpos++;
            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }

            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
                bufpos = 0;
            }

        }

    }
    if (endpos - startpos <= 0)
    {
        free(pszFile);
        printf("get .thm 11 fail!\n");
        return TS_FAILURE;
    }

    if (endpos - startpos >= stStat.st_size)
    {
        free(pszFile);
        printf("NO DCF info, get .thm 22 fail!\n");
        return TS_FAILURE;
    }

    TS_CHAR* temp = pszFile + startpos;
    if(MAX_THM_SIZE < (endpos - startpos))
    {
        printf("Thm is too large than MAX_THM_SIZE, get .thm 33 fail!\n");
        return TS_FAILURE;
    }

    TS_CHAR* cDstBuf = (TS_CHAR*)malloc(endpos - startpos);
    if (NULL == cDstBuf)
    {
        printf("memory malloc fail!\n");
        return TS_FAILURE;
    }

    memcpy(cDstBuf, temp, endpos - startpos);

    DstBuf = cDstBuf;
    *DstSize = endpos - startpos;
    free(pszFile);

    return TS_SUCCESS;
}

#else
static TS_S32 FileTrans_GetThmFromJpg(TS_CHAR* JPGPath, TS_U32* DstSize)
{
    TS_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    TS_S32 bufpos = 0;
    TS_CHAR startflag[2] = {0xff, 0xd8};
    TS_S32 startpos = 0;
    TS_CHAR endflag[2] = {0xff, 0xd9};
    TS_S32 endpos = 0;
    TS_BOOL bStartMatch = TS_FALSE;

    TS_S32 fd = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));

    FILE* fpJpg = NULL;
    fpJpg = fopen(JPGPath, "rb");
    if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return TS_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);

        while (!feof(fpJpg))
        {
            tempbuf[bufpos]=getc(fpJpg);
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = ftell(fpJpg)-2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
                    bStartMatch = TS_TRUE;
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    endpos = ftell(fpJpg);
                    if(TS_TRUE == bStartMatch)
                    {
                        break;
                    }
                }
            }
            bufpos++;

            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }
            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos -1] == 0xFF)
                {
                    tempbuf[0] = 0xFF;
                    bufpos = 1;
                }
                else
                {
                    bufpos = 0;
                }
            }
        }
    }

    if (endpos - startpos <= 0)
    {
        printf("get .thm 11 fail!\n");
        fclose(fpJpg);
        return TS_FAILURE;
    }

    if (endpos - startpos > MAX_THM_SIZE)
    {
        printf("Thm is too large than MAX_THM_SIZE, get .thm 22 fail!\n");
        fclose(fpJpg);
        return TS_FAILURE;
    }

    if (endpos - startpos >= stStat.st_size)
    {
        printf("NO DCF info, get .thm 33 fail!\n");
        fclose(fpJpg);
        return TS_FAILURE;
    }

    TS_CHAR* cDstBuf = (TS_CHAR*)malloc(endpos - startpos);
    if (NULL == cDstBuf)
    {
        printf("memory malloc fail!\n");
        fclose(fpJpg);
        return TS_FAILURE;
    }

    fseek(fpJpg, (long)startpos, SEEK_SET);
    *DstSize = fread(cDstBuf,1,endpos-startpos,fpJpg);
    if(*DstSize != (endpos - startpos))
    {
        free(cDstBuf);
        printf("fread fail!\n");
        fclose(fpJpg);
        return TS_FAILURE;
    }

    DstBuf = cDstBuf;
    fclose(fpJpg);

    return TS_SUCCESS;
}
#endif

VENC_CHN_ATTR_S VENC_CHN_ATTR_H265_720P =
{
	{PT_H265,1280,720,1280*720*3/2,2,TS_TRUE,1280,720,
		{{PIXEL_FORMAT_NV_12}},//TODO(jcy): temporary borrowing
	},
	{VENC_RC_MODE_H265CBR,
		{{30,1,30,30,500}},
	},
	{VENC_GOPMODE_NORMALP,
		{{1}},
	}
};

VENC_CHN_ATTR_S VENC_CHN_ATTR_H265_1080P =
{
	{PT_H265,1920,1080,1920*1080*3/2,2,TS_TRUE,1920,1080,
		{{PIXEL_FORMAT_NV_12}},//TODO(jcy): temporary borrowing
	},
	{VENC_RC_MODE_H265CBR,
		{{30,1,30,30,1600}},
	},
	{VENC_GOPMODE_NORMALP,
		{{1}},
	}
};

VENC_CHN_ATTR_S VENC_CHN_ATTR_H265_4K =
{
	{PT_H265,3840,2160,3840*2160*3/2,2,TS_TRUE,3840,2160,
		{{PIXEL_FORMAT_NV_12}},//TODO(jcy): temporary borrowing
	},
	{VENC_RC_MODE_H265CBR,
		{{20,1,20,20,5500}},
	},
	{VENC_GOPMODE_NORMALP,
		{{1}},
	}
};

VENC_CHN_ATTR_S VENC_CHN_ATTR_H265_4MP =
{
	{PT_H265,2560,1440,2560*1440*3/2,2,TS_TRUE,2560,1440,
		{{PIXEL_FORMAT_NV_12}},//TODO(jcy): temporary borrowing
	},
	{VENC_RC_MODE_H265CBR,
		{{25,1,15,15,1600}},
	},
	{VENC_GOPMODE_NORMALP,
		{{1}},
	}
};

VENC_CHN_ATTR_S VENC_CHN_ATTR_H265_5MP =
{
	{PT_H265,2688,1856,2688*1856*3/2,2,TS_TRUE,2688,1856,
		{{PIXEL_FORMAT_NV_12}},//TODO(jcy): temporary borrowing
	},
	{VENC_RC_MODE_H265CBR,
		{{30,1,30,30,2500}},
	},
	{VENC_GOPMODE_NORMALP,
		{{1}},
	}
};


TS_S32 SAMPLE_COMM_VENC_GetChnAttrByTpye(PAYLOAD_TYPE_E   enType,   VENC_CHN_ATTR_S *pstVencChnAttr)
{
    switch (enType)
    {
        case PT_H265:
		case PT_H264:
		case PT_JPEG:
			if (SENSOR0_TYPE == EVB_SENSOR)
				memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_1080P, sizeof(VENC_CHN_ATTR_S));
			else if (SENSOR0_TYPE == EVB_SENSOR_4K)
				memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_4K, sizeof(VENC_CHN_ATTR_S));
			else if (SENSOR0_TYPE == EVB_SENSOR_4MP)
				memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_4MP, sizeof(VENC_CHN_ATTR_S));
			else if (SENSOR0_TYPE == EVB_SENSOR_5MP)
				memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_5MP, sizeof(VENC_CHN_ATTR_S));
			else
				memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_720P, sizeof(VENC_CHN_ATTR_S));

            break;

        default:
            memcpy(pstVencChnAttr, &VENC_CHN_ATTR_H265_720P, sizeof(VENC_CHN_ATTR_S));
    }

	//TODO: JPEG
	if (PT_JPEG == enType) {
		pstVencChnAttr->stVencAttr.enType = PT_JPEG;
	}

    return TS_SUCCESS;
}

SAMPLE_RC_E SAMPLE_COMM_VENC_GetRcMode(void)
{
    char c;
    SAMPLE_RC_E  enRcMode = 0;

Begin_Get:

    printf("please choose rc mode:\n");
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
    printf("\t a) avbr. (Currently not supported)\n");
    printf("\t x) cvbr.\n");
    printf("\t q) qvbr. (Currently not supported)\n");
    printf("\t f) fixQp\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
        case 'a':
            enRcMode = SAMPLE_RC_AVBR;
            break;
        case 'q':
            enRcMode = SAMPLE_RC_QVBR;
            break;
        case 'x':
            enRcMode = SAMPLE_RC_CVBR;
            break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            SAMPLE_PRT("input rcmode: %c, is invaild!\n",c);
            goto Begin_Get;
    }
    return enRcMode;
}

VENC_GOP_MODE_E SAMPLE_COMM_VENC_GetGopMode(void)
{
    char c;
    VENC_GOP_MODE_E enGopMode = 0;

Begin_Get:

    printf("please choose gop mode:\n");
    printf("\t 0) NORMALP.\n");
    printf("\t 1) DUALP. (Currently not supported)\n");
    printf("\t 2) SMARTP. (Currently not supported)\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case '0':
            enGopMode = VENC_GOPMODE_NORMALP;
            break;
        case '1':
            enGopMode = VENC_GOPMODE_DUALP;
            break;
        case '2':
            enGopMode = VENC_GOPMODE_SMARTP;
            break;
        default:
            SAMPLE_PRT("input rcmode: %c, is invaild!\n",c);
            goto Begin_Get;
    }

    return enGopMode;
}

PAYLOAD_TYPE_E SAMPLE_COMM_VENC_GetPayLoadType(void)
{
    char c;
    PAYLOAD_TYPE_E enType = PT_H264;

Begin_Get:

    printf("please choose payload type:\n");
    printf("\t 0) H264. (Currently not supported)\n");
    printf("\t 1) H265.\n");
    printf("\t 2) MJPEG. (Currently not supported)\n");
    printf("\t 3) JPEG.\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case '0':
            enType = PT_H264;
            break;
        case '1':
            enType = PT_H265;
            break;
        case '2':
            enType = PT_MJPEG;
            break;
        case '3':
            enType = PT_JPEG;
            break;
        default:
            SAMPLE_PRT("input rcmode: %c, is invaild!\n",c);
            goto Begin_Get;
    }

    return enType;
}

PIXEL_FORMAT_E SAMPLE_COMM_VENC_Get_PixelFormat_fromKeyboard(enum SAMPLE_DETAIL detail)
{
    char c;
    PIXEL_FORMAT_E  pixel_format = 0;

Begin_Get:

	if (FULL_DETAIL == detail) {
		printf("please choose pixel format of the sensor:\n");
		printf("\t 0) I420.\n");
		printf("\t 1) YV12.\n");
		printf("\t 2) NV12.\n");
		printf("\t 3) NV21.\n");
	} else {
		printf("please choose pixel format of the yuv file:\n");
		printf("\t 0) (Invalid)\n");
		printf("\t 1) (Invalid)\n");
		printf("\t 2) YV12.\n");
		printf("\t 3) I420.\n");
	}

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case '0':
            pixel_format = PIXEL_FORMAT_YUV_PLANAR_420;
            break;
        case '1':
            pixel_format = PIXEL_FORMAT_YVU_PLANAR_420;
            break;
#if defined(TX5215) || defined(TX5239DV200)
		case '2':
			pixel_format = PIXEL_FORMAT_NV_21;
			break;
		case '3':
			pixel_format = PIXEL_FORMAT_NV_12;
			break;
#else
        case '2':
            pixel_format = PIXEL_FORMAT_NV_12;
            break;
        case '3':
            pixel_format = PIXEL_FORMAT_NV_21;
			break;
#endif
        default:
            SAMPLE_PRT("input pixel format: %c, but it's invaild!\n",c);
            goto Begin_Get;
    }
    return pixel_format;
}

int SAMPLE_COMM_VENC_Get_int_fromKeyboard(char * pTitle)
{
    int value;

    printf("please input %s :\n", pTitle);
	scanf("%d", &value);

    return value;
}

VENC_INTRA_REFRESH_MODE_E SAMPLE_COMM_VENC_GetIntraRefreshMode(void)
{
    char c;
    VENC_INTRA_REFRESH_MODE_E   enIntraRefreshMode = INTRA_REFRESH_ROW;

Begin_Get:

    printf("please input choose IntraRefresh mode!\n");
    printf("\t r) ROW.\n");
    printf("\t c) COLUMN.\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case 'r':
            enIntraRefreshMode = INTRA_REFRESH_ROW;
            break;
        case 'c':
            enIntraRefreshMode = INTRA_REFRESH_COLUMN;
            break;

        default:
            SAMPLE_PRT("input IntraRefresh Mode: %c, is invaild!\n",c);
            goto Begin_Get;
    }
    return enIntraRefreshMode;
}


/******************************************************************************
* function : Set venc memory location
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_MemConfig(TS_VOID)
{
    TS_S32 i = 0;
    //TS_S32 s32Ret;
    //TS_CHAR* pcMmzName;
    //MPP_CHN_S stMppChnVENC;
    /* group, venc max chn is 64*/
    for (i = 0; i < 64; i++)
    {
        //stMppChnVENC.enModId = TS_ID_VENC;
        //stMppChnVENC.s32DevId = 0;
        //stMppChnVENC.s32ChnId = i;
        //pcMmzName = NULL;
		#if 0
        /*venc*/
        s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChnVENC, pcMmzName);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_SYS_SetMemConfig with %#x!\n", s32Ret);
            return TS_FAILURE;
        }
		#endif
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr)
{
    switch(enGopMode)
    {
        case VENC_GOPMODE_NORMALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_NORMALP;
            pstGopAttr->stNormalP.s32IPQpDelta = 2;
        break;
        case VENC_GOPMODE_SMARTP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_SMARTP;
            pstGopAttr->stSmartP.s32BgQpDelta  = 4;
            pstGopAttr->stSmartP.s32ViQpDelta  = 2;
            pstGopAttr->stSmartP.u32BgInterval =  90;
        break;

        case VENC_GOPMODE_DUALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_DUALP;
            pstGopAttr->stDualP.s32IPQpDelta  = 4;
            pstGopAttr->stDualP.s32SPQpDelta  = 2;
            pstGopAttr->stDualP.u32SPInterval = 3;
        break;

        case VENC_GOPMODE_BIPREDB:
            pstGopAttr->enGopMode  = VENC_GOPMODE_BIPREDB;
            pstGopAttr->stBipredB.s32BQpDelta  = -2;
            pstGopAttr->stBipredB.s32IPQpDelta = 3;
            pstGopAttr->stBipredB.u32BFrmNum   = 2;
        break;

        default:
            SAMPLE_PRT("not support the gop mode !\n");
            return TS_FAILURE;
        break;
    }
    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_Getdcfinfo(char* SrcJpgPath, char* DstThmPath)
{
    TS_S32 s32RtnVal = TS_SUCCESS;
    TS_CHAR JPGSrcPath[FILE_NAME_LEN] = {0};
    TS_CHAR JPGDesPath[FILE_NAME_LEN] = {0};
    TS_U32 DstSize = 0;
    snprintf(JPGSrcPath, sizeof(JPGSrcPath), "%s", SrcJpgPath);
    snprintf(JPGDesPath, sizeof(JPGDesPath), "%s", DstThmPath);

    s32RtnVal = FileTrans_GetThmFromJpg(JPGSrcPath, &DstSize);
    if ((TS_SUCCESS != s32RtnVal) || (0 == DstSize))
    {
        printf("fail to get thm\n");
        return TS_FAILURE;
    }

    FILE* fpTHM = fopen(JPGDesPath, "w");
    if (TS_NULL == fpTHM)
    {
        printf("file to create file %s\n", JPGDesPath);
        return TS_FAILURE;
    }

    TS_U32 u32WritenSize = 0;
    while (u32WritenSize < DstSize)
    {
        s32RtnVal = fwrite(DstBuf + u32WritenSize, 1, DstSize, fpTHM);
        if (s32RtnVal <= 0)
        {
            printf("fail to wirte file, rtn=%d\n", s32RtnVal);
            break;
        }

        u32WritenSize += s32RtnVal;
    }

    if (fpTHM)
    {
        fclose(fpTHM);
        fpTHM = 0;
    }

    if(NULL != DstBuf)
    {
        free(DstBuf);
        DstBuf = NULL;
    }

    return 0;
}



/******************************************************************************
* funciton : save stream
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream)
{
    TS_S32 i;
	int ret;
	size_t nmemb;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
#ifndef HIDE_PRINT_0919
		printf("once write stream[%p:%d]\n", pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
			pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset);
#endif
        nmemb = fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
		if (nmemb != 1)
			SAMPLE_PRT("nmemb 1 should be written, but %d actually, error %d", nmemb, errno);

        ret = fflush(pFd);
		if (ret)
			SAMPLE_PRT("fflush ret %d, error %d", ret, errno);
    }

    return TS_SUCCESS;
}


/******************************************************************************
* funciton : the process of physical address retrace
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_SaveStream_PhyAddr(FILE* pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S* pstStream)
{
    TS_U32 i,j;
    TS_U64 u64SrcPhyAddr;
    TS_U32 u32Left;
    TS_S32 s32Ret = 0;

    for(i=0; i<pstStream->u32PackCount; i++)
    {
        for(j=0; j<MAX_TILE_NUM; j++)
        {
            if((pstStream->pstPack[i].u64PhyAddr > pstStreamBuf->u64PhyAddr[j])&&\
                (pstStream->pstPack[i].u64PhyAddr <= pstStreamBuf->u64PhyAddr[j]+pstStreamBuf->u64BufSize[j]))
                break;
        }

        if(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Len >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
        {
            if (pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
            {
                /* physical address retrace in offset segment */
                u64SrcPhyAddr = pstStreamBuf->u64PhyAddr[j] +
                                ((pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset) -
                                (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]));

                s32Ret = fwrite ((void *)(TS_UL)u64SrcPhyAddr, pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    SAMPLE_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
            else
            {
                /* physical address retrace in data segment */
                u32Left = (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]) - pstStream->pstPack[i].u64PhyAddr;

                s32Ret = fwrite((void *)(TS_UL)(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset),
                             u32Left - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    SAMPLE_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }

                s32Ret = fwrite((void *)(TS_UL)pstStreamBuf->u64PhyAddr[j], pstStream->pstPack[i].u32Len - u32Left, 1, pFd);
                if(s32Ret<0)
                {
                    SAMPLE_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
        }
        else
        {
            /* physical address retrace does not happen */
            s32Ret = fwrite ((void *)(TS_UL)(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset),
                          pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
            if(s32Ret<0)
            {
                SAMPLE_PRT("fwrite err %d\n", s32Ret);
                return s32Ret;
            }
        }
            fflush(pFd);
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_CloseReEncode(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    //VENC_RC_PARAM_S stRcParam;
    VENC_CHN_ATTR_S stChnAttr;

    s32Ret = TS_MPI_VENC_GetChnAttr(VencChn,&stChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("GetChnAttr failed!\n");
        return TS_FAILURE;
    }
	#if 0
    s32Ret = TS_MPI_VENC_GetRcParam(VencChn,&stRcParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("GetRcParam failed!\n");
        return TS_FAILURE;
    }

    if(VENC_RC_MODE_H264CBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H264VBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H265CBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H265VBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
    }
    else
    {
        return TS_SUCCESS;
    }
    s32Ret = TS_MPI_VENC_SetRcParam(VencChn,&stRcParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SetRcParam failed!\n");
        return TS_FAILURE;
    }
	#endif
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_CustomCreat(VENC_CHN VencChn,SIZE_S * pInSize)
{
	TS_S32 			s32Ret = TS_SUCCESS;
    VENC_ATTR_JPEG_S stJpegAttr;
	VENC_CHN_ATTR_S pstVencChnAttr;
	SAMPLE_RC_E 	enRcMode;
	TS_U32          u32Gop;
	TS_BOOL bRcnRefShareBuf;
	VENC_GOP_MODE_E enGopMode;
	SAMPLE_VI_CONFIG_S     stViConfig;
    TS_U32                 u32FrameRate;
	TS_U32                 u32StatTime;
	pstVencChnAttr.stVencAttr.u32MaxPicWidth  = pInSize->u32Width;
    pstVencChnAttr.stVencAttr.u32MaxPicHeight = pInSize->u32Height;
	pstVencChnAttr.stVencAttr.u32PicWidth     = pInSize->u32Width;/*the picture width*/
    pstVencChnAttr.stVencAttr.u32PicHeight    = pInSize->u32Height;/*the picture height*/
    pstVencChnAttr.stVencAttr.u32BufSize      = pInSize->u32Width * pInSize->u32Height * 2;/*stream buffer size*/
	pstVencChnAttr.stVencAttr.bByFrame        = TS_TRUE;/*get stream mode is slice mode or frame mode?*/


	enRcMode = SAMPLE_COMM_VENC_GetRcMode();
	enGopMode = SAMPLE_COMM_VENC_GetGopMode();
	s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&pstVencChnAttr.stGopAttr);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Get GopAttr for %#x!\n", s32Ret);
		return TS_FALSE;
	}

	bRcnRefShareBuf = SAMPLE_COMM_VENC_Get_PixelFormat_fromKeyboard(WITHOUT_WH);
	pstVencChnAttr.stVencAttr.enType = SAMPLE_COMM_VENC_GetPayLoadType();
	u32Gop =  SAMPLE_COMM_VENC_Get_int_fromKeyboard("GOP (integer)");
	pstVencChnAttr.stVencAttr.u32Profile = 2;//according to init_chn_attr_h265_cbr

	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return TS_FALSE;
    }
    s32Ret = SAMPLE_COMM_VI_GetFrameRateBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &u32FrameRate);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetFrameRateBySensor failed!\n");
        return s32Ret;
    }

	if(VENC_GOPMODE_SMARTP == pstVencChnAttr.stGopAttr.enGopMode)
    {
        u32StatTime = pstVencChnAttr.stGopAttr.stSmartP.u32BgInterval/u32Gop;
    }
    else
    {
        u32StatTime = 1;
    }

	switch (pstVencChnAttr.stVencAttr.enType)
    {
        case PT_H265:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H265_CBR_S    stH265Cbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;
                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                stH265Cbr.u32BitRate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("bitrate (Kbps/ Kilo Bits Per Second)");
                memcpy(&pstVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H265_VBR_S    stH265Vbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32FrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                stH265Vbr.u32MaxBitRate    = SAMPLE_COMM_VENC_Get_int_fromKeyboard("maxbitrate (Kbps/ Kilo Bits Per Second)");
                memcpy(&pstVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H265_CVBR_S    stH265CVbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
                stH265CVbr.u32Gop         = u32Gop;
                stH265CVbr.u32StatTime    = u32StatTime;
                stH265CVbr.u32SrcFrameRate  = u32FrameRate;
                stH265CVbr.fr32DstFrameRate = u32FrameRate;
                stH265CVbr.u32LongTermStatTime  = 1;
                stH265CVbr.u32ShortTermStatTime = u32StatTime;
                stH265CVbr.u32MaxBitRate         = SAMPLE_COMM_VENC_Get_int_fromKeyboard("maxbitrate (Kbps/ Kilo Bits Per Second)");
                stH265CVbr.u32LongTermMaxBitrate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("LongTermMaxBitrate (Kbps/ Kilo Bits Per Second)");
                stH265CVbr.u32LongTermMinBitrate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("LongTermMinBitrate (Kbps/ Kilo Bits Per Second)");

                memcpy(&pstVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(VENC_H265_CVBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H265_FIXQP_S    stH265FixQp;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = 30;
                stH265FixQp.u32SrcFrameRate     = u32FrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = SAMPLE_COMM_VENC_Get_int_fromKeyboard("QP (integer)");//25, according to init_chn_attr_h265_fixqp
                stH265FixQp.u32PQp              = stH265FixQp.u32IQp;
                stH265FixQp.u32BQp              = stH265FixQp.u32IQp;
                memcpy(&pstVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
            pstVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = bRcnRefShareBuf;
        break;
        case PT_H264:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H264_CBR_S    stH264Cbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                stH264Cbr.u32BitRate         = SAMPLE_COMM_VENC_Get_int_fromKeyboard("bitrate (Kbps/ Kilo Bits Per Second)");

                memcpy(&pstVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H264_FIXQP_S    stH264FixQp;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = 30;
                stH264FixQp.u32SrcFrameRate  = u32FrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = SAMPLE_COMM_VENC_Get_int_fromKeyboard("QP (integer)");;
                stH264FixQp.u32PQp           = stH264FixQp.u32IQp;
                stH264FixQp.u32BQp           = stH264FixQp.u32IQp;
                memcpy(&pstVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264Vbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32FrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                stH264Vbr.u32MaxBitRate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("maxbitrate (Kbps/ Kilo Bits Per Second)");

                memcpy(&pstVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }

            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H264_CVBR_S    stH264CVbr;

                pstVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
                stH264CVbr.u32Gop         = u32Gop;
                stH264CVbr.u32StatTime    = u32StatTime;
                stH264CVbr.u32SrcFrameRate  = u32FrameRate;
                stH264CVbr.fr32DstFrameRate = u32FrameRate;
                stH264CVbr.u32LongTermStatTime  = 1;
                stH264CVbr.u32ShortTermStatTime = u32StatTime;

                stH264CVbr.u32MaxBitRate         = SAMPLE_COMM_VENC_Get_int_fromKeyboard("maxbitrate (Kbps/ Kilo Bits Per Second)");
                stH264CVbr.u32LongTermMaxBitrate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("LongTermMaxBitrate (Kbps/ Kilo Bits Per Second)");
                stH264CVbr.u32LongTermMinBitrate = SAMPLE_COMM_VENC_Get_int_fromKeyboard("LongTermMinBitrate (Kbps/ Kilo Bits Per Second)");

                memcpy(&pstVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(VENC_H264_CVBR_S));
            }

            pstVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = bRcnRefShareBuf;
        }
        break;
        case PT_JPEG:
        {
            stJpegAttr.bSupportDCF     = TS_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&pstVencChnAttr.stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        }
        default:
            SAMPLE_PRT("cann't support this enType (%d) in this version!\n",pstVencChnAttr.stVencAttr.enType);
            return TS_ERR_VENC_NOT_SUPPORT;
    	}

	}

    if(PT_MJPEG == pstVencChnAttr.stVencAttr.enType || PT_JPEG == pstVencChnAttr.stVencAttr.enType )
    {
        pstVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        pstVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    }

	if((VENC_GOPMODE_BIPREDB == pstVencChnAttr.stGopAttr.enGopMode)&&(PT_H264 == pstVencChnAttr.stVencAttr.enType))
	    {
	        if(0 == pstVencChnAttr.stVencAttr.u32Profile)
	        {
	            pstVencChnAttr.stVencAttr.u32Profile = 1;

	            SAMPLE_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
	        }
	    }

	    if((VENC_RC_MODE_H264QPMAP == pstVencChnAttr.stRcAttr.enRcMode)||(VENC_RC_MODE_H265QPMAP == pstVencChnAttr.stRcAttr.enRcMode))
	    {
	        if(VENC_GOPMODE_ADVSMARTP == pstVencChnAttr.stGopAttr.enGopMode)
	        {
	            pstVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

	            SAMPLE_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
	        }
	    }

		SAMPLE_PRT("%s, MaxWH=[%d,%d], w=h[%d,%d], bufSize=%d\n",
			PT_H264 == pstVencChnAttr.stVencAttr.enType ? "H264" : "H265",
			pstVencChnAttr.stVencAttr.u32MaxPicWidth, pstVencChnAttr.stVencAttr.u32MaxPicHeight,
			pstVencChnAttr.stVencAttr.u32PicWidth, pstVencChnAttr.stVencAttr.u32PicHeight,
			pstVencChnAttr.stVencAttr.u32BufSize);

	s32Ret = TS_MPI_VENC_CreateChn(VencChn, &pstVencChnAttr);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
				   VencChn, s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
	if (TS_SUCCESS != s32Ret)
	{
		TS_MPI_VENC_DestroyChn(VencChn);
		return s32Ret;
	}

	return s32Ret;
}


TS_S32 SAMPLE_COMM_VENC_Creat(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, TS_U32  u32Profile, TS_BOOL bRcnRefShareBuf,VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S        stVencChnAttr;
    VENC_ATTR_JPEG_S       stJpegAttr;
    SAMPLE_VI_CONFIG_S     stViConfig;
    TS_U32                 u32FrameRate;
    TS_U32                 u32StatTime;
    TS_U32                 u32Gop = 30;//100;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize( enSize, &stPicSize);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get picture size failed!\n");
        return TS_FAILURE;
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return TS_FALSE;
    }
    s32Ret = SAMPLE_COMM_VI_GetFrameRateBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &u32FrameRate);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetFrameRateBySensor failed!\n");
        return s32Ret;
    }

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType          = enType;
    stVencChnAttr.stVencAttr.u32MaxPicWidth  = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth     = stPicSize.u32Width;/*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight    = stPicSize.u32Height;/*the picture height*/
    stVencChnAttr.stVencAttr.u32BufSize      = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
    stVencChnAttr.stVencAttr.u32Profile      = u32Profile;
    stVencChnAttr.stVencAttr.bByFrame        = TS_TRUE;/*get stream mode is slice mode or frame mode?*/

    if(VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode)
    {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval/u32Gop;
    }
    else
    {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H265_CBR_S    stH265Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;

                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
					case PIC_360P:
                		stH265Cbr.u32BitRate = 380;
						break;
                    case PIC_720P:
                        stH265Cbr.u32BitRate = 800;
                        break;
                    case PIC_1080P:
                        stH265Cbr.u32BitRate = 1024;
                        break;
                    case PIC_2304x1296:
                        stH265Cbr.u32BitRate = 1450;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2560x1440:
                        stH265Cbr.u32BitRate = 1600;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265Cbr.u32BitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Cbr.u32BitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Cbr.u32BitRate = 1024 * 3  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Cbr.u32BitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H265_FIXQP_S    stH265FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = 30;
                stH265FixQp.u32SrcFrameRate     = u32FrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = 25;
                stH265FixQp.u32PQp              = 30;
                stH265FixQp.u32BQp              = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H265_VBR_S    stH265Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32FrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH265Vbr.u32MaxBitRate = 1024*u32FrameRate/30;
						break;
                    case PIC_720P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2304x1296:
                        stH265Vbr.u32MaxBitRate = 1600;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
					case PIC_2560x1440:
						stH265Vbr.u32MaxBitRate = 1600; //1024 * 3 + 3072*u32FrameRate/30;
						break;
                    case PIC_2688x1860:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Vbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Vbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Vbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Vbr.u32MaxBitRate    = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
            else if(SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
            else if(SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H265_QVBR_S    stH265QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
                stH265QVbr.u32Gop         = u32Gop;
                stH265QVbr.u32StatTime    = u32StatTime;
                stH265QVbr.u32SrcFrameRate  = u32FrameRate;
                stH265QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265QVbr.u32TargetBitRate= 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265QVbr.u32TargetBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265QVbr.u32TargetBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265QVbr.u32TargetBitRate    = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H265_CVBR_S    stH265CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
                stH265CVbr.u32Gop         = u32Gop;
                stH265CVbr.u32StatTime    = u32StatTime;
                stH265CVbr.u32SrcFrameRate  = u32FrameRate;
                stH265CVbr.fr32DstFrameRate = u32FrameRate;
                stH265CVbr.u32LongTermStatTime  = 1;
                stH265CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH265CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH265CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(VENC_H265_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H265_QPMAP_S    stH265QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop           = u32Gop;
                stH265QpMap.u32StatTime      = u32StatTime;
                stH265QpMap.u32SrcFrameRate  = u32FrameRate;
                stH265QpMap.fr32DstFrameRate = u32FrameRate;
                stH265QpMap.enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
                memcpy(&stVencChnAttr.stRcAttr.stH265QpMap, &stH265QpMap, sizeof(VENC_H265_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
            stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = bRcnRefShareBuf;
        }
        break;
        case PT_H264:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H264_CBR_S    stH264Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                    case PIC_360P:
                        stH264Cbr.u32BitRate         = 380;
                        break;
                    case PIC_720P:
                        stH264Cbr.u32BitRate         = 800;
                        break;
                    case PIC_1080P:
                        stH264Cbr.u32BitRate         = 1024;
                        break;
                    case PIC_2304x1296:
                        stH264Cbr.u32BitRate         = 1450;
                        break;
                    case PIC_2560x1440:
                        stH264Cbr.u32BitRate         = 1600;
                        break;
                    case PIC_2688x1860:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Cbr.u32BitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Cbr.u32BitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Cbr.u32BitRate         = 1024 * 3  + 5120*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H264_FIXQP_S    stH264FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = 30;
                stH264FixQp.u32SrcFrameRate  = u32FrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = 25;
                stH264FixQp.u32PQp           = 30;
                stH264FixQp.u32BQp           = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32FrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Vbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Vbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Vbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 3  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 3  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
            else if (SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H264_QVBR_S    stH264QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
                stH264QVbr.u32Gop           = u32Gop;
                stH264QVbr.u32StatTime      = u32StatTime;
                stH264QVbr.u32SrcFrameRate  = u32FrameRate;
                stH264QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264QVbr.u32TargetBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264QVbr.u32TargetBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264QVbr.u32TargetBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264QVbr.u32TargetBitRate = 1024 * 3  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H264_CVBR_S    stH264CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
                stH264CVbr.u32Gop         = u32Gop;
                stH264CVbr.u32StatTime    = u32StatTime;
                stH264CVbr.u32SrcFrameRate  = u32FrameRate;
                stH264CVbr.fr32DstFrameRate = u32FrameRate;
                stH264CVbr.u32LongTermStatTime  = 1;
                stH264CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH264CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH264CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(VENC_H264_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H264_QPMAP_S    stH264QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop           = u32Gop;
                stH264QpMap.u32StatTime      = u32StatTime;
                stH264QpMap.u32SrcFrameRate  = u32FrameRate;
                stH264QpMap.fr32DstFrameRate = u32FrameRate;
                memcpy(&stVencChnAttr.stRcAttr.stH264QpMap, &stH264QpMap, sizeof(VENC_H264_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
            stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = bRcnRefShareBuf;
        }
        break;
        case PT_MJPEG:
        {
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_MJPEG_FIXQP_S stMjpegeFixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 95;
                stMjpegeFixQp.u32SrcFrameRate    = u32FrameRate;
                stMjpegeFixQp.fr32DstFrameRate   = u32FrameRate;

                memcpy(&stVencChnAttr.stRcAttr.stMjpegFixQp, &stMjpegeFixQp,sizeof(VENC_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_MJPEG_CBR_S stMjpegeCbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stMjpegeCbr.u32StatTime         = u32StatTime;
                stMjpegeCbr.u32SrcFrameRate     = u32FrameRate;
                stMjpegeCbr.fr32DstFrameRate    = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegeCbr.u32BitRate = 1024 * 3  + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegeCbr.u32BitRate = 1024 * 5  + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegeCbr.u32BitRate = 1024 * 8  + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegeCbr.u32BitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegeCbr.u32BitRate = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegeCbr.u32BitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegeCbr.u32BitRate = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegCbr, &stMjpegeCbr,sizeof(VENC_MJPEG_CBR_S));
            }
            else if ((SAMPLE_RC_VBR == enRcMode) ||(SAMPLE_RC_AVBR == enRcMode)||
                     (SAMPLE_RC_QVBR == enRcMode)||(SAMPLE_RC_CVBR == enRcMode))
            {
                VENC_MJPEG_VBR_S   stMjpegVbr;

                if(SAMPLE_RC_AVBR == enRcMode)
                {
                    SAMPLE_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stMjpegVbr.u32StatTime      = u32StatTime;
                stMjpegVbr.u32SrcFrameRate  = u32FrameRate;
                stMjpegVbr.fr32DstFrameRate = 5;

                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegVbr.u32MaxBitRate    = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegVbr.u32MaxBitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegVbr.u32MaxBitRate = 1024 * 3 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegVbr, &stMjpegVbr,sizeof(VENC_MJPEG_VBR_S));
            }
            else
            {
                SAMPLE_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return TS_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = TS_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&stVencChnAttr.stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            SAMPLE_PRT("cann't support this enType (%d) in this version!\n",enType);
            return TS_ERR_VENC_NOT_SUPPORT;
    }

    if(PT_MJPEG == enType || PT_JPEG == enType )
    {
        stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    }
    else
    {
        memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode)&&(PT_H264 == enType))
        {
            if(0 == stVencChnAttr.stVencAttr.u32Profile)
            {
                stVencChnAttr.stVencAttr.u32Profile = 1;

                SAMPLE_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if((VENC_RC_MODE_H264QPMAP == stVencChnAttr.stRcAttr.enRcMode)||(VENC_RC_MODE_H265QPMAP == stVencChnAttr.stRcAttr.enRcMode))
        {
            if(VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode)
            {
                stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                SAMPLE_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }

	//SAMPLE_COMM_VENC_GetChnAttrByTpye(enType, enSize, &stVencChnAttr);

	SAMPLE_PRT("create venChn[%d], %s, MaxWH=[%d,%d], w=h[%d,%d], bufSize=%d\n", VencChn,
		PT_H264 == stVencChnAttr.stVencAttr.enType ? "H264" : "H265",
		stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight,
		stVencChnAttr.stVencAttr.u32PicWidth, stVencChnAttr.stVencAttr.u32PicHeight,
		stVencChnAttr.stVencAttr.u32BufSize);

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        TS_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, TS_U32  u32Profile, TS_BOOL bRcnRefShareBuf,VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_Creat(VencChn,enType,enSize,enRcMode,u32Profile,bRcnRefShareBuf,pstGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SetSmartH265(VENC_CHN VencChn, TS_U8 u8Enable)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

	s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }

	s32Ret = TS_MPI_VENC_SetSmartParam(VencChn,&u8Enable);	
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_SetSmartParam faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }

	s32Ret = TS_MPI_VENC_GetSmartParam(VencChn,&u8Enable);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetSmartParam faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
	else
	{
		SAMPLE_PRT("TS_MPI_VENC_GetSmartParam enable=%d! \n", u8Enable);
	}

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_GetSmartH265(VENC_CHN VencChn, TS_U8 *u8Enable)
{
    TS_S32 s32Ret;

	s32Ret = TS_MPI_VENC_GetSmartParam(VencChn,u8Enable);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetSmartParam faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
	else
	{
		SAMPLE_PRT("TS_MPI_VENC_GetSmartParam enable=%d! \n", *u8Enable);
	}

    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_CustomStart(VENC_CHN VencChn,SIZE_S * pInSize)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_CustomCreat(VencChn,pInSize);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}



/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return TS_FAILURE;
    }    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = TS_MPI_VENC_DestroyChn(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start snap
******************************************************************************/

TS_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, TS_BOOL bSupportDCF)
{
    TS_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType = PT_JPEG;
    stVencChnAttr.stVencAttr.u32Profile         = 0;
    stVencChnAttr.stVencAttr.u32MaxPicWidth     = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight    = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth        = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32PicHeight       = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32BufSize         = pstSize->u32Width * pstSize->u32Height * 2;
    stVencChnAttr.stVencAttr.bByFrame           = TS_TRUE;/*get stream mode is field mode  or frame mode*/
    stVencChnAttr.stVencAttr.stAttrJpege.bSupportDCF = bSupportDCF;
    //stVencChnAttr.stVencAttr.stAttrJpege.bSupportXMP = TS_FALSE;
    stVencChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
    stVencChnAttr.stVencAttr.stAttrJpege.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x!\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Stop snap
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
    s32Ret = TS_MPI_VENC_DestroyChn(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/

TS_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_S32 s32VencFd;
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    TS_U32 i;

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = SnapCnt;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 3:  recv picture
    ******************************************/
    s32VencFd = TS_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return TS_FAILURE;
    }

    for(i=0; i<SnapCnt; i++)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32VencFd, &read_fds);
        TimeoutVal.tv_sec  = 10;
        TimeoutVal.tv_usec = 0;
        s32Ret = TS_MPI_VENC_SELECT(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("snap select failed!\n");
            return TS_FAILURE;
        }
        else if (0 == s32Ret)
        {
            SAMPLE_PRT("snap time out!\n");
            return TS_FAILURE;
        }
        else
        {
            if (FD_ISSET(s32VencFd, &read_fds))
            {
                s32Ret = TS_MPI_VENC_QueryStatus(VencChn, &stStat);
                if (s32Ret != TS_SUCCESS)
                {
                    SAMPLE_PRT("TS_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                    return TS_FAILURE;
                }
                /*******************************************************
                suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                 {                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                 }
                 *******************************************************/
                if (0 == stStat.u32CurPacks)
                {
                    SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                }
                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    SAMPLE_PRT("malloc memory failed!\n");
                    return TS_FAILURE;
                }
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, -1);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    return TS_FAILURE;
                }
                if(bSaveJpg || bSaveThm)
                {
                    char acFile[FILE_NAME_LEN]    = {0};
                    FILE* pFile;

                    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
                    pFile = fopen(acFile, "wb");
                    if (pFile == NULL)
                    {
                        SAMPLE_PRT("open file err\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        return TS_FAILURE;
                    }

                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("save snap picture failed!\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;

                        fclose(pFile);
                        return TS_FAILURE;
                    }

                    if(bSaveThm)
                    {
                        char acFile_dcf[FILE_NAME_LEN]  = {0};
                        snprintf(acFile_dcf, FILE_NAME_LEN, "snap_thm_%d.jpg", gs_s32SnapCnt);
                        s32Ret = SAMPLE_COMM_VENC_Getdcfinfo(acFile, acFile_dcf);
                        if (TS_SUCCESS != s32Ret)
                        {
                            SAMPLE_PRT("save thm picture failed!\n");

                            free(stStream.pstPack);
                            stStream.pstPack = NULL;

                            fclose(pFile);
                            return TS_FAILURE;
                        }
                    }

                    fclose(pFile);
                    gs_s32SnapCnt++;
                }

                s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;

                    return TS_FAILURE;
                }

                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, TS_U32 SnapCnt)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_S32 s32VencFd;
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    TS_U32 i;

    s32VencFd = TS_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return TS_FAILURE;
    }

    for(i=0; i<SnapCnt; i++)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32VencFd, &read_fds);
        TimeoutVal.tv_sec  = 10;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("snap select failed!\n");
            return TS_FAILURE;
        }
        else if (0 == s32Ret)
        {
            SAMPLE_PRT("snap time out!\n");
            return TS_FAILURE;
        }
        else
        {
            if (FD_ISSET(s32VencFd, &read_fds))
            {
                s32Ret = TS_MPI_VENC_QueryStatus(VencChn, &stStat);
                if (s32Ret != TS_SUCCESS)
                {
                    SAMPLE_PRT("TS_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                    return TS_FAILURE;
                }
                /*******************************************************
                suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                 {                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                 }
                 *******************************************************/
                if (0 == stStat.u32CurPacks)
                {
                    SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                }
                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    SAMPLE_PRT("malloc memory failed!\n");
                    return TS_FAILURE;
                }
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, -1);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    return TS_FAILURE;
                }
                if(1)
                {
                    char acFile[FILE_NAME_LEN]    = {0};
                    FILE* pFile;

                    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
                    pFile = fopen(acFile, "wb");
                    if (pFile == NULL)
                    {
                        SAMPLE_PRT("open file err\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        return TS_FAILURE;
                    }

                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("save snap picture failed!\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;

                        fclose(pFile);
                        return TS_FAILURE;
                    }

                    fclose(pFile);
                    gs_s32SnapCnt++;
                }

                s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;

                    return TS_FAILURE;
                }

                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }

    return TS_SUCCESS;
}


#define QpMapBufNum 8
TS_VOID* SAMPLE_COMM_QpmapSendFrameProc(TS_VOID* p)
{
    TS_U32 i,j,VeChnCnt;
    TS_S32 s32Ret;
    VIDEO_FRAME_INFO_S  *pstVideoFrame;
    USER_FRAME_INFO_S   stFrame[QpMapBufNum];
    SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S *pstPara;
    //TS_U32 u32AlignPicHeight ;
    //TS_U32 u32AlignPicWidth  ;

    TS_U32   u32QpMapSize;
    TS_U64   u64QpMapPhyAddr[QpMapBufNum];
    TS_VOID* pQpMapVirAddr[QpMapBufNum];
    TS_U32 u32QpMapSizeHeight ;
    TS_U32 u32QpMapSizeWidth  ;
    TS_U8 *pVirAddr;
    TS_U64 u64PhyAddr;
    TS_U8 *pVirAddrTemp;

    TS_U32   u32SkipWeightHeight_H264 ;
    TS_U32   u32SkipWeightWidth_H264  ;
    TS_U32   u32SkipWeightSize_H264;
    TS_U64   u64SkipWeightPhyAddr_H264[QpMapBufNum];
    TS_VOID* pSkipWeightVirAddr_H264[QpMapBufNum];

    TS_U32   u32SkipWeightHeight_H265 ;
    TS_U32   u32SkipWeightWidth_H265  ;
    TS_U32   u32SkipWeightSize_H265;
    TS_U64   u64SkipWeightPhyAddr_H265[QpMapBufNum];
    TS_VOID* pSkipWeightVirAddr_H265[QpMapBufNum];

    VPSS_CHN_ATTR_S  stChnAttr;

    pstPara = (SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S*)p;

    //u32AlignPicWidth  = (pstPara->stSize.u32Height + 15)/16;
    //u32AlignPicHeight = (pstPara->stSize.u32Height + 15)/16;

    /* qpmap */
    u32QpMapSizeWidth  = (pstPara->stSize.u32Width + 511)/512 *32;
    u32QpMapSizeHeight = (pstPara->stSize.u32Height + 15)/16;
    u32QpMapSize      = u32QpMapSizeWidth * u32QpMapSizeHeight;
    s32Ret = TS_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, TS_NULL, u32QpMapSize*QpMapBufNum);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        return NULL;
    }

    for(i=0; i<QpMapBufNum; i++)
    {
        u64QpMapPhyAddr[i] = u64PhyAddr + i*u32QpMapSize;
        pQpMapVirAddr[i]   = pVirAddr + i*u32QpMapSize;
    }

    /* skipweight h.264 */
    u32SkipWeightWidth_H264  = (pstPara->stSize.u32Width + 511)/512 *16;
    u32SkipWeightHeight_H264 = (pstPara->stSize.u32Height + 15)/16;
    u32SkipWeightSize_H264   = u32SkipWeightWidth_H264*u32SkipWeightHeight_H264;
    s32Ret = TS_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, TS_NULL, u32SkipWeightSize_H264*QpMapBufNum);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        TS_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
        return NULL;
    }

    for(i=0; i<QpMapBufNum; i++)
    {
        u64SkipWeightPhyAddr_H264[i] = u64PhyAddr + i*u32SkipWeightSize_H264;
        pSkipWeightVirAddr_H264[i]   = pVirAddr + i*u32SkipWeightSize_H264;
    }

    /* skipweight h.265 */
    u32SkipWeightWidth_H265  = (pstPara->stSize.u32Width + 2047)/2048 *16;
    u32SkipWeightHeight_H265 = (pstPara->stSize.u32Height + 63)/64;
    u32SkipWeightSize_H265   = u32SkipWeightWidth_H265*u32SkipWeightHeight_H265;
    s32Ret = TS_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, TS_NULL, u32SkipWeightSize_H265*QpMapBufNum);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        TS_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
        TS_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H264[0],pSkipWeightVirAddr_H264[0]);
        return NULL;
    }
    for(i=0; i<QpMapBufNum; i++)
    {
        u64SkipWeightPhyAddr_H265[i] = u64PhyAddr + i*u32SkipWeightSize_H265;
        pSkipWeightVirAddr_H265[i]   = pVirAddr + i*u32SkipWeightSize_H265;
    }


    s32Ret = TS_MPI_VPSS_GetChnAttr(pstPara->VpssGrp,pstPara->VpssChn,&stChnAttr);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VPSS_GetChnAttr err:0x%x",s32Ret);

        return NULL;
    }

    stChnAttr.u32Depth = 3;
    s32Ret = TS_MPI_VPSS_SetChnAttr(pstPara->VpssGrp,pstPara->VpssChn,&stChnAttr);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr err:0x%x",s32Ret);

        return NULL;
    }

    i=0;
    while(TS_TRUE == pstPara->bThreadStart)
    {
        pstVideoFrame = &stFrame[i].stUserFrame;
        s32Ret = TS_MPI_VPSS_GetChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame,1000);
        if(TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VPSS_GetChnFrame err:0x%x\n",s32Ret);
            continue;
        }

        pVirAddrTemp = (TS_U8 *)pQpMapVirAddr[i];
        for(j=0; j<u32QpMapSize; j++)
        {
            *pVirAddrTemp = 0x5E;
            pVirAddrTemp++;
        }

        pVirAddrTemp = (TS_U8 *)pSkipWeightVirAddr_H264[i];
        for(j=0; j<u32SkipWeightSize_H264; j++)
        {
            *pVirAddrTemp = 0x88;
            pVirAddrTemp++;
        }

        pVirAddrTemp = (TS_U8 *)pSkipWeightVirAddr_H265[i];
        for(j=0; j<u32SkipWeightSize_H265; j++)
        {
            *pVirAddrTemp = 0x88;
            pVirAddrTemp++;
        }

        for(VeChnCnt=0; VeChnCnt<pstPara->s32Cnt; VeChnCnt++)
        {
            VENC_CHN_ATTR_S stChnAttr;
            TS_MPI_VENC_GetChnAttr(pstPara->VeChn[VeChnCnt],&stChnAttr);
            if(PT_H264 ==stChnAttr.stVencAttr.enType)
            {
                stFrame[i].stUserRcInfo.bSkipWeightValid = 1;
                stFrame[i].stUserRcInfo.u64SkipWeightPhyAddr = u64SkipWeightPhyAddr_H264[i];
            }
            else if(PT_H265 ==stChnAttr.stVencAttr.enType)
            {
                stFrame[i].stUserRcInfo.bSkipWeightValid = 1;
                stFrame[i].stUserRcInfo.u64SkipWeightPhyAddr = u64SkipWeightPhyAddr_H265[i];
            }
            else
            {
                continue;
            }

            stFrame[i].stUserRcInfo.bQpMapValid     = 1;
            stFrame[i].stUserRcInfo.u64QpMapPhyAddr = u64QpMapPhyAddr[i];
            stFrame[i].stUserRcInfo.u32BlkStartQp   = 30;
            stFrame[i].stUserRcInfo.enFrameType = VENC_FRAME_TYPE_NONE;

            s32Ret = TS_MPI_VENC_SendFrame(pstPara->VeChn[VeChnCnt], &stFrame[i].stUserFrame,-1);
            if(TS_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("TS_MPI_VENC_SendFrame err:0x%x\n",s32Ret);
                break;
            }
        }
        if(TS_SUCCESS != s32Ret)
        {
            s32Ret = TS_MPI_VPSS_ReleaseChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame);
            if(TS_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame err:0x%x",s32Ret);
                goto err_out;
            }
            continue;
        }

        s32Ret = TS_MPI_VPSS_ReleaseChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame);
        if(TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame err:0x%x",s32Ret);
            goto err_out;
        }

        i++;
        if(i >= QpMapBufNum)
        {
            i = 0;
        }
    }
err_out:
    s32Ret = TS_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

    s32Ret =  TS_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H264[0],pSkipWeightVirAddr_H264[0]);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

    s32Ret = TS_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H265[0],pSkipWeightVirAddr_H265[0]);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

   return NULL;
}

static void debug_save_file(void * buf, int len, char *fileName){
	int fd = -1;
	unsigned long bytes;
	if (-1 == fd){
		fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd < 0){
			SAMPLE_PRT("unable to create debug file.");
			fd = -2;
		}
	}

	if (fd > 0){
		bytes = write(fd, buf, len);
		SAMPLE_PRT(" file %s write bytes %ld yuv debug file closed .",fileName, bytes);
		close(fd);
		fd = -2;
	}
}

TS_S32 SAMPLE_COMM_VENC_Save_Frame(VIDEO_FRAME_INFO_S *video_frame, char *name)
{
	static int cnt = 0;
	char fullname[128] = {0};
	char type[16] = {0};
	int denominator = 1;
	int len = 0;
	int w = 0;
	int h = 0;

	if (!video_frame || !name) {
		return TS_FAILURE;
	}

	w = video_frame->stVFrame.u32Width;
	h = video_frame->stVFrame.u32Height;

	switch (video_frame->stVFrame.enPixelFormat) {
	case PIXEL_FORMAT_NV_12:
		denominator = 2;
		strncpy(type, "NV12", sizeof(type));
		break;
	default:
		denominator = 1;
		strncpy(type, "unknown", sizeof(type));
		break;
	}

	len = w * h * 3 / denominator;

	snprintf(fullname, sizeof(fullname), "%s_%dx%d_P%dM%d_%d.%s",
		name, w, h, video_frame->u32PoolId, video_frame->enModId, ++cnt, type);
	if (strlen(fullname) >= sizeof(fullname) - 1) SAMPLE_PRT("file name may be truncated!");

	debug_save_file((void *)(unsigned long)video_frame->stVFrame.u64VirAddr[0], len, fullname);

	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_Manual_Capture(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn, TS_BOOL enableJPEG)
{
	TS_S32 s32Ret = TS_FAILURE;
	VIDEO_FRAME_INFO_S stVideoFrame = {0};
	VIDEO_FRAME_INFO_S *pstVideoFrame = &stVideoFrame;
    VENC_RECV_PIC_PARAM_S stRecvParam = {-1};

	s32Ret = TS_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, pstVideoFrame, 1000);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VPSS_GetChnFrame err:0x%x\n",s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VENC_Save_Frame(pstVideoFrame, "capture");
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_VENC_Save_Frame err:0x%x",s32Ret);
	}

	if (enableJPEG) {
		SAMPLE_PRT("BF TS_MPI_VENC_StartRecvFrame");
		s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
		if(TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_VENC_StartRecvFrame err:0x%x\n",s32Ret);
		}

        s32Ret = TS_MPI_VB_DupBlock(pstVideoFrame->u32PoolId, pstVideoFrame->stVFrame.u64PhyAddr[0]);
        if(TS_SUCCESS != s32Ret) {
            SAMPLE_PRT("TS_MPI_VB_DupBlock error:%x\n", s32Ret);
        }

		SAMPLE_PRT("BF TS_MPI_VENC_SendFrame");
		s32Ret = TS_MPI_VENC_SendFrame(VencChn, pstVideoFrame,-1);
		if(TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_VENC_SendFrame err:0x%x\n",s32Ret);
		} else {
			SAMPLE_PRT("TS_MPI_VENC_SendFrame finish");
		}
	}

	s32Ret = TS_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, pstVideoFrame);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame err:0x%x",s32Ret);
	}

	if (enableJPEG) {
		sleep(1);//wait for getstream and consume fininsh
		SAMPLE_PRT("BF TS_MPI_VENC_StopRecvFrame");
		s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
		if(TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame err:0x%x\n",s32Ret);
		} else {
			SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame finish");
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_COMM_VENC_QpmapSendFrame(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,VENC_CHN VeChn[],TS_S32 s32Cnt,SIZE_S stSize)
{
    TS_S32 i;

    stQpMapSendFramePara.bThreadStart = TS_TRUE;
    stQpMapSendFramePara.VpssGrp = VpssGrp;
    stQpMapSendFramePara.VpssChn = VpssChn;
    stQpMapSendFramePara.s32Cnt  = s32Cnt;
    stQpMapSendFramePara.stSize  = stSize;
    for(i=0; i<s32Cnt;i++)
    {
        stQpMapSendFramePara.VeChn[i] = VeChn[i];
    }

    return pthread_create(&gs_VencQpmapPid, 0, SAMPLE_COMM_QpmapSendFrameProc, (TS_VOID*)&stQpMapSendFramePara);
}

TS_S32 SAMPLE_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S * pFrame, TS_S32 s32MilliSec)
{
	return TS_MPI_VENC_SendFrame(VencChn, pFrame, s32MilliSec);
}

#ifdef HIDE_PRINT_0919
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr)[0])//without check __must_be_array(arr)
#endif

#define UPDATE_COUNT_INTERVAL (30*10)
#define REFRESH_TARGET_RATE (4.0f) //per second
#define DIFF_RATE (0.5f)

static char angle_loop[] = {'|','/','-','\\','|','/','-','\\'};

void update_refresh_interval(int *interval)
{
	static unsigned int count = 0;
	static time_t last = 0;
	static time_t now;
	float real_call_rate;

	if (0 == count % UPDATE_COUNT_INTERVAL) {
		now = time(NULL);
		real_call_rate = ((float)UPDATE_COUNT_INTERVAL) / ((float)(now - last));

		(*interval) = (int)(real_call_rate / REFRESH_TARGET_RATE);

		*interval = (*interval <= 0) ? 1 : (*interval);

//		printf("now=%ld, last=%ld, real_call_rate=%0.2f, interval=%d\n", now, last, real_call_rate,*interval);

		last = now;
	}
	count++;

	return;
}

void show_running_fan(void)
{
	static int REFRESH_INTERVAL = 1;
	static int tick = 0;
	static int idx = 0;
	if (0 == tick % REFRESH_INTERVAL) {
		printf("\r Running <%c>", angle_loop[idx]);
		fflush(stdout);
		idx++;
		idx = (idx >= ARRAY_SIZE(angle_loop)) ? 0 : idx;
	}
	tick++;
	update_refresh_interval(&REFRESH_INTERVAL);
	return;
}
#endif


/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
TS_VOID* SAMPLE_COMM_VENC_GetVencStreamProc(TS_VOID* p)
{
    TS_S32 i;
    TS_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    TS_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_U32 u32PictureCnt[VENC_MAX_CHN_NUM]={0};
    TS_S32 VencFd[VENC_MAX_CHN_NUM];
    TS_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];

    prctl(PR_SET_NAME, "GetVencStream", 0,0,0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;
    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = pstPara->VeChn[i];
        s32Ret = TS_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }
        if(PT_JPEG != enPayLoadType[i])
        {
            snprintf(aszFileName[i],32, "stream_chn%d%s", i, szFilePostfix);

            pFile[i] = fopen(aszFileName[i], "wb");
            if (!pFile[i])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i]);
                return NULL;
            }
        }
        /* Set Venc Fd. */
        VencFd[i] = TS_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }
#ifdef RTSP_DEBUG_ON_JCY
	openTSingRTSPServer(stVencChnAttr.stVencAttr.enType);
#endif

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (TS_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 15;
        TimeoutVal.tv_usec = 0;
        s32Ret = TS_MPI_VENC_SELECT(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = TS_MPI_VENC_QueryStatus(i, &stStat);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if(0 == stStat.u32CurPacks)
                    {
                          SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                          continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = TS_MPI_VENC_GetStream(i, &stStream, TS_TRUE);
                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    if (PT_JPEG == enPayLoadType[i])
                    {
                        snprintf(aszFileName[i],32, "stream_chn%d_%d%s", i, u32PictureCnt[i],szFilePostfix);
                        pFile[i] = fopen(aszFileName[i], "wb");
                        if (!pFile[i])
                        {
                            SAMPLE_PRT("open file err!\n");
                            return NULL;
                        }
                    }

                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i], &stStream);
                    //s32Ret = SAMPLE_COMM_VENC_SaveStream_PhyAddr(pFile[i], &stStreamBufInfo[i], &stStream);

#ifdef RTSP_DEBUG_ON_JCY
					SAMPLE_PRT("TS_SAMPLE_VencPackageStream start\n");
					TS_SAMPLE_VencPackageStream(&stStream, i, wrFd);
					SAMPLE_PRT("TS_SAMPLE_VencPackageStream over\n");
#endif

                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.6 : release stream
                     *******************************************************/
                    s32Ret = TS_MPI_VENC_ReleaseStream(i, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed!\n");
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }

                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    u32PictureCnt[i]++;
                    if(PT_JPEG == enPayLoadType[i])
                    {
                        fclose(pFile[i]);
                    }
                }
            }
        }
    }
    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        if(PT_JPEG != enPayLoadType[i])
        {
            fclose(pFile[i]);
        }
    }
#ifdef RTSP_DEBUG_ON_JCY
//	ProconVideoClose(wrFd);
	wrFd = 0;
	SAMPLE_PRT("ProconVideoClose close!\n");
#endif
	return NULL;
}

TS_VOID SAMPLE_COMM_VENC_GetTestSavePath(TS_VOID)
{
	FILE *fp = fopen("/root/test_save_path", "r");
	if (NULL == fp)
	{
		goto exit;
	}
	fgets(g_venc_save_path, sizeof(g_venc_save_path) - 1, fp);
	g_venc_save_path[strlen(g_venc_save_path)-1] = 0;
	fclose(fp);
	return;
exit:
	strncpy(g_venc_save_path, "/tmp", sizeof(g_venc_save_path));
	return;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
TS_VOID* SAMPLE_COMM_VENC_GetVencStreamProcV2(TS_VOID* p)
{
    TS_S32 i;
    TS_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    TS_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_U32 u32PictureCnt[VENC_MAX_CHN_NUM]={0};
    TS_S32 VencFd[VENC_MAX_CHN_NUM];
    TS_CHAR aszFileName[VENC_MAX_CHN_NUM][256];
    FILE* pFile[VENC_MAX_CHN_NUM];
#ifdef DYN_SWITCH_ENCODER
    FILE* pFile2[VENC_MAX_CHN_NUM];
    VENC_CHN_ATTR_S chn_attr = {0};
#endif
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
	enum SAMPLE_VENC_CONSUME_TYPE consume_type;

    prctl(PR_SET_NAME, "GetVencStream", 0,0,0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
	if (!pstPara)
		return NULL;
	consume_type = pstPara->consume_type;
    s32ChnTotal = pstPara->s32Cnt;

	SAMPLE_COMM_VENC_GetTestSavePath();
	SAMPLE_PRT("venc save path:[%s]\n", g_venc_save_path);
    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = pstPara->VeChn[i];
        s32Ret = TS_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }
        if(PT_JPEG != enPayLoadType[i])
        {
			if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
				snprintf(aszFileName[i], 256, "%s/stream_chn%d%s", g_venc_save_path, i, szFilePostfix);
	            pFile[i] = fopen(aszFileName[i], "wb");
	            if (!pFile[i])
	            {
	                SAMPLE_PRT("open file[%s] failed!\n",
	                           aszFileName[i]);
	                return NULL;
	            }
			}
        }

#ifdef DYN_SWITCH_ENCODER
		memset(szFilePostfix, 0, sizeof(szFilePostfix));
		if (enPayLoadType[i] == PT_H264) {
			s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(PT_H265, szFilePostfix);
			if (s32Ret != TS_SUCCESS)
			{
				SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
						   stVencChnAttr.stVencAttr.enType, s32Ret);
				return NULL;
			}
		} else if (enPayLoadType[i] == PT_H265) {
			s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(PT_H264, szFilePostfix);
			if (s32Ret != TS_SUCCESS)
			{
				SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
						   stVencChnAttr.stVencAttr.enType, s32Ret);
				return NULL;
			}
		}
		memset(aszFileName[i], 0, 256);
        if(PT_JPEG != enPayLoadType[i])
        {
			if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
				snprintf(aszFileName[i], 256, "%s/stream_chn%d%s", g_venc_save_path, i, szFilePostfix);
	            pFile2[i] = fopen(aszFileName[i], "wb");
	            if (!pFile2[i])
	            {
	                SAMPLE_PRT("open file[%s] failed!\n",
	                           aszFileName[i]);
	                return NULL;
	            }
			}
        }
#endif
        /* Set Venc Fd. */
        VencFd[i] = TS_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }
	

	//if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH)
		//openTSingRTSPServer(stVencChnAttr.stVencAttr.enType);


    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (TS_TRUE == pstPara->bThreadStart)
    {
		if (g_getstream_pause) {
#ifdef FAST_SWITCH_RESOLUTION
			sem_post(&g_switch_sem);
			sched_yield();
#else
			usleep(5 * 1000);
#endif
			continue;
		}

#ifdef HIDE_PRINT_0919
		show_running_fan();
#endif

        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 0;
#ifdef FAST_SWITCH_RESOLUTION
        TimeoutVal.tv_usec = 67 * 1000;//15fps, 66.7ms per frame
#else
        TimeoutVal.tv_usec = 500 * 1000;
#endif
        s32Ret = TS_MPI_VENC_SELECT(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, try again\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = TS_MPI_VENC_QueryStatus(i, &stStat);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if(0 == stStat.u32CurPacks)
                    {
                          SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                          continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = TS_MPI_VENC_GetStream(i, &stStream, TS_TRUE);
                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }

					//SAMPLE_PRT("packCount=%d, type=%d, wh=[%d,%d]\n", stStream.u32PackCount,
					//	stStream.pstPack->DataType.enH265EType,
					//	stStream.u32Width, stStream.u32Height);

                     //static int cunt=1;
                    // char time_start[40];
                    // get_time_ms(time_start, sizeof(time_start));
                   // printf("TS_MPI_VENC_GetStream cunt=%d,%s\n", cunt++, time_start);

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    if (PT_JPEG == enPayLoadType[i])
                    {
						if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
							snprintf(aszFileName[i], 256, "%s/stream_chn%d_%d%s", g_venc_save_path, i, u32PictureCnt[i],szFilePostfix);
	                        pFile[i] = fopen(aszFileName[i], "wb");
	                        if (!pFile[i])
	                        {
	                            SAMPLE_PRT("open file err!\n");
	                            return NULL;
	                        }
						}
                    }

					if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
#ifdef DYN_SWITCH_ENCODER
						s32Ret = TS_MPI_VENC_GetChnAttr(i, &chn_attr);
						if(TS_SUCCESS != s32Ret){
							SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
						}

						if (chn_attr.stVencAttr.enType == enPayLoadType[i]) {
							s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i], &stStream);
						} else {
							s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile2[i], &stStream);
						}
#else
						s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i], &stStream);
#endif
					}

					if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH) {
#ifndef HIDE_PRINT_0919
						SAMPLE_PRT("TS_SAMPLE_VencPackageStream start\n");
#endif
						//TS_SAMPLE_VencPackageStream(&stStream, i, wrFd);
#ifndef HIDE_PRINT_0919
						SAMPLE_PRT("TS_SAMPLE_VencPackageStream over\n");
#endif
					}

                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.6 : release stream
                     *******************************************************/
                    s32Ret = TS_MPI_VENC_ReleaseStream(i, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed!\n");
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }

                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    u32PictureCnt[i]++;
					if(u32PictureCnt[i] % 100 == 0){
						SAMPLE_PRT("venc stream[%d] recv %d frames\n", i, u32PictureCnt[i]);
					}
                    if(PT_JPEG == enPayLoadType[i])
                    {
						if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
                        	fclose(pFile[i]);
						}
                    }
                }
            }
        }
    }
    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        if(PT_JPEG != enPayLoadType[i])
        {
			if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
	            fclose(pFile[i]);
#ifdef DYN_SWITCH_ENCODER
	            fclose(pFile2[i]);
#endif
			}
        }
    }
	if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH) {
//		ProconVideoClose(wrFd);
		//wrFd = 0;
		SAMPLE_PRT("ProconVideoClose close!\n");
	}
    return NULL;
}

/******************************************************************************
* funciton : request IDR
******************************************************************************/
TS_VOID* SAMPLE_COMM_VENC_IDRProc(TS_VOID* p)
{
    TS_S32 i;
    TS_S32 s32ChnTotal;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    TS_S32 s32Ret;
    VENC_CHN VencChn;

    prctl(PR_SET_NAME, "RequestIDR", 0,0,0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    while (TS_TRUE == pstPara->bThreadStart) {
	    for (i = 0; i < s32ChnTotal; i++) {
			VencChn = pstPara->VeChn[i];

			s32Ret = TS_MPI_VENC_RequestIDR(VencChn, (i > 0 ? TS_FALSE : TS_TRUE));
	        if (s32Ret != TS_SUCCESS) {
	            SAMPLE_PRT("TS_MPI_VENC_RequestIDR chn[%d] failed with %#x!\n", \
	                       VencChn, s32Ret);
	            return NULL;
	        }
		}
		sleep(5);
	}
    return NULL;
}


/******************************************************************************
* funciton : get svc_t stream from h264 channels and save them
******************************************************************************/
TS_VOID* SAMPLE_COMM_VENC_GetVencStreamProc_Svc_t(void* p)
{
    TS_S32 i = 0;
    TS_S32 s32Cnt = 0;
    TS_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    TS_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_S32 VencFd[VENC_MAX_CHN_NUM];
    TS_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = TS_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }

        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            snprintf(aszFileName[i + s32Cnt],32, "Tid%d%s", i + s32Cnt, szFilePostfix);

            pFile[i + s32Cnt] = fopen(aszFileName[i + s32Cnt], "wb");

            if (!pFile[i + s32Cnt])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i + s32Cnt]);
                return NULL;
            }
        }

        /* Set Venc Fd. */
        VencFd[i] = TS_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (TS_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }
        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = TS_MPI_VENC_QueryStatus(i, &stStat);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }
                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                     *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = TS_MPI_VENC_GetStream(i, &stStream, TS_TRUE);
                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        switch (s32Cnt)
                        {
                            case 0:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {

                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i+s32Cnt], &stStream);

                                }
                                break;
                            case 1:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType      ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i+s32Cnt], &stStream);
                                }
                                break;
                            case 2:
                                s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i+s32Cnt], &stStream);

                                break;
                        }
                        if (TS_SUCCESS != s32Ret)
                        {
                            free(stStream.pstPack);
                            stStream.pstPack = NULL;
                            SAMPLE_PRT("save stream failed!\n");
                            break;
                        }
                    }

                    /*******************************************************
                    step 2.6 : release stream
                    *******************************************************/
                    s32Ret = TS_MPI_VENC_ReleaseStream(i, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }
    /*******************************************************
     step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            if (pFile[i + s32Cnt])
            {
                fclose(pFile[i + s32Cnt]);
            }
        }
    }
    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[],TS_S32 s32Cnt)
{
    TS_U32 i;

    gs_stPara.bThreadStart = TS_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    for(i=0; i<s32Cnt; i++)
    {
        gs_stPara.VeChn[i] = VeChn[i];
    }
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProc, (TS_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartGetStreamV2(VENC_CHN VeChn[],TS_S32 s32Cnt, enum SAMPLE_VENC_CONSUME_TYPE type)
{
    TS_U32 i;

    gs_stPara.bThreadStart = TS_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    for(i=0; i<s32Cnt; i++)
    {
        gs_stPara.VeChn[i] = VeChn[i];
    }
	gs_stPara.consume_type = type;
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProcV2, (TS_VOID*)&gs_stPara);
}

TS_S32 SAMPLE_COMM_VENC_SwitchResolution(VENC_CHN VeChn, TS_U32 width, TS_U32 height)
{
	TS_S32 s32Ret = TS_FAILURE;
	VENC_CHN_ATTR_S chn_attr;
    VENC_RECV_PIC_PARAM_S  stRecvParam = {-1};

#ifdef FAST_SWITCH_RESOLUTION
	sem_init(&g_switch_sem, TS_FALSE, 0);
	g_getstream_pause = TS_TRUE;
	sem_wait(&g_switch_sem);
#else
	g_getstream_pause = TS_TRUE;
	usleep(500 * 1000);
#endif

	s32Ret = TS_MPI_VENC_StopRecvFrame(VeChn);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_VENC_GetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}

	chn_attr.stVencAttr.u32PicWidth = width;
	chn_attr.stVencAttr.u32PicHeight = height;
	chn_attr.stVencAttr.u32BufSize	= width * height;

	s32Ret = TS_MPI_VENC_SetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VENC_SetChnAttr success, new WH=[%d,%d]\n",
		width, height);

	s32Ret = TS_MPI_VENC_StartRecvFrame(VeChn, &stRecvParam);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_StartRecvFrame faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}

	g_getstream_pause = TS_FALSE;
#ifdef FAST_SWITCH_RESOLUTION
	sem_destroy(&g_switch_sem);
#endif

	return s32Ret;
}

TS_S32 SAMPLE_COMM_VENC_SwitchEncoder(VENC_CHN VeChn, PAYLOAD_TYPE_E type)
{
	TS_S32 s32Ret = TS_FAILURE;
	VENC_CHN_ATTR_S chn_attr;
    //VENC_RECV_PIC_PARAM_S  stRecvParam = {-1};

#ifdef FAST_SWITCH_RESOLUTION
	sem_init(&g_switch_sem, TS_FALSE, 0);
	g_getstream_pause = TS_TRUE;
	sem_wait(&g_switch_sem);
#else
	g_getstream_pause = TS_TRUE;
	usleep(500 * 1000);
#endif

	s32Ret = TS_MPI_VENC_GetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}

	chn_attr.stVencAttr.enType = type;
	if (PT_H265 == type){
		chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
	}
	else{
		chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
	}
	s32Ret = TS_MPI_VENC_SetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
#ifdef FAST_SWITCH_RESOLUTION
		g_getstream_pause = TS_FALSE;
		sem_destroy(&g_switch_sem);
#endif
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VENC_SetChnAttr success, new encoder=[%d]\n", type);

	g_getstream_pause = TS_FALSE;
#ifdef FAST_SWITCH_RESOLUTION
	sem_destroy(&g_switch_sem);
#endif

	return s32Ret;
}

/******************************************************************************
* funciton : start IDR thread
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartIDR(VENC_CHN VeChn[],TS_S32 s32Cnt)
{
    TS_U32 i;

    gs_stIDRPara.bThreadStart = TS_TRUE;
    gs_stIDRPara.s32Cnt = s32Cnt;
    for(i=0; i<s32Cnt; i++)
    {
        gs_stIDRPara.VeChn[i] = VeChn[i];
    }
    return pthread_create(&gs_VencIDRPid, 0, SAMPLE_COMM_VENC_IDRProc, (TS_VOID*)&gs_stIDRPara);
}


/******************************************************************************
* funciton : start get venc svc-t stream process thread
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(TS_S32 s32Cnt)
{
    gs_stPara.bThreadStart = TS_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProc_Svc_t, (TS_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StopGetStream(void)
{
    if (TS_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop IDR process.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StopIDR(void)
{
    if (TS_TRUE == gs_stIDRPara.bThreadStart)
    {
        gs_stIDRPara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencIDRPid, 0);
    }
    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_StopSendQpmapFrame(void)
{
    if (TS_TRUE == stQpMapSendFramePara.bThreadStart)
    {
        stQpMapSendFramePara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencQpmapPid, 0);
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_StopSendFrame(void)
{
    if (TS_TRUE == stSendFramePara.bThreadStart)
    {
        stSendFramePara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencSndFrmPid, 0);
    }
    return TS_SUCCESS;
}

TS_VOID SAMPLE_COMM_VENC_ReadOneFrame( FILE* fp, TS_U8* pY, TS_U8* pU, TS_U8* pV,
                                       TS_U32 width, TS_U32 height, TS_U32 stride, TS_U32 stride2)
{
    TS_U8* pDst;
    TS_U32 u32Row;
    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        fread( pDst, width, 1, fp );
        pDst += stride;
    }
    pDst = pU;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        fread( pDst, width / 2, 1, fp );
        pDst += stride2;
    }
    pDst = pV;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        fread( pDst, width / 2, 1, fp );
        pDst += stride2;
    }
}

TS_S32 SAMPLE_COMM_VENC_PlanToSemi(TS_U8* pY, TS_S32 yStride,
                                   TS_U8* pU, TS_S32 uStride,
                                   TS_U8* pV, TS_S32 vStride,
                                   TS_S32 picWidth, TS_S32 picHeight)
{
    TS_S32 i;
    TS_U8* pTmpU, *ptu;
    TS_U8* pTmpV, *ptv;
    TS_S32 s32HafW = uStride >> 1 ;
    TS_S32 s32HafH = picHeight >> 1 ;
    TS_S32 s32Size = s32HafW * s32HafH;

    pTmpU = malloc( s32Size );
    if(pTmpU == TS_NULL)
    {
        printf("malloc buf failed\n");
        return TS_FAILURE;
    }
    ptu = pTmpU;

    pTmpV = malloc( s32Size );
    if (pTmpV == TS_NULL)
    {
        printf("malloc buf failed\n");
        free( pTmpU );
        return TS_FAILURE;
    }
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;
    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }
    free( ptu );
    free( ptv );
    return TS_SUCCESS;
}

int SAMPLE_COMM_VENC_FF_INIT(VENC_CHN chn, VENC_CHN_ATTR_S *VencChnAttr, VENC_RECV_PIC_PARAM_S *RecvParam)
{
	if (!VencChnAttr || !RecvParam) {
		SAMPLE_PRT("NULL!\n");
		return TS_FAILURE;
	}

	int ret;

	ret = TS_MPI_VENC_CreateChn(chn, VencChnAttr);
	if (TS_SUCCESS != ret) {
		SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
			chn, ret);
		return TS_FAILURE;
	}

	ret = TS_MPI_VENC_StartRecvFrame(chn, RecvParam);
	if (TS_SUCCESS != ret)
	{
		SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", ret);
		return TS_FAILURE;
	}

	return TS_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

