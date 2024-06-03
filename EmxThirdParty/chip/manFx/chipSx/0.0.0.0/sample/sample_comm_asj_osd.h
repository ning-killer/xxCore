#ifndef __SAMPLE_COMM_ASJ_OSD_H__
#define __SAMPLE_COMM_ASJ_OSD_H__

#include "ts_comm_region.h"

#define ASJ_CHN_NUM 5
#define ASJ_RGN_HDL_TIMESTAMP_RATIO 1
#define ASJ_RGN_HDL_LOGO_RATIO 2
#define ASJ_RGN_HDL_CHNNAME_RATIO 3

#define ASJ_VPSS_CHNNUM_PERGRP 3

#define ASJ_TIMESTAMP_TEMPLATE "2023-03-31 11:16:30 PM Tues"

//#include "app_comm.h"

typedef enum
{
    ASJ_CHN_OSD_CHAOS  =   0,
	ASJ_CHN_OSD_CREATED,
} ASJ_CHN_OSD_STATUS;

typedef struct tagTITLE_PARAM_CTX_S {
    RGN_HANDLE channel;
    unsigned int   width;//chn w, not osd w
    unsigned int   height;//chn h, not osd h
}TITLE_PARAM_CTX_S;

//#include "ovd_param.h"
#define OSD_TEXT_COUNT  (8)
#define OSD_CHN_0    0

typedef enum
{
    OVD_1DMODE  =   0, //低清
    OVD_SDMODE  =   1, //标清
    OVD_HDMODE  =   2, //高清
    OVD_FHDMODE  =  3, //超高清
} OVDEncodeQuality;

typedef enum {
	OVD_RES_TYPE_640X360,
	OVD_RES_TYPE_1280X720,
	OVD_RES_TYPE_1920X1280,
	OVD_RES_TYPE_2304X1296,
	OVD_RES_TYPE_2560X1440,
	OVD_RES_TYPE_960X544,
} OVD_RES_TYPE_E;

typedef struct {
	OVD_RES_TYPE_E		type;
	OVDEncodeQuality	quality;
	unsigned int		fps;
	unsigned int		interval;
	unsigned int		bitrate;
	unsigned int		width;
	unsigned int		height;
	unsigned int		font;
	unsigned int		time_x;
	unsigned int		time_y;
	unsigned int		logo_x;
	unsigned int		logo_y;
} OVD_RES_ATTR_T;

//#include "ovd_base_info.h"
//可选 OSD字体大小设置
typedef enum {
    OSDFONT_VENDOR_DEF = 0,
    OSDFONT_SMALL = 1,
    OSDFONT_MEDIUM = 2,
    OSDFONT_BIG = 3,
} ovd_osd_font_size;

//可选 OSD字体颜色设置
typedef struct {
    int red;
    int green;
    int blue;
} ovd_osd_color;

//可选 OSD设置坐标
typedef struct {
    int x;
    int y;
} ovd_osd_pos;

typedef enum {
	hleft = 0,
	hright = 1,
	hcenter = 2,
} osd_halign_t;

typedef enum {
	vtop = 0,
	vbottom = 1,
	vcenter = 2,
} osd_valign_t;

typedef struct {
    int on;
    int mode;
    int id;//RGN_HANDLE
    char text[128]; //<必填，可读可写, 字符串类型：OSD文本内容，若为空串，则当前没有OSD显示>
    ovd_osd_font_size osdfont; //<必填，可读可写，整型：字号大小，1为小字号，2为大字号>
    ovd_osd_pos osdpos;
    ovd_osd_color color;
} osd_text_t;

typedef struct {
    int osd_count;/*在配置文件中设置osd组数*/
	osd_halign_t osdhoralign;
	osd_valign_t osdveralign;
    osd_text_t osd_text[OSD_TEXT_COUNT];
} ovd_osd_text_t;

typedef struct {
	int id;//RGN_HANDLE
    int on;
    ovd_osd_pos osdpos;
	osd_halign_t osdhoralign;
	unsigned char DateMode;/*日期格式，0为MM-DD-YY，1为YY-MM-DD*/
	unsigned char TimeMode;/*时间格式，0为12小时制，1为24小时制*/
	int refresh;
} ovd_osd_timestamp_t;

typedef struct {
	int id;//RGN_HANDLE
    int on;
    ovd_osd_pos osdpos;
	osd_halign_t osdhoralign;
} ovd_osd_logo_t;

typedef struct {
	ASJ_CHN_OSD_STATUS status;
	int have_inited;

	TITLE_PARAM_CTX_S stPatam;
	pthread_mutex_t osd_mutex_lock;
	unsigned char* globalTimeBitmapBuf;

	ovd_osd_timestamp_t timestamp;
	ovd_osd_logo_t logo;
	ovd_osd_text_t text;
} ovd_osd_chn_t;

typedef struct {
	ovd_osd_chn_t chn[ASJ_CHN_NUM];
} ovd_osd_t;

TS_S32 osdDeinit(int chn);
TS_S32 osdInit(int chn,const char *imagePath,const char *fontpath);
TS_S32 osdRestartInit(int chn, const char *imagepath, const char *fontpath);
TS_S32 SAMPLE_REGION_Close_One_Rgn(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn);
TS_S32 APP_COMM_VENC_Open_OSD_TimeStamp(int chn, TS_BOOL bCreateThread,const char *fontPath);
TS_S32 APP_COMM_VENC_Open_OSD_Text(int chn);
TS_S32 APP_COMM_VENC_Open_OSD_Logo(int chn,const char *imagePath);
TS_S32 APP_COMM_VENC_Close_OSD_TimeStamp(int chn);
TS_S32 APP_COMM_VENC_Stop_OSD_Text(int chn,int i);
TS_S32 APP_COMM_VENC_Close_OSD_Logo(int chn);

#endif /* End of #ifndef __LOAD_BMP_H__*/
