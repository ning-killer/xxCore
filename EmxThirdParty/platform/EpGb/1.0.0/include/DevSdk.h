#pragma once
#include "SdkDef.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
* @brief 获取sdk版本号
*/

const char* sdk_version();

/*
* @brief 国标配置初始化(内存)
*/
int gb_config_init(const char* memConfig);
/*
* @brief 国标配置初始化
*/
int gb_config_initEx(gb_platform_info platInfo);

int set_sdk_cb(DevCallback c);

int gb_start();

int gb_stop();

/*
* @brief 报警通知
* @param aType 0-移动告警 1-区域入侵
* @param reservered 当为区域入侵的时候 1-进入 2-离开
*/
int gb_alarm_notify(int aType, int reserved);

/*
* @brief 向国标添加一路流 最大3路流
* 
* @param streamnumber 流编号 0，1，2.... 默认0主码流
* @param vi 0-h264,1-h265
* @param ai 2-aac 3-pcma
*/
int add_stream(int streamnumber, int vi, int ai);

/*
* @brief 给添加的流输入媒体数据
* 
* @param streamnumber 流编号
* @param data 媒体数据
* @param size 数据大小
* @param pts 时间戳 单位ms
* @param keyFrame 关键帧 0-非关键帧，1-关键帧
* @param t 0-视频, 1-音频
*/
void input_data_on_stream(int streamnumber, const char* data, uint32_t size, uint32_t pts, uint8_t keyFrame, uint8_t t);

#ifdef __cplusplus
}
#endif /* __cplusplus */