/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_NIR_IQ_API_ID_H_
#define _MI_NIR_IQ_API_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NIR_ID_API_COMMON_BASE (0x0100)
#define NIR_ID_API_COMMON_END  (0x01FF)

#define NIR_ID_API_IQ_BASE (0x1000)
#define NIR_ID_API_IQ_END  (0x10FF)

//================================================================
//  ID Defined : NIR COMMON  API
//================================================================
#define NIR_ID_API_COMMON_DEVICE_ID  (NIR_ID_API_COMMON_BASE + 1)
#define NIR_ID_API_COMMON_CHANNEL_ID (NIR_ID_API_COMMON_BASE + 2)

//================================================================
//  ID Defined : NIR IQ  API
//================================================================
#define NIR_ID_API_BLENDING_SATURATION (NIR_ID_API_IQ_BASE + 1)
#define NIR_ID_API_CONTRAST            (NIR_ID_API_IQ_BASE + 2)
#define NIR_ID_API_SATURATION          (NIR_ID_API_IQ_BASE + 3)
#define NIR_ID_API_WEIGHT              (NIR_ID_API_IQ_BASE + 4)

#ifdef __cplusplus
} // end of extern C
#endif

#endif
