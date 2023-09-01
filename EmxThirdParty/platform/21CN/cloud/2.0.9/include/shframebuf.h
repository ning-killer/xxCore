#ifndef __SHFRAMEBUF_H__
#define __SHFRAMEBUF_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

typedef void*  p_shmem_t;

//支持的图像格式
typedef enum enum_CLOUDCAMERA_VIDEO_TYPE
{
	EN_CLOUDCAMERA_TYPE_NV12          = 0x00,//NV12图像格式, 12bpp
    EN_CLOUDCAMERA_TYPE_RGB888        = 0x01,//RGB888图像格式, 24bpp
}EN_CLOUDCAMERA_VIDEO_TYPE;

//支持的音频格式
typedef enum enum_CLOUDCAMERA_AUDIO_TYPE
{
	EN_CLOUDCAMERA_AUDIO_PCM16          = 0x01,
    EN_CLOUDCAMERA_AUDIO_G711A          = 0x02,
    EN_CLOUDCAMERA_AUDIO_G711U          = 0x04,
    EN_CLOUDCAMERA_AUDIO_AAC            = 0x08,
    EN_CLOUDCAMERA_AUDIO_MP3            = 0x10,
}EN_CLOUDCAMERA_AUDIO_TYPE;

//传输数据的属性，其中音频为保留项
typedef struct shframe_attr
{
    int                         av_type;//音频还是视频,0视频,1音频
    int                         av_fmt;//编码格式
    union
    {
        struct
        {
            int                 width;
            int                 height;
            int                 bpp;//bits per pixel
        }video;
        struct 
        {
            int                 sample_rate;
            int                 channel;
            int                 bits_per_sample;
        }audio;
    };
}shframe_attr_t;

//实际的数据信息
typedef struct shframe_info
{
    int                         size;//数据大小
    void                       *addr;//数据内容  
    struct timeval              tm;//当前系统时间
    long long                   ts;//系统启动后经过的时间
}shframe_info_t;

//跟踪框的颜色枚举 
typedef enum {
    TRACK_OSD_RED, /* red */ 
    TRACK_OSD_BLACK, /* black */ 
    TRACK_OSD_GREEN, /* green */ 
    TRACK_OSD_YELLOW, /* yellow */ 
}TrackColor; 

//实时跟踪框数据
typedef struct shtrack_info{
    int id;//从0开始递增
    int x;//左上角点的横坐标
    int y; //左上角点的纵坐标
    int width; 
    int height; 
    int color;//线框下有效，取值参考TrackColor，如果芯片支持其他颜色，可按rgba格式绘制 
    int linewidth;
}shtrack_info_t; 


typedef struct tagshtrack_mgr {
    int num;//实际的数据类型 
    int type;//0为线框，1为模糊框 
    shtrack_info_t track_arr[50]; 
}shtrack_mgr_t; 

// 创建共享内存接口
// attr :共享数据的属性
// depth : 视频帧的缓存深度
p_shmem_t shframe_buf_create(int key, shframe_attr_t* attr, int depth);
int shframe_buf_write(p_shmem_t writer, shframe_info_t *frame, const int timeout);
int shframe_buf_close(p_shmem_t writer);


p_shmem_t shframe_reader_get(int key);
int shframe_buf_get_attr(p_shmem_t reader, shframe_attr_t *attr);
int shframe_reader_get_frame(p_shmem_t reader, shframe_info_t* frame, const int timeout);
int shframe_reader_release_frame(p_shmem_t reader, shframe_info_t* frame);
int shframe_reader_release(p_shmem_t reader);

#ifdef __cplusplus
}
#endif    

#endif