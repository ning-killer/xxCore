#ifndef __PROCONVIDEO_H__
#define __PROCONVIDEO_H__

#include "const.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

void ProconVideoInit(int chNum, int dataNode);
ulongt ProconVideoOpen(int channel, int flag);
void ProconVideoClose(ulongt fd);
int ProconVideoWrite(ulongt fd, DATA_PIECE_T proDataInfo);
PROCON_NODE_T *ProconVideoRead(ulongt fd);
PROCON_NODE_T *ProconAlarmVideoRead(ulongt fd);
void ProconVideoFree(PROCON_NODE_T *pcpNode);
void ProconVideoReFleshReadPos(ulongt fd);
void ProconVideoReFleshReadPosBack(ulongt fd, int beforeNodeNum);
void ProconVideoDeInit();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __PROCONVIDEO_H__
