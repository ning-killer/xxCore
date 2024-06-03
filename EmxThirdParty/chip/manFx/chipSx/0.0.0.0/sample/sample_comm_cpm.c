
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

TS_S32 SAMPLE_COMM_CPM_Bind_ViCpmVenc(TS_U32 u32PipeNum, AVS_GRP CPMGrp, AVS_CHN CPMChn, VI_CHN ViChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
   // VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

#if 0
    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }
#endif

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);

    for (i = 0; i < u32PipeNum; i++)
    {
        CPMPipe = i;
        ViPipe = i;
        SAMPLE_PRT("SAMPLE_COMM_VI_Bind_CPM ViPipe=%d, ViChn=%d,CPMGrp=%d, CPMPipe=%d\n", ViPipe, ViChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VI_Bind_CPM(ViPipe, ViChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("AVS bind VO fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("to call SAMPLE_COMM_CPM_Bind_VENC\n");
    s32Ret = SAMPLE_COMM_CPM_Bind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VPSSs bind VENC fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    // if (benChn1)
    // {
    //     CPMChn = 1;
    // }
    // else
    // {
    //     CPMChn = 0;
    // }

#if 0
    s32Ret = SAMPLE_COMM_AVS_Bind_VO(CPMGrp, CPMChn, VoLayer, VoChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AVS bind VO fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
#endif
    SAMPLE_PRT("SAMPLE_CPM_Bind exit!\n");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_CPM_Bind_ViCpm(TS_U32 u32PipeNum, AVS_GRP CPMGrp, AVS_CHN CPMChn, VI_CHN ViChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
   // VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);

    for (i = 0; i < u32PipeNum; i++)
    {
        CPMPipe = i;
        ViPipe = i;
        SAMPLE_PRT("SAMPLE_COMM_VI_Bind_CPM ViPipe=%d, ViChn=%d,CPMGrp=%d, CPMPipe=%d\n", ViPipe, ViChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VI_Bind_CPM(ViPipe, ViChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("sample cpm bind exit!\n");

    return TS_SUCCESS;
}




TS_S32 SAMPLE_COMM_CPM_UnBind_ViCpmVenc(TS_U32 u32PipeNum, CPM_GRP CPMGrp, CPM_CHN CPMChn, VI_CHN ViChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp  = 0;
    AVS_PIPE CPMPipe;
    // AVS_CHN  CPMChn  = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn   = 0;
    TS_S32   s32Ret  = TS_SUCCESS;
    // VO_LAYER VoLayer = 0;
    // VO_CHN   VoChn   = 0;
    TS_U32   i;
    // VPSS_GRP VpssGrp = 0;
    // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;

    s32Ret = SAMPLE_COMM_CPM_UnBind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AVS unbind VENC fail with %#x", s32Ret);
        return TS_FAILURE;
    }

#if 0
    s32Ret = SAMPLE_COMM_AVS_UnBind_VO(VoLayer, VoChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AVS unbind VO fail with %#x", s32Ret);
        return TS_FAILURE;
    }

    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI unbind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }
#endif

    for (i = 0; i < u32PipeNum; i++)
    {
        CPMPipe = i;
        ViPipe = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_CPM(ViPipe, ViChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS unbind AVS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_ViCpm(TS_U32 u32PipeNum, AVS_GRP CPMGrp, AVS_CHN CPMChn, VI_CHN ViChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
   // VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;


    for (i = 0; i < u32PipeNum; i++)
    {
        CPMPipe = i;
        ViPipe = i;
        SAMPLE_PRT("SAMPLE_COMM_VI_UnBind_CPM ViPipe=%d, ViChn=%d,CPMGrp=%d, CPMPipe=%d\n", ViPipe, ViChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VI_UnBind_CPM(ViPipe, ViChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("sample cpm bind exit!\n");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Bind_ViVpssCpmVenc(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);


    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        // VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_Bind_CPM\n");
        CPMPipe = 0;
        SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS bind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }

    SAMPLE_PRT("to call SAMPLE_COMM_CPM_Bind_VENC\n");
    s32Ret = SAMPLE_COMM_CPM_Bind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM bind VENC fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("sample cpm bind exit!\n");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_CPM_Bind_ViVpssCpm(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);

    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        // VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_Bind_CPM\n");
    CPMPipe = 0;
    SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
    s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VPSS bind CPM fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("sample cpm bind exit!\n");

    return TS_SUCCESS;
}





TS_S32 SAMPLE_COMM_CPM_UnBind_ViVpssCpmVenc(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);

    SAMPLE_PRT("to call SAMPLE_COMM_CPM_UnBind_VENC\n");
    s32Ret = SAMPLE_COMM_CPM_UnBind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM unbind VENC fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }


    CPMPipe = 0;
    SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
    s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VPSS unbind CPM fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("to call SAMPLE_COMM_VI_UnBind_VPSS\n");
    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        // VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI unbind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }

    SAMPLE_PRT("sample cpm unbind exit!\n");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_CPM_UnBind_ViVpssCpm(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
    TS_U32 i;

    /**vi --> cpm -->venc**/
    SAMPLE_PRT("Enter u32PipeNum = %d\n", u32PipeNum);


    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_UnBind_CPM\n");
        CPMPipe = 0;
        SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS unbind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }



    for (i = 0; i < u32PipeNum; i++)
    {
        ViPipe = i;
        // VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI unbind VPSS fail with %#x", s32Ret);
            return TS_FAILURE;
        }
    }


    SAMPLE_PRT("sample cpm unbind exit!\n");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_CPM_Bind_VpssCpmVenc(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    //VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;

    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_Bind_CPM\n");
        CPMPipe = 0;
        SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS bind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }

    SAMPLE_PRT("to call SAMPLE_COMM_CPM_Bind_VENC\n");
    s32Ret = SAMPLE_COMM_CPM_Bind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM bind VENC fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("sample bind exit!\n");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VpssCpmVenc(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    //VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;

    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_UnBind_CPM\n");
    CPMPipe = 0;
    SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
    s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VPSS Unbind CPM fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("to call SAMPLE_COMM_CPM_UnBind_VENC\n");
    s32Ret = SAMPLE_COMM_CPM_UnBind_VENC(CPMGrp, CPMChn, VencChn);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM unbind VENC fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("sample cpm unbind exit!\n");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_CPM_Bind_VpssCpm(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    //VI_PIPE  ViPipe;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;
   // TS_U32 i;

    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_Bind_CPM\n");
    CPMPipe = 0;
    SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
    s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VPSS bind CPM fail with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    SAMPLE_PRT("sample cpm bind exit!\n");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VpssCpm(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp)
{
    // AVS_GRP  CPMGrp = 0;
    AVS_PIPE CPMPipe = 0;
    // AVS_CHN  CPMChn = 0;
    // VI_CHN   ViChn = 0;
    TS_S32   s32Ret = TS_SUCCESS;
//    VPSS_GRP VpssGrp = 0;
   // VPSS_CHN VpssChn = 0;
    // VENC_CHN VencChn = 0;
   // VO_LAYER VoLayer = 0;
   // VO_CHN   VoChn = 0;


    SAMPLE_PRT("to call SAMPLE_COMM_VPSS_UnBind_CPM\n");
        CPMPipe = 0;
        SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM VpssGrp=%d, VPSSChn=%d,CPMGrp=%d, CPMPipe=%d\n", VpssGrp, VpssChn, CPMGrp, CPMPipe);
        s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(VpssGrp, VpssChn, CPMGrp, CPMPipe);

        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS unbind CPM fail with %#x!\n", s32Ret);
            return TS_FAILURE;
        }

    SAMPLE_PRT("sample cpm unbind exit!\n");

    return TS_SUCCESS;
}


#if 0
static TS_S32 SAMPLE_COMM_CPM_Init(TS_VOID **pHandle)
{

    SAMPLE_PRT("Error:omit register cpm init()!\n");
    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_CPM_Exit(TS_VOID *pHandle)
{

    SAMPLE_PRT("Error:omit register cpm exit()!\n");
    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_CPM_Process(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{

    SAMPLE_PRT("Error:omit register cpm process()!\n");
    return TS_SUCCESS;
}
#endif


static TS_S32 SAMPLE_COMM_CPM_ALGO_SetParam(TS_VOID *pHandle, TS_VOID *pParam)
{

    SAMPLE_PRT("Warning:omit register cpm set_param()!\n");
    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_CPM_ALGO_GetParam(TS_VOID *pHandle, TS_VOID *pParam)
{

    SAMPLE_PRT("Warning:omit register cpm get_param()!\n");
    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_CPM_ALGO_GetResult(TS_VOID *pHandle, TS_VOID *pResult)
{

    SAMPLE_PRT("Warning:omit register cpm get_result()!\n");
    return TS_SUCCESS;
}


static TS_S32 SAMPLE_COMM_CPM_ALGO_ReleaseResult(TS_VOID *pHandle, TS_VOID *pResult)
{

    SAMPLE_PRT("Warning:omit register cpm release_result()!\n");
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Start(CPM_ADAP_S *pCPMAdap)
{
    // TS_U32         u32LUTSize;
    // AVS_GRP_ATTR_S stCPMGrpAttr = {0};
    // AVS_CHN        CPMChn = 0;
    // AVS_CHN_ATTR_S stChnAttr = {0};
    // TS_VOID*       pLUTVirAddr = NULL;
    TS_S32         s32Ret = TS_SUCCESS;
    // TS_U32         u32PipeNum;
    TS_U32         i = 0;
    CPM_GRP_ATTR_S  stCPMGrpAttr;
    CPM_Handle_S    stCPMHandle;

    if (pCPMAdap->pCPMAdapGrpAttr.u32ChnNum >= CPM_MAX_CHN_NUM) {
        SAMPLE_PRT("outnumber CPM_MAX_CHN_NUM !\n");
        goto exit;
    }

    if (pCPMAdap->pCPMAdapHandle.s32PipeNum >= CPM_MAX_PIPE_NUM) {
        SAMPLE_PRT("outnumber CPM_MAX_PIPE_NUM !\n");
        goto exit;
    }

    if (pCPMAdap->pCPMAdapHandle.init == NULL) {
        SAMPLE_PRT("Error:you must register cpm init()!\n");
        goto exit;
    }

    if (pCPMAdap->pCPMAdapHandle.process == NULL) {
        SAMPLE_PRT("Error:you must register cpm process()!\n");
        goto exit;
    }

    if (pCPMAdap->pCPMAdapHandle.exit == NULL) {
        SAMPLE_PRT("Error:you must register cpm exit()!\n");
        goto exit;
    }

    memset(&stCPMGrpAttr, 0, sizeof(CPM_GRP_ATTR_S));

    stCPMGrpAttr.u32GrpId        = pCPMAdap->pCPMAdapGrpAttr.u32GrpId;
    stCPMGrpAttr.u32PipeNum      = pCPMAdap->pCPMAdapGrpAttr.u32PipeNum;
    stCPMGrpAttr.u32ChnNum       = pCPMAdap->pCPMAdapGrpAttr.u32ChnNum;
    stCPMGrpAttr.u32Interval     = pCPMAdap->pCPMAdapGrpAttr.u32Interval;
    for (i = 0; i < stCPMGrpAttr.u32ChnNum ; i++) {
        stCPMGrpAttr.stChnAttr[i].u32Width      = pCPMAdap->pCPMAdapGrpAttr.stChnAttr[i].u32Width;
        stCPMGrpAttr.stChnAttr[i].u32Height     = pCPMAdap->pCPMAdapGrpAttr.stChnAttr[i].u32Height;
        stCPMGrpAttr.stChnAttr[i].enPixelFormat = pCPMAdap->pCPMAdapGrpAttr.stChnAttr[i].enPixelFormat;
        stCPMGrpAttr.stChnAttr[i].u32Depth      = pCPMAdap->pCPMAdapGrpAttr.stChnAttr[i].u32Depth;
    }

    s32Ret = TS_MPI_CPM_CreateGrp(stCPMGrpAttr.u32GrpId, &stCPMGrpAttr);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Creat grp failed with %#x!\n", s32Ret);
        goto exit;
    }

    /*register algorithm*/
    memset(&stCPMHandle, 0, sizeof(CPM_Handle_S));
    stCPMHandle.s32PipeNum     = pCPMAdap->pCPMAdapHandle.s32PipeNum;

    for (i = 0; i < pCPMAdap->pCPMAdapHandle.s32PipeNum; i++) {
        stCPMHandle.stPipeAttr[i].u32Width      = pCPMAdap->pCPMAdapHandle.stPipeAttr[i].u32Width;
        stCPMHandle.stPipeAttr[i].u32Height     = pCPMAdap->pCPMAdapHandle.stPipeAttr[i].u32Height;
        stCPMHandle.stPipeAttr[i].enPixelFormat = pCPMAdap->pCPMAdapHandle.stPipeAttr[i].enPixelFormat;
    }
    stCPMHandle.init           = pCPMAdap->pCPMAdapHandle.init;
    stCPMHandle.exit           = pCPMAdap->pCPMAdapHandle.exit;
    stCPMHandle.process        = pCPMAdap->pCPMAdapHandle.process;

    if (pCPMAdap->pCPMAdapHandle.set_param)
        stCPMHandle.set_param      = pCPMAdap->pCPMAdapHandle.set_param;
    else
        stCPMHandle.set_param      = SAMPLE_COMM_CPM_ALGO_SetParam;

    if (pCPMAdap->pCPMAdapHandle.get_param)
        stCPMHandle.get_param      = pCPMAdap->pCPMAdapHandle.get_param;
    else
        stCPMHandle.get_param      = SAMPLE_COMM_CPM_ALGO_GetParam;

    if (pCPMAdap->pCPMAdapHandle.get_result)
        stCPMHandle.get_result      = pCPMAdap->pCPMAdapHandle.get_result;
    else
        stCPMHandle.get_result      = SAMPLE_COMM_CPM_ALGO_GetResult;

    if (pCPMAdap->pCPMAdapHandle.release_result)
        stCPMHandle.release_result      = pCPMAdap->pCPMAdapHandle.release_result;
    else
        stCPMHandle.release_result      = SAMPLE_COMM_CPM_ALGO_ReleaseResult;

    s32Ret = TS_MPI_CPM_Register(stCPMGrpAttr.u32GrpId, &stCPMHandle);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_CPM_Register failed with %#x!\n", s32Ret);
        goto exit;
    }

    return TS_SUCCESS;

exit:
    TS_MPI_CPM_DestroyGrp(stCPMGrpAttr.u32GrpId);


    return TS_FAILURE;

}
TS_S32 SAMPLE_COMM_CPM_Active(CPM_GRP CPMGrp)
{

    TS_S32 s32Ret = TS_SUCCESS;
    /* active cpm comp */
    s32Ret = TS_MPI_CPM_StartGrp(CPMGrp);
    if (TS_SUCCESS != s32Ret)
    {
       SAMPLE_PRT("CPM start grp failed with %#x!\n", s32Ret);
       return TS_FAILURE;
    }

    return TS_SUCCESS;

}

TS_S32 SAMPLE_COMM_CPM_Stop(CPM_GRP CPMGrp)
{
    // AVS_CHN CPMChn = 0;
    TS_S32  s32Ret = TS_SUCCESS;

    s32Ret = TS_MPI_CPM_StopGrp(CPMGrp);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("StopGrp failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_CPM_Unregister(CPMGrp);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM_Unregister failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_CPM_DestroyGrp(CPMGrp);// TS_MPI_CPM_DestroyGrp

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("DestoryGrp failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_GetResult(CPM_GRP CPMGrp, TS_VOID *pResult)
{

    return TS_MPI_CPM_GetResult(CPMGrp, &pResult);
}

TS_S32 SAMPLE_COMM_CPM_SetParam(CPM_GRP CPMGrp, TS_VOID *pParam)
{

    return TS_MPI_CPM_SetParam(CPMGrp, pParam);
}

TS_S32 SAMPLE_COMM_CPM_GetParam(CPM_GRP CPMGrp, TS_VOID *pParam)
{

    return TS_MPI_CPM_GetParam(CPMGrp, pParam);
}

TS_S32 SAMPLE_COMM_CPM_SetGrpAttr(CPM_GRP CPMGrp, CPM_GRP_ATTR_S *pstGrpAttr)
{

    return TS_MPI_CPM_SetGrpAttr(CPMGrp, pstGrpAttr);
}

TS_S32 SAMPLE_COMM_CPM_GetGrpAttr(CPM_GRP CPMGrp, CPM_GRP_ATTR_S *pstGrpAttr)
{

    return TS_MPI_CPM_GetGrpAttr(CPMGrp, pstGrpAttr);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
