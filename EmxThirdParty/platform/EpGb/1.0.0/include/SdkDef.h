#pragma once
#include <stdint.h>
#include <time.h>

/*国标平台接入信息*/
typedef struct _gb_platform_info
{
	char serverIp[64];
	uint16_t serverPort[64];
	char serverDomain[21];
	char deviceId[21];
	char channelId[8][21];//最大8通道
	char password[16];
	char protocol[4];//udp or tcp
	int regExpires;//默认3600s
	int heartbeatInterval;//默认60s
	int heartbeatCount;//默认3次
}gb_platform_info;

typedef struct _gb_deviceInfo
{
	char manufacturer[256];//厂商
	char model[256]; //型号
	char firmware[256];//固件版本
}gb_deviceInfo;

typedef struct _rec_info
{
	time_t startTime;
	time_t endTime;
	char filePath[128];//只支持mp4文件
}rec_info;

/*
* 卡存媒体信息
*/
typedef struct _playback_media {
	void* opaque;//用户自定义数据
	int vi;//视频编码类型 0-h264,1-h265
	int ai;//音频编码类型 2-aac 3-pcma
	int fps;//帧率
}playback_media;

/*
* @brief 云台操作
*/
typedef struct _ptz_info
{
	char channelId[21];//通道ID
	/*
	* 操作命令
	* 0x00-ptz停止
	* 0x01-向右,0x02-向左,0x04-向下,0x08-向上
	* 0x0A-左上,0x06-左下,0x09-右上,0x05-右下
	* 0x10-放大,0x20-缩小
	*
	* 0x48-缩小光圈,0x44-放大光圈,0x42-聚焦近,0x41-聚焦远,0x40-fi动作
	* 0x81-设置预设点,0x82-调用预设点,0x83-删除预设点
	*/
	uint8_t cmd;
	union ControlValue
	{
		uint8_t ptz[3];//ptz[0]-水平方向速度,ptz[1]-垂直方向速度,ptz[2]-镜头变倍速度
		uint8_t fi[3];//fi[1]-操作光圈速度，fi[0]-聚焦速度
		uint8_t preset[3];//preset[1]-预置位数
	};
	ControlValue val;
}PTZInfo;
/*
*
* @brief 平台注册回调
*
* @param statusCode 返回状态值
* @param statusText 返回状态描述
*
*/
typedef void (*on_platform_register)(int statusCode, const char* statusText);

/*
* @brief 设备信息查询
* @param out d
*/
typedef void(*on_deviceinfo_query)(gb_deviceInfo& d);

/*
* @brief 录像查询
*
* @param info 查询参数
* @param out 返回录像列表，必须使用new为每项分配内存，由sdk回收内存
* @param recSize 录像列表大小
* @return 0-succ，1-error
*/
typedef void (*on_record_query)(time_t startTime, time_t stopTime, rec_info** out, int& recSize);

typedef void (*on_ptz)(PTZInfo& info);

/*
* 设备重启
*/
typedef void (*on_boot)();

/*
* 对讲开始：设备准备接收音频数据
*/
typedef void(*on_start_voice)();

/*
* 对讲结束：设备不再接收音频数据
*/
typedef void(*on_stop_voice)();

/*
* 对讲音频数据 g711a
*/
typedef void(*on_voice_data)(const uint8_t* data, uint32_t size, uint32_t pts);

/*
* sdk日志回调
*/
typedef void(*on_sdk_log)(const char* fmt, ...);

/*
* @brief 根据时间端打开回放流
* 
* @return 回放流信息  NULL-打开回放流异常
*/
typedef playback_media* (*on_start_playback)(time_t startTime, time_t stopTime, int reserved);

/*
* @brief 回放拖动
*/
typedef void(*on_seek_playback)(playback_media* m, uint32_t ms);

/*
* @brief 获取一帧回放数据
*
* @param m 回放流上下文
* @param frame 回放媒体数据
* @param size 媒体数据大小
* @param pts 时间戳 单位ms
* @param isKey 是否是关键帧
* @param t 0-视频 1-音频
*/
typedef int (*on_read_playback_data)(playback_media* m, uint8_t** frame, uint32_t& size, uint32_t& pts, uint8_t& isKey, uint8_t& t);

/*
* @brief 关闭回放流 用户回收资源
*/
typedef void(*on_stop_playback)(playback_media* m);

struct DevCallback
{
	on_platform_register register_cb;

	on_boot boot_cb;

	on_record_query record_query_cb;

	on_deviceinfo_query deviceinfo_cb;

	on_ptz ptz_cb;
	/*对讲*/
	on_start_voice start_voice_cb;

	on_voice_data voice_cb;

	on_stop_voice stop_voice_cb;
	/*日志*/
	on_sdk_log sdk_log_cb;
	/*回放*/
	on_seek_playback seek_playback_cb;

	on_start_playback start_playback_cb;

	on_read_playback_data read_playback_data_cb;

	on_stop_playback stop_playback_cb;
};