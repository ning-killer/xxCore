// PHZ
// 2018-5-16

#ifndef GB_RTSP_APP_H
#define GB_RTSP_APP_H
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

//streamType: 0: h265  1: h264
typedef enum tsRTSP_TYPE_E
{
    RTSP_STREAM_TYPE_H265 = 0,
    RTSP_STREAM_TYPE_H264,
    RTSP_STREAM_TYPE_BUFF
} RTSP_STREAM_TYPE_E;

int gb_rtsp_server_start(RTSP_STREAM_TYPE_E streamType);
int sendToRtspVencFrame(unsigned char *buffer, unsigned int size, unsigned int type, unsigned long long timestamp);
int gb_rtsp_server_stop(void);
int gb_rtsp_get_client_num(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
