

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
#include <sys/prctl.h>

#include "sample_comm.h"

#define MAX_SENSOR_NUM      2
#define ISP_MAX_DEV_NUM     4

//static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    15,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_MIPI_2M_30FPS_WDR2TO1_LINE =
{
	{0, 0, 1920, 1080},
	{1920, 1080},
	30,
	BAYER_RGGB,
	WDR_MODE_2To1_LINE,
	0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_X86_LOGITECH_HD1080 =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    15,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_X86_LOGITECH_HD720 =
{
    {0, 0, 1280, 720},
    {1280, 720},
    15,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_HAPS_ARMISP =
{
    {0, 0, 1280, 720},
    {1280, 720},
    15,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_EVB_SENSOR =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
#if defined(TX5215)
	30,
#elif defined(TX5239DV200)
	30,
#else
    30,
#endif
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_EVB_SENSOR_4K =
{
    {0, 0, 3840, 2160},
    {3840, 2160},
    30,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_EVB_SENSOR_4MP =
{
    {0, 0, 2560, 1440},
    {2560, 1440},
#if defined(TX5215)
	30,
#elif defined(TX5239DV200)
	30,
#else
    30,
#endif
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_EVB_SENSOR_5MP =
{
    {0, 0, 2688, 1860},
    {2688, 1860},
    30,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};


TS_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S* pstPubAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case X86_LOGITECH_HD1080:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_X86_LOGITECH_HD1080, sizeof(ISP_PUB_ATTR_S));
            break;
        case X86_LOGITECH_HD720:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_X86_LOGITECH_HD720, sizeof(ISP_PUB_ATTR_S));
            break;
		case HAPS_ARMISP:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_HAPS_ARMISP, sizeof(ISP_PUB_ATTR_S));
			break;
		case EVB_SENSOR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_EVB_SENSOR, sizeof(ISP_PUB_ATTR_S));
			break;
		case EVB_SENSOR_4K:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_EVB_SENSOR_4K, sizeof(ISP_PUB_ATTR_S));
			break;
		case EVB_SENSOR_4MP:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_EVB_SENSOR_4MP, sizeof(ISP_PUB_ATTR_S));
			break;
		case EVB_SENSOR_5MP:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_EVB_SENSOR_5MP, sizeof(ISP_PUB_ATTR_S));
			break;

        default:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
    }

    return TS_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
TS_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev)
{
    TS_S32 s32Ret = 0;
    return s32Ret;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
TS_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev)
{
#if 0
    if (g_IspPid[IspDev])
    {
        TS_MPI_ISP_Exit(IspDev);
        pthread_join(g_IspPid[IspDev], NULL);
        SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
        g_IspPid[IspDev] = 0;
    }
#endif
    return;
}

TS_VOID SAMPLE_COMM_All_ISP_Stop(TS_VOID)
{
    ISP_DEV IspDev;

    for (IspDev = 0; IspDev < ISP_MAX_PIPE_NUM; IspDev++)
    {
        SAMPLE_COMM_ISP_Stop(IspDev);
    }
}

TS_S32 SAMPLE_COMM_ISP_SetParamIndex(VI_PIPE ViPipe, TS_U32 paramSetIndex)
{
	return TS_MPI_ISP_SetParamIndex(ViPipe, paramSetIndex);
}

TS_S32 SAMPLE_COMM_ISP_GetParamIndex(VI_PIPE ViPipe, TS_U32 *paramSetIndex)
{
	return TS_MPI_ISP_GetParamIndex(ViPipe, paramSetIndex);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

