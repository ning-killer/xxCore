/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: defination of hi_ivp.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2018/10/26
 */
#ifndef _HI_IVP_H_
#define _HI_IVP_H_
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define HI_IVP_MAX_VENC_CHN_NUM 16
#define HI_IVP_MAX_VIPIPE_NUM 2
#define HI_IVP_MAX_CLASS 2
#define HI_IVP_CLASS_NAME_LEN 32

typedef struct{
    unsigned long long  physical_addr; /* RW;The physical address of the memory */
    unsigned long long  virtual_addr; /* RW;The virtual address of the memory */
    unsigned int  memory_size;    /* RW;The size of memory */
}hi_ivp_mem_info;

typedef struct{
    float threshold;
}hi_ivp_ctrl_attr;

typedef struct{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
}hi_ivp_rect;

typedef struct{
	hi_ivp_rect rect;
	float quality;
}hi_ivp_obj;

typedef struct{
    char class_name[HI_IVP_CLASS_NAME_LEN];
    unsigned int rect_num;
    unsigned int rect_capcity;
    hi_ivp_obj objs[20];
}hi_ivp_obj_of_one_class;

typedef struct{
    int class_num;
    hi_ivp_obj_of_one_class obj_class[HI_IVP_MAX_CLASS];
}hi_ivp_obj_array;

typedef struct{
    int enable;
    unsigned int threshold; /* RW;Range: [1,1024] */
}hi_ivp_roi_attr;

typedef enum{
    HI_IVP_ROI_MB_MODE_4X4,
    HI_IVP_ROI_MB_MODE_8X8,
    HI_IVP_ROI_MB_MODE_16X16,
    HI_IVP_ROI_MB_MODE_BUTT
}hi_ivp_roi_mb_mode;

typedef struct{
    hi_ivp_roi_mb_mode roi_mb_mode;
    unsigned int img_width; /* equal to the width of processing frame */
    unsigned int img_height; /* equal to the height of processing frame */
    unsigned char *mb_map; /* for 4x4 mb mode, alloc (DIV_UP(img_width,4))*(DIV_UP(img_height,4)) bytes */
}hi_ivp_roi_map;

/* Error Code */
typedef enum hiEN_IVP_ERR_CODE_E {
    ERR_IVP_READ_FILE      = 0x41,   /* IVP read file error */
    ERR_IVP_OPERATE_FILE   = 0x42,   /* IVP operate file error */
    ERR_IVP_PROCESS_ERR    = 0x43,
    ERR_IVP_INIT_FAIL      = 0x44,   /* IVP init fail */
    ERR_IVP_EXIT_FAIL      = 0x45,   /* IVP exit fail */
    ERR_IVP_LOAD_RESOURCE_FAIL = 0x46,
    ERR_IVP_BUTT
}EN_IVP_ERR_CODE_E;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif/*_HI_IVP_H_*/

