/********************************************************************************
 **********Copyright (C), 2022, Beijing Tsing Micro Technology  Co., Ltd.***********
 ********************************************************************************
 * @file     	 : common.h
 * @brief   	 : 公共函数接口
 * @author       : Tsing Micro
 * @date         : 2022-11-27
********************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
#if __cplusplus
				extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
#include <stdbool.h>
#define     STARTCODE 	        0xa815aa55

#ifndef false
#define false	0
#endif

#ifndef true
#define true	1
#endif

#define alignTo(n,align) (((n) + (align) - 1) & ~((align) - 1))

typedef struct _VSTREAMHEAD//32Byte
{
    unsigned int        startcode;  //  0xa815aa55
    char                type;       //0:I frame 1:p frame 3:jpeg  6:audio
    char                streamid;   //0:h264_video  1:audio 2:h265_video 3:jpeg
    unsigned short      militime;
    unsigned int        sectime;   //暂时拿来存储时间戳
    unsigned int        frameno;
    unsigned int        len;
    unsigned char       version; //0:实时流 1:回放流 2:下载录像文件 3:下载图片文件
    unsigned char       resolution;//0:640*360 1:320*240 3:1080*720 4:1920*1080  5:640*480 6:1280*720 7:2304*1296 8:2560*1440 9:960*544
    unsigned long long  utctime;   //utc时间，单位ms
    unsigned char       res[2];
} STREAMHEAD, *PSTREAMHEAD;

typedef struct DEVICEINFO
{
    char sn[32];
    char mac[32];
    char passwd[64];
}device_info_t;

int threadCreate(pthread_t *pt_id,void * proc,void * arg);
int execCmd(const char *cmd);
int getLocalMac(const char *eth_inf, char *mac);
int getLocalIp(const char *eth_inf, char *ip);
int checkProcStatus(char * cp_process_name);
int execCmdChstrExist(char* cmd, char* str);
int execCmdEx(const char *cmd, char* res, int max);
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd,  void *buf, size_t count);
int isFileExist(const char* file_path);
int isDirExist(const char* dir_path);
unsigned int getTickMs(void);
unsigned long long getUtcTime(void);
void sleepMs(unsigned int msec);
void cleanCached(void);
bool checkVersionValid(char *version);
//int alignTo(int n, int align);
const char *get_device_sn();
const char *get_device_passwd();
unsigned int safe_convert_long2int(long long value);
int wdtInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
