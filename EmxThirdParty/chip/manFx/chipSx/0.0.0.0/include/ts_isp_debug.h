/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_isp_debug.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */


#ifndef __TS_ISP_DEBUG_H__
#define __TS_ISP_DEBUG_H__

#include "ts_debug.h"

#define ISP_EMERG_TRACE(fmt, ...)\
    do {\
		TS_EMERG_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_ALERT_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_ALERT_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_CRIT_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_CRIT_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_ERR_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_ERR_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_WARN_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_WARN_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_NOTICE_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_NOTICE_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_INFO_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_INFO_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_DEBUG_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
		TS_DEBUG_TRACE(TS_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   (((a) == 0) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))

#endif     /* __TS_ISP_DEBUG_H__ */
