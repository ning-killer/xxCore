

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
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "sample_comm.h"





static TS_S32 s_s32SampleMemDev = -1;
#define SAMPLE_MEM_DEV_OPEN if (s_s32SampleMemDev <= 0)\
{\
    s_s32SampleMemDev = open("/dev/mem", O_RDWR|O_SYNC);\
    if (s_s32SampleMemDev < 0)\
    {\
        perror("Open dev/mem error");\
        return NULL;\
    }\
}\


TS_VOID * SAMPLE_SYS_IOMmap(TS_U64 u64PhyAddr, TS_U32 u32Size)
{
    TS_U32 u32Diff;
    TS_U64 u64PagePhy;
    TS_U8 * pPageAddr;
    TS_UL    ulPageSize;

    SAMPLE_MEM_DEV_OPEN;

    /**********************************************************
    PageSize will be 0 when u32size is 0 and u32Diff is 0,
    and then mmap will be error (error: Invalid argument)
    ***********************************************************/
    if (!u32Size)
    {
        printf("Func: %s u32Size can't be 0.\n", __FUNCTION__);
        return NULL;
    }

    /* The mmap address should align with page */
    u64PagePhy = u64PhyAddr & 0xfffffffffffff000ULL;
    u32Diff    = u64PhyAddr - u64PagePhy;

    /* The mmap size shuld be mutliples of 1024 */
    ulPageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    pPageAddr    = mmap ((void *)0, ulPageSize, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, s_s32SampleMemDev, u64PagePhy);
    if (MAP_FAILED == pPageAddr )
    {
        perror("mmap error");
        return NULL;
    }
    return (TS_VOID *) (pPageAddr + u32Diff);
}


TS_S32 SAMPLE_SYS_Munmap(TS_VOID* pVirAddr, TS_U32 u32Size)
{
    TS_U64 u64PageAddr;
    TS_U32 u32PageSize;
    TS_U32 u32Diff;

    u64PageAddr = (((TS_UL)pVirAddr) & 0xfffffffffffff000ULL);
    u32Diff     = (TS_UL)pVirAddr - u64PageAddr;
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    return munmap((TS_VOID*)(TS_UL)u64PageAddr, u32PageSize);
}



TS_S32 SAMPLE_SYS_SetReg(TS_U64 u64Addr, TS_U32 u32Value)
{
    TS_U32 *pu32RegAddr = NULL;
    TS_U32 u32MapLen = sizeof(u32Value);

    pu32RegAddr = (TS_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return TS_FAILURE;
    }

    *pu32RegAddr = u32Value;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}

TS_S32 SAMPLE_SYS_GetReg(TS_U64 u64Addr, TS_U32 *pu32Value)
{
    TS_U32 *pu32RegAddr = NULL;
    TS_U32 u32MapLen;

    if (NULL == pu32Value)
    {
        return TS_ERR_SYS_NULL_PTR;
    }

    u32MapLen = sizeof(*pu32Value);
    pu32RegAddr = (TS_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return TS_FAILURE;
    }

    *pu32Value = *pu32RegAddr;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}



/******************************************************************************
* function : get picture size(w*h), according enPicSize
******************************************************************************/
TS_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize)
{
    switch (enPicSize)
    {
        case PIC_CIF:   /* 352 * 288 */
            pstSize->u32Width  = 352;
            pstSize->u32Height = 288;
            break;

        case PIC_D1_PAL:   /* 720 * 576 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 576;
            break;

        case PIC_D1_NTSC:   /* 720 * 480 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 480;
            break;

        case PIC_360P:   /* 640 * 360 */
            pstSize->u32Width  = 640;
            pstSize->u32Height = 360;
            break;

		case PIC_720P:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;

        case PIC_1080P:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;

		case PIC_2304x1296:
			pstSize->u32Width  = 2304;
			pstSize->u32Height = 1296;
			break;

        case PIC_2560x1440:
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 1440;
            break;

        case PIC_2592x1520:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;

        case PIC_2688x1860:
            pstSize->u32Width  = 2688;
            pstSize->u32Height = 1860;
            break;

        case PIC_2592x1944:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;

        case PIC_2592x1536:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1536;
            break;

	    case PIC_2716x1524:
            pstSize->u32Width  = 2716;
            pstSize->u32Height = 1524;
            break;

        case PIC_3840x2160:
            pstSize->u32Width  = 3840;
            pstSize->u32Height = 2160;
            break;

        case PIC_3000x3000:
            pstSize->u32Width  = 3000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4000x3000:
            pstSize->u32Width  = 4000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4096x2160:
            pstSize->u32Width  = 4096;
            pstSize->u32Height = 2160;
            break;

        case PIC_7680x4320:
            pstSize->u32Width  = 7680;
            pstSize->u32Height = 4320;
            break;
        case PIC_3840x8640:
            pstSize->u32Width = 3840;
            pstSize->u32Height = 8640;
            break;
        default:
            return TS_FAILURE;
    }

    return TS_SUCCESS;
}




/******************************************************************************
* function : Set system memory location
******************************************************************************/
// TS_S32 SAMPLE_COMM_SYS_MemConfig(TS_VOID)
// {
//     TS_S32 i, j;
//     TS_S32 s32Ret = TS_SUCCESS;
//     TS_CHAR* pcMmzName = NULL;
//     MPP_CHN_S stMppChn;

//     /*config memory for vi*/
//     for (i = 0; i < VI_MAX_PIPE_NUM; i++)
//     {
//         for (j = 0; j < VI_MAX_CHN_NUM; j++)
//         {
//             stMppChn.enModId  = TS_ID_VI;
//             stMppChn.s32DevId = i;
//             stMppChn.s32ChnId = j;
//             s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//             if (s32Ret)
//             {
//                 SAMPLE_PRT("TS_MPI_SYS_SetMemConfig ERR !\n");
//                 return TS_FAILURE;
//             }
//         }
//     }

//     /*config memory for avs */
//     for (i = 0; i < AVS_MAX_GRP_NUM; i++)
//     {
//         stMppChn.enModId  = TS_ID_AVS;
//         stMppChn.s32DevId = i;
//         stMppChn.s32ChnId = 0;
//         s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//         if (s32Ret)
//         {
//             SAMPLE_PRT("TS_MPI_SYS_SetMemConfig ERR !\n");
//             return TS_FAILURE;
//         }
//     }

//     /*config memory for vpss */
//     for (i = 0; i < VPSS_MAX_GRP_NUM; i++)
//     {
//         stMppChn.enModId  = TS_ID_VPSS;
//         stMppChn.s32DevId = i;
//         stMppChn.s32ChnId = 0;
//         s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//         if (s32Ret)
//         {
//             SAMPLE_PRT("TS_MPI_SYS_SetMemConfig ERR !\n");
//             return TS_FAILURE;
//         }
//     }

//     /*config memory for venc */
//     for (i = 0; i < VENC_MAX_CHN_NUM; i++)
//     {

//         stMppChn.enModId  = TS_ID_VENC;
//         stMppChn.s32DevId = 0;
//         stMppChn.s32ChnId = i;
//         s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//         if (s32Ret)
//         {
//             SAMPLE_PRT("TS_MPI_SYS_SetMemConf ERR !\n");
//             return TS_FAILURE;
//         }
//     }

//     /*config memory for vo*/
//     for (i = 0; i < VO_MAX_LAYER_NUM; i++)
//     {
//         for (j = 0; j < VO_MAX_CHN_NUM; j++)
//         {
//             stMppChn.enModId    = TS_ID_VO;
//             stMppChn.s32DevId = i;
//             stMppChn.s32ChnId = j;
//             s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//             if (s32Ret)
//             {
//                 SAMPLE_PRT("TS_MPI_SYS_SetMemConfig ERR !\n");
//                 return TS_FAILURE;
//             }
//         }
//     }

//     /*config memory for vdec */
//     for (i = 0; i < VDEC_MAX_CHN_NUM; i++)
//     {

//         stMppChn.enModId  = TS_ID_VDEC;
//         stMppChn.s32DevId = 0;
//         stMppChn.s32ChnId = i;
//         s32Ret = TS_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

//         if (s32Ret)
//         {
//             SAMPLE_PRT("TS_MPI_SYS_SetMemConf ERR !\n");
//             return TS_FAILURE;
//         }
//     }


//     return s32Ret;
// }

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
TS_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig)
{
    TS_S32 s32Ret = TS_FAILURE;

    TS_MPI_SYS_Exit();
    TS_MPI_VB_Exit();

    if (NULL == pstVbConfig)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VB_SetConfig(pstVbConfig);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VB_SetConf failed!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VB_Init();

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VB_Init failed!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_SYS_Init();
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_Init failed!\n");
        TS_MPI_VB_Exit();
        return TS_FAILURE;
    }

    return s32Ret;
}

TS_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S* pstVbConf, TS_U32 u32SupplementConfig)
{
	return TS_SUCCESS;
}


/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
TS_VOID SAMPLE_COMM_SYS_Exit(void)
{
    /*SAMPLE_COMM_VO_Exit();*/	/* only for debug */
    TS_MPI_SYS_Exit();
    //TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    TS_MPI_VB_Exit();
    return;
}

TS_S32 SAMPLE_COMM_VB_Save2File(VIDEO_FRAME_INFO_S *pVideoFrame, TS_CHAR * pPath, TS_S32 fileIdx)
{
	TS_CHAR fileName[128];
    int  fd = -1;
//  unsigned long bytes;

	if(!pVideoFrame){
		SAMPLE_PRT("NULL ptr \n");
		return TS_FAILURE;
	}

	SAMPLE_PRT("frame info : wh=[%d,%d], stride=[%d,%d], virAddr=[%llx,%llx], [%lld,%lld]\n",
		pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height,
		pVideoFrame->stVFrame.u32Stride[0], pVideoFrame->stVFrame.u32Stride[1],
		pVideoFrame->stVFrame.u64VirAddr[0], pVideoFrame->stVFrame.u64VirAddr[1],
		pVideoFrame->stVFrame.u64VirAddr[0], pVideoFrame->stVFrame.u64VirAddr[1]);

	if(fileIdx >= 0){
		if(pPath){
			sprintf(fileName, "%s/frame_%04d_%d_%d.yuv", pPath, fileIdx,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
		else{
			sprintf(fileName, "./frame_%04d_%d_%d.yuv", fileIdx,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
	}
	else{
		if(pPath){
			sprintf(fileName, "%s/frame_%d_%d.yuv", pPath,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
		else{
			sprintf(fileName, "./frame_%d_%d.yuv",
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
	}

	fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0){
		SAMPLE_PRT("unable to create debug file.");
		return TS_FAILURE;
	}

	#if 1
	if(pVideoFrame->stVFrame.u64VirAddr){
		write(fd, &pVideoFrame->stVFrame.u64VirAddr[0],
			pVideoFrame->stVFrame.u32Width * pVideoFrame->stVFrame.u32Height * 3 / 2);
	}
	#else
	if(pVideoFrame->stVFrame.u64VirAddr){
		for(i = 0; i < pVideoFrame->stVFrame.u32Height; i ++){
			TS_VOID * pAddr = (TS_VOID *)(pVideoFrame->stVFrame.u64VirAddr[0] + i * pVideoFrame->stVFrame.u32Stride[0]);
			write(fd, pAddr, pVideoFrame->stVFrame.u32Width);
		}

		for(i = 0; i < pVideoFrame->stVFrame.u32Height / 2; i ++){
			TS_VOID * pAddr = (TS_VOID *)(pVideoFrame->stVFrame.u64VirAddr[1] + i * pVideoFrame->stVFrame.u32Stride[1]);
			write(fd, pAddr, pVideoFrame->stVFrame.u32Width);
		}
	}
	#endif

	close(fd);

	SAMPLE_PRT("save %s success\n", fileName);

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Bind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(CPM-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(CPM-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Bind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(CPM-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(CPM-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VPSS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_GRP VpssGrpDst)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrpDst;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VPSS)");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AVS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AVS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AVS-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AVS-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AVS-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AVS-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AVS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AVS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = TS_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId    = TS_ID_VO;
    stSrcChn.s32DevId   = SrcVoLayer;
    stSrcChn.s32ChnId   = SrcVoChn;

    stDestChn.enModId   = TS_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return TS_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

TS_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stDestChn;
    stDestChn.enModId   = TS_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return TS_MPI_SYS_UnBind(NULL, &stDestChn);
}


TS_S32 SAMPLE_COMM_VDEC_Bind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;


    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = TS_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_Bind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_VDEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = VdecChn;

	stDestChn.enModId  = TS_ID_CPM;
	stDestChn.s32DevId = CpmGrp;
	stDestChn.s32ChnId = CpmPipe;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-CPM)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_VDEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = VdecChn;

	stDestChn.enModId  = TS_ID_CPM;
	stDestChn.s32DevId = CpmGrp;
	stDestChn.s32ChnId = CpmPipe;

	CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-CPM)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AI_Bind_AO(AI_CHN AiChn, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AI-AO)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AI_Bind_AENC(AI_CHN AiChn, AENC_CHN AencChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AencChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AI-AENC)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ADEC_Bind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_ADEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AdChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(ADEC-AO)");

	return TS_SUCCESS;
}

char* SAMPLE_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType)
{
    if (PT_G711A == enType)
    {
        return "g711a";
    }
    else if (PT_G711U == enType)
    {
        return "g711u";
    }
    else if (PT_ADPCMA == enType)
    {
        return "adpcm";
    }
    else if (PT_G726 == enType)
    {
        return "g726";
    }
    else if (PT_LPCM == enType)
    {
        return "pcm";
    }
    else if (PT_AAC == enType)
    {
        return "aac";
    }
    else if (PT_MP3 == enType)
    {
        return "mp3";
    }
    else
    {
        return "data";
    }
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
