
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

#include "sample_comm.h"


/*****************************************************************************
* function : start vpss grp.
*****************************************************************************/
TS_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, TS_BOOL* pabChnEnable, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr)
{
    VPSS_CHN VpssChn;
    TS_S32 s32Ret;
    TS_S32 j;

    s32Ret = TS_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return TS_FAILURE;
    }

    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        SAMPLE_PRT("pabChnEnable[%d] = %d\n", j, pabChnEnable[j]);
        if(TS_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = TS_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);

            if (s32Ret != TS_SUCCESS)
            {
                SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr(grp:%d, chn:%d) failed with %#x\n", VpssGrp, j , s32Ret);
                return TS_FAILURE;
            }
			SAMPLE_PRT("TS_MPI_VPSS_SetChnAttrr(grp:%d, chn:%d) success\n", VpssGrp, j);

            s32Ret = TS_MPI_VPSS_EnableChn(VpssGrp, VpssChn);

            if (s32Ret != TS_SUCCESS)
            {
				SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, j);
                return TS_FAILURE;
            }
			SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, j);
        }
    }

	SAMPLE_PRT("is going to TS_MPI_VPSS_StartGrp [%d]\n", VpssGrp);
    s32Ret = TS_MPI_VPSS_StartGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_FF_INIT(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, pastVpssChnAttr);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr(grp:%d, chn:%d) failed with %#x\n", s32Ret, VpssGrp, VpssChn);
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VPSS_SetChnAttrr(grp:%d, chn:%d) success\n", VpssGrp, VpssChn);

	s32Ret = TS_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, VpssChn);
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, VpssChn);

	SAMPLE_PRT("is going to TS_MPI_VPSS_StartGrp [%d]\n", VpssGrp);
	s32Ret = TS_MPI_VPSS_StartGrp(VpssGrp);

	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
		return TS_FAILURE;
	}

	return TS_SUCCESS;
}


/*****************************************************************************
* function : stop vpss grp
*****************************************************************************/
TS_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, TS_BOOL* pabChnEnable)
{
    TS_S32 j;
    TS_S32 s32Ret = TS_SUCCESS;
    VPSS_CHN VpssChn;

    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        if(TS_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = TS_MPI_VPSS_DisableChn(VpssGrp, VpssChn);

            if (s32Ret != TS_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return TS_FAILURE;
            }
        }
    }

    s32Ret = TS_MPI_VPSS_StopGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VPSS_DestroyGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

/*****************************************************************************
* function : start vpss grp chn adv fun.
*****************************************************************************/
TS_S32 SAMPLE_COMM_VPSS_AdvFunStart(VPSS_GRP VpssGrp, TS_BOOL* pabChnEnable, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr)
{
    VPSS_CHN VpssChn;
    TS_S32 s32Ret;
    TS_S32 j;

    s32Ret = TS_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return TS_FAILURE;
    }

    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        if(TS_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = TS_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);

            if (s32Ret != TS_SUCCESS)
            {
                SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return TS_FAILURE;
            }
			#if 0
            /* chn crop */
            VPSS_CROP_INFO_S stCropInfo;
            stCropInfo.bEnable = TS_TRUE;
            stCropInfo.enCropCoordinate = VPSS_CROP_RATIO_COOR;
            stCropInfo.stCropRect.s32X = 240;
            stCropInfo.stCropRect.s32Y = 60;
            stCropInfo.stCropRect.u32Height =  600;
            stCropInfo.stCropRect.u32Width = 800;
            s32Ret = TS_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCropInfo);
            if (s32Ret != TS_SUCCESS) {
                SAMPLE_PRT("TS_MPI_VPSS_SetChnCrop failed with %#x\n", s32Ret);
                return TS_FAILURE;
            }

            VPSS_CROP_INFO_S stCropInfo1 = {0};
            VPSS_CROP_INFO_S *pstCropInfo = NULL;
            pstCropInfo = &stCropInfo1;
            s32Ret = TS_MPI_VPSS_GetChnCrop(VpssGrp, VpssChn, pstCropInfo);
            if (s32Ret != TS_SUCCESS) {
                SAMPLE_PRT("TS_MPI_VPSS_GetChnCrop failed with %#x\n", s32Ret);
                return TS_FAILURE;
            } else {
                SAMPLE_PRT(" chn%xenable is%d,coordinate is %d,crop info:[%d,%d,%d,%d]\n", j, pstCropInfo->bEnable, pstCropInfo->enCropCoordinate,
                pstCropInfo->stCropRect.s32X, pstCropInfo->stCropRect.s32Y, pstCropInfo->stCropRect.u32Height, pstCropInfo->stCropRect.u32Width);
            }
			#endif
            s32Ret = TS_MPI_VPSS_EnableChn(VpssGrp, VpssChn);

            if (s32Ret != TS_SUCCESS)
            {
                return TS_FAILURE;
            }
        }
    }

	SAMPLE_PRT("is going to TS_MPI_VPSS_StartGrp [%d]\n", VpssGrp);
    s32Ret = TS_MPI_VPSS_StartGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}




/*****************************************************************************
* function : start vpss grp chn extern fun.
*****************************************************************************/
TS_S32 SAMPLE_COMM_VPSS_ExtendFunStart(VPSS_GRP VpssGrp, TS_BOOL* pabChnEnable, SAMPLE_VPSS_EXTEND_ATTR_S *pstVpssExtendAttr)
{
    VPSS_CHN VpssChn;
    TS_S32 s32Ret;
    TS_S32 j;


    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        if(TS_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = TS_MPI_VPSS_DisableChn(VpssGrp, VpssChn);

            if (s32Ret != TS_SUCCESS)
            {
                return TS_FAILURE;
            }
            /* chn crop */
            if (pstVpssExtendAttr->stVpssChnCrop.bEnable) {

				#if 0
                VPSS_CROP_INFO_S stCropInfo;
                stCropInfo.bEnable = TS_TRUE;
                stCropInfo.enCropCoordinate = pstVpssExtendAttr->stVpssChnCrop.enCropCoordinate;
                stCropInfo.stCropRect.s32X = pstVpssExtendAttr->stVpssChnCrop.stCropRect.s32X;
                stCropInfo.stCropRect.s32Y = pstVpssExtendAttr->stVpssChnCrop.stCropRect.s32Y;
                stCropInfo.stCropRect.u32Height =  pstVpssExtendAttr->stVpssChnCrop.stCropRect.u32Height;
                stCropInfo.stCropRect.u32Width = pstVpssExtendAttr->stVpssChnCrop.stCropRect.u32Width;
				
                s32Ret = TS_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCropInfo);
                if (s32Ret != TS_SUCCESS) {
                    SAMPLE_PRT("TS_MPI_VPSS_SetChnCrop failed with %#x\n", s32Ret);
                    return TS_FAILURE;
                }

                VPSS_CROP_INFO_S stCropInfo1 = {0};
                VPSS_CROP_INFO_S *pstCropInfo = NULL;
                pstCropInfo = &stCropInfo1;
                s32Ret = TS_MPI_VPSS_GetChnCrop(VpssGrp, VpssChn, pstCropInfo);
                if (s32Ret != TS_SUCCESS) {
                    SAMPLE_PRT("TS_MPI_VPSS_GetChnCrop failed with %#x\n", s32Ret);
                    return TS_FAILURE;
                } else {
                    SAMPLE_PRT(" chn%xenable is%d,coordinate is %d,crop info:[%d,%d,%d,%d]\n", j, pstCropInfo->bEnable, pstCropInfo->enCropCoordinate,
                    pstCropInfo->stCropRect.s32X, pstCropInfo->stCropRect.s32Y, pstCropInfo->stCropRect.u32Height, pstCropInfo->stCropRect.u32Width);
                }
				#endif
            }

            if (pstVpssExtendAttr->stVpssChnRotation.bEnable) {


            }

            s32Ret = TS_MPI_VPSS_EnableChn(VpssGrp, VpssChn);

            if (s32Ret != TS_SUCCESS)
            {
                return TS_FAILURE;
            }
        }
    }


    if (pstVpssExtendAttr->stVpssGrpCrop.bEnable) {



    }

	// SAMPLE_PRT("is going to TS_MPI_VPSS_StartGrp [%d]\n", VpssGrp);
    // s32Ret = TS_MPI_VPSS_StartGrp(VpssGrp);

    // if (s32Ret != TS_SUCCESS)
    // {
    //     SAMPLE_PRT("TS_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
    //     return TS_FAILURE;
    // }

    return TS_SUCCESS;
}

/*****************************************************************************
* function : start vpss grp.
*****************************************************************************/
TS_S32 SAMPLE_COMM_VPSS_SwitchResolution(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 width, TS_U32 height)
{
	VPSS_CHN_ATTR_S stVpssChnAttr;
    TS_S32 s32Ret;

	s32Ret = TS_MPI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("TS_MPI_VPSS_GetChnAttr(grp:%d, chn:%d) failed with %#x!\n", VpssGrp, VpssChn, s32Ret);
		return TS_FAILURE;
	}

	stVpssChnAttr.u32Width = width;
	stVpssChnAttr.u32Height = height;

	s32Ret = TS_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr(grp:%d, chn:%d) failed with %#x!\n", VpssGrp, VpssChn, s32Ret);
		return TS_FAILURE;
	}

    return TS_SUCCESS;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
