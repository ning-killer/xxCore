/*************************************************************
 *
 * This is a part of the OVD SDK.
 * Copyright:2021 OVD
 * File name:OVD_OpenAPI.h
 * Version:1.19.0
 * All rights reserved.
 *
 *************************************************************/

#ifndef OVDOPENAPI_OPENAPI_H
#define OVDOPENAPI_OPENAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <stdarg.h>
#include "OVD_define.h"



typedef struct
{

    /**
    *【回调】获取设备信息
    *
    * @description：OVC获取OVD设备信息时调用，返回相应的OVDDeviceInfo信息。若当前设备为有线设备，则wifi相关字段置空即可。
    *
    * @param[out] deviceInfo        需要设备返回的信息，详细见结构体OVDDeviceInfo
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetOVDDeviceInfo)(OVDDeviceInfo *deviceInfo);

    /**
    *【回调】获取远程配置信息
    *
    * @description：OVC获取OVD远程配置信息时调用
    *
    * @param[out] output_ovdconfig  字符串，厂商负责分配内存malloc,由设备sdk负责释放
    * @param[out] output_size       字符串长度
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetOVDConfigureInfo)(OVD_char **output_ovdconfig, OVD_int32 *output_size);

    /**
    *【回调】设置远程配置信息
    *
    * @description：OVC下发修改参数，OVD启用相应配置的参数。没有携带的信息，设备保持原参数配置。
    *
    * @param[in] in_ovdconfig      需要设置的信息，json格式信息
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVC_SetOVDConfigureInfo)(OVD_char *in_ovdconfig);

    /**
    *【回调】连接服务器结果回调
    *
    * @description：SDK与OVC之间的连接状态发生变化时（连接上/连接断开），通过此接口通知OVD。
    *               OVD根据状态的通知，决定相应异步处理。
    *
    * @others: 当前信令平台鉴权是在OVD与OVC连接成功后才开始鉴权，故会出现先OVD_OVCConnectStatus 反馈连接成功，后因鉴权失败导致反馈连接失败。
    *
    * @param[in] connectStatus      0:连接成功;     -1:连接失败;     1:重连中
    */
    OVD_void(*OVD_OVCConnectStatus)(OVD_int32 connectStatus);

    /**
    *【回调】重启通道(NVR设备)
    *
    * @description：当用户通过APP远程控制channel重启时，调用此接口;若OVD不支持单channel重启，那么就重启设备
    *
    * @param[in] channel     需要重启的channel
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */

    OVD_int32(*OVD_ReBootChannel)(OVD_int32 channel);

    /**
    *【回调】重启设备
    *
    * @description：当用户通过APP远程控制设备重启时，调用此接口
    *
    * @others：
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_ReBootDevice)();

    /**
    *【回调】恢复默认设置
    *
    * @description：当OVC决定设备恢复出厂配置时，调用此接口。
    *               设备收到该请求后，应该将所有配置恢复到出厂状态（包括wifi配置），但不能断开当前网络连接，并返回成功应答。
    *               OVC稍后会再下发一个重启指令将设备重启，默认配置生效。
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_ResetConfiguration)();

    /**
    *【回调】卡录像查询RecordSearch
    *
    * @description：OVC可以通过该指令后去获取指定时间区间的卡录像记录
    *
    * @others：RecordSearch和RecordSeek非强关联，两者互不干扰
    *
    * @param[in] channel               通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] starttime             录像查询的起始时间戳(单位为秒,绝对时间戳)，当天0点
    * @param[in] endtime               录像查询的结束时间戳(单位为秒,绝对时间戳)，当前时间
    * @param[in] page                  查询的页面索引，若一次查询区间不够，sdk会回调第二次
    * @param[in] numinpage             该页面最大的区间数量
    * @param[out] fileInPage           厂商回填区间信息
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_DMEAPI_callback_RecordSearch)(OVD_int32 channel, OVD_uint64 starttime, OVD_uint64 endtime, OVD_int32 page,
            OVD_int32 numInPage, OVD_DMERecordFileListPerPage *fileinpage);

    /**
    *【回调】卡录像打开RecordOpen
    *
    * @description：通知厂商打开一条某一channel下的卡录像会话。
    *
    * @param[in] channel             通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    *
    * @return
    * 成功: 返回卡录像会话厂商侧的上下文（context）标识，ctx，会话句柄非单个文件句柄
    * 失败: NULL
    */
    OVD_void *(*OVD_DMEAPI_callback_RecordOpen)(OVD_int32 channel);

    /**
    *【回调】卡录像播放RecordSeek
    *
    * @description：该调用后，sdk下一次读取音视频数据帧时，厂商需从相近时间点的I帧（优先向前）开始返回数据。若对应时间点没有录像数据则返回错误。
    *
    * @param[in] ctx                厂商卡录像会话上下文标识，为RecordOpen返回的ctx
    * @param[in] timestamp          绝对时间戳，seek的绝对时间点,单位为秒
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_DMEAPI_callback_RecordSeek)(OVD_void *ctx, OVD_int64 timestamp);

    /**
    *【回调】卡录像数据读取RecordReadFrame
    *
    * @description：pframe_info 结构体内存由sdk分配, 由sdk 负责释放。
    *               pframe_info里的frame_buf 由厂商分配，也由厂商释放;建议厂商在下次read或者会话结束释放内存。
    *
    * @param[in] ctx               厂商卡录像会话上下文标识
    * @param[in] pframe_info：    帧数据信息
    *
    * @return
    * OVD_RET_COMMON_ERROR = -1,    //通用错误码
    * OVD_RET_SUCCESS = 0,          //成功
    * OVD_RET_READ_FRAME_RETRY=201, //读帧重试错误码
    * OVD_RET_READ_FRAME_EOF=202,   //卡录像读取完毕，厂商反馈EOF
    *
    * 若收到通用错误，则sdk会调用销毁卡录像会话资源机制
    */
    OVD_int32(*OVD_DMEAPI_callback_RecordReadFrame)(OVD_void *ctx, OVD_FrameInfo *pframe_info);

    /**
    *【回调】卡录像会话关闭RecordClose
    *
    * @param[in] ctx         厂商卡录像会话上下文
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_DMEAPI_callback_RecordClose)(OVD_void *ctx);

    /**
    *【回调】触发升级
    *
    * @description：OVD对比要升级的固件版本号和当前自身的固件版本号，若版本号不一致，那么到相应的URL去下载升级固件，升级过程厂商异步处理。
    *
    * @param[in] firmware_model            要升级的固件的版本号
    * @param[in] upgradeURL                升级固件的远程url，由设备主动去下载、升级
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_FirmwareUpgrade)(OVD_char *firmware_model, OVD_char *upgradeURL);

    /**
    *【回调】升级状态查询
    *
    * @description：OVC下发升级版本的命令后，会周期性的查询OVD的升级状态及进度，OVD根据当天升级状态及进度，反馈相应的值。
    *
    * @others：升级详情见文档升级流程图
    *
    * @param[out] upgradeStatus         升级状态，详细见枚举值OVDUpgradeStatus
    * @param[out] upgradeProgress       升级进度，整数值 0-100。设备主要升级过程为下载和安装，下载时反馈进度0~100，下载完开始安装后进度重置为0~100
    * @param[out] version               当前系统版本，version所占内存由sdk分配
    * @param[in] version_len            sdk分配version内存的最大长度
    * @param[out] last_upgrade_time     字符串，最近一次处理升级指令的时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,sdk 已分配内存，对应的字段信息需要厂商固化
    * @param[in] time_len               sdk分配的last_upgrade_time的最大长度
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_QueryFirmwareUpgradeStatus)(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress, OVD_char *version,
            OVD_int32 version_len, OVD_char *last_upgrade_time, OVD_int32 time_len);

    /**
    *【回调】时间同步
    *
    * @description：OVC每天0天下发，OVD根据传入的时间与系统时间的偏差，决定是否根据下发的参数修改时间
    *
    * @param[in] datetime      必填，字符串；格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32
    * @param[in] tz            可选，整数：时区数，东为正数，西为负数。例如东八区为+8，默认为+8
    * @param[in] offset        必填，整数；可接受的偏差，单位秒，若摄像机时间与上面给定的时间的偏差在offset秒之内，则摄像机无需同步时间
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_SyncTime)(OVD_char *datetime, OVD_int32 tz, OVD_int32 offset);

    /**
    *【回调】时间获取（目前未启用）
    *
    * @description：OVC需要获取设备时间时调用，OVD查询本地时间，反馈参数
    *
    * @param[out] datetime      必填，字符串；格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32
    * @param[in] tz             可选，整数：时区数，东为正数，西为负数。例如东八区为+8，默认为+8
    * @param[in] offset         必填，整数；可接受的偏差，单位秒，若摄像机时间与上面给定的时间的偏差在offset秒之内，则摄像机无需同步时间
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */

    OVD_int32(*OVD_QueryTime)(OVD_char *out_datetime, OVD_int32 time_len, OVD_int32 *tz);

    /**
    *【回调】云台控制
    *
    * @description：APP端控制OVD进行转动时，调用此接口
    *
    * @param[in] channel            通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] ptzcmd             控制命令,详细可见枚举类型OVCPTZControlCmd
    * @param[in] ptzvalue           参考下图
    *
    * | op | 操作 | value |
    * | --- | --- | --- |
    * | up | 上 | 可选，整数，速度，0-100，0最慢，100最快，默认50 |
    * | down | 下 | 同上 |
    * | left | 左 | 同上 |
    * | right | 右 | 同上 |
    * | upleft | 左上 | 同上 |
    * | upright | 右上 | 同上 |
    * | downleft | 左下 | 同上 |
    * | downright | 右下 | 同上 |
    * | zoomin | 拉近 | 同上 |
    * | zoomout | 拉远 | 同上 |
    * | stop | 停止 | 可选，整数，但数值没有意义 |
    * | goto_preset | 跳转预置位 | 预置位ID， 0-255 |
    * | set_preset | 设置预置位 | 同上 |
    * | clear_preset | 清除预置位 | 同上 |
    * | up_step | 单步上 | 可选，整数，单步步长，0-100，0最小，100最大，默认0 |
    * | down_step | 单步下 | 同上 |
    * | left_step | 单步左 | 同上 |
    * | right_step | 单步右 | 同上 |
    * | upleft_step | 单步左上 | 同上 |
    * | upright_step | 单步右上 | 同上 |
    * | downleft_step | 单步左下 | 同上 |
    * | downright_step | 单步右下 | 同上 |
    * | zoomin_step | 单步拉近 | 同上 |
    * | zoomout_step | 单步拉远 | 同上 |
    * | focusdown    | 焦距调小 | 同上 |
    * | focusup      | 焦距调大 | 同上 |
    * | zoom_reset   | 恢复默认倍率 | 同上 |
    * | focusdown_setp | 焦距单步调小 | 同上 |
    * | focusup_step   | 焦距单步调大 | 同上 |
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_PTZCmd)(OVD_int32 channel, OVCPTZControlCmd ptzcmd, OVD_int32 ptzvalue);

    /**
    *【回调】EBO机器人指令控制
    *
    * @description：APP端控制OVD进行转动时，调用此接口
    *
    * @param[in] channel            通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] ptzcmd             控制命令,详细可见枚举类型OVCPTZControlCmd
    * @param[in] ptzvalue           参考下图
    *
    * | op                  | 操作     |value|
    * | ------------------  | -------- | --- |
    * | OVC_EBO_MV_STEP     | 步进指令 |  1  |
    * | OVC_EBO_MV_CONT     | 持续移动 |  2  |
    * | OVC_EBO_RESERVE     | 预留指令 |  3  |
    * | OVC_EBO_RESERVE2    | 预留指令 |  4  |
    * | OVC_EBO_MV_FORWARD  | 向前猛冲 |  5  |
    * | OVC_EBO_RECHARGE    | 回充     |  6  |
    * | OVC_EBO_CIRCLE      | 转圈     |  7  |
    * | OVC_EBO_SHAKE       | 抖动     |  8  |
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_EBOCmd)(OVCEBOControlCmd ebocmd, void *data);

    /**
    *【回调】获取预置位
    *
    * @description：OVC下发命令，OVD返回前期配置的预置点信息
    *
    * @param[in] channel                通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[out] presetList            返回设备当前已设置的预置点列表，非数组索引,sdk已分配内存
    * @param[in] array_malloc_num       sdk内部分配的预置点数组大小
    * @param[out] count                 预置点实际个数
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetPresetList)(OVD_int32 channel, OVD_int32 *presetList, OVD_int32 array_malloc_num, OVD_int32 *count);

    /**
    *【回调】强制I帧
    *
    * @description：SDK在收到上传视频等命令时，为了服务器/客户端能够最短时间内播放视频，调用此接口在100ms内设备底层编码出一个I帧喂入
    *
    * @param[in] channel     通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_ForceIFrame)(OVD_int32 channel);

    /**
    *【回调】封面截图
    *
    * @description：OVD截取channel上的当前图片，反馈截图内容及信息。若截图大小大于maxImageSize，则返回-2并在OVDImageInfo.size中带回所需要图片大小，之后会再调用一次。
    *
    * @param[in] channel               通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[out] ImageInfo            截取的图片信息
    *
    * @return
    *  成功：0
    *  截图失败：-1
    *  空间不足：-2,  maxImageSize空间不足，并在OVDImageInfo.size中带回所需要图片大小
    */
    OVD_int32(*OVD_Snapshot)(OVD_int32 channel, OVDImageInfo *imageInfo, OVD_int32 maxImageSize);

    /*
     * 调用时机：OVC通知OVD播放音乐/音频时调用,暂不启用
     * 功能介绍：OVD去url下载音乐内容，并在channel的音箱上播放
     *
    **参数说明:
    **    [in]channel:      通道号
    **    [in]url:          歌曲/音频下载的url
    **    [in]repeat:       播放次数，默认为1次，有效值为1~10
    **    [in]volume        播放音量，默认用当前音量，有效值为0~100
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32(*OVD_SetAudioOutPlay)(OVD_int32 channel, OVD_char *url, int repeat, int volume);

    /**
    *【回调】从ovd设备中获取时间戳函数，精确度毫秒， 暂不启用
    *
    * @param[out] out_time
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */    //
    OVD_int32(*OVD_gettime)(OVD_uint64 *out_time);

    /**
    *【回调】设置设备时间
    *
    * @description：首次连接或者重新调度时设置设备时间
    *
    * @param[in] input_time            设置的时间戳，单位为毫秒
    * @param[in] tolerance_value       设置的容忍值，单位为秒,即当前系统时间戳和预设置的时间相差小于容忍值，则不需要设置系统时间
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_settime)(OVD_uint64 input_time, OVD_int32 tolerance_value);

    /**
    *【回调】设备状态获取
    *
    * @description：回调时机为服务启动初期，sdk回调厂商接口
    *
    * @param[out] out_simpleovd          sdk已分配内存 channel_count不能少于1
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_getsimpleovdinfo)(SimpleOVDinfo *out_simpleovdinfo);

    /**
    *【回调】卡状态查询
    *
    * @param[out] out_state            必填，整数；sd卡状态，0：正常，1：未插卡，2：卡异常,3格式化中
    * @param[out] out_total            必填，整数, sd卡总容量，单位为MB
    * @param[out] out_free             必填, 整数, sd卡剩余容量，单位为MB
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetDiskInfo)(OVD_int32 *out_state, OVD_int32 *out_total, OVD_int32 *out_free);

    /**
    *【回调】OVD格式化SD卡
    *
    * @description：OVC需要格式化SD卡时调用，厂商需异步处理
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_SetSDCardFormat)();

    /**
    *【回调】同步软探针配置项
    *
    * @description：返回结果为softprobe_on的值，返回1sdk会开启软探针模块，返回0则不开启软探针模块。默认开启。
    *
    * @return
    * - = 0:  不开软探针模块
    * - = 1:  开启软探针模块
    * - < 0:  失败
    */
    OVD_int32(*OVD_ProbeByval)();

    /**
    *【回调】日志上传
    *
    * @description：OVC可以通过该命令控制OVD上传相关本地日志，供调试分析使用。
    *               OVD收到该指令后，通过异步方式将指定时间范围日志上传到相应的URL。
    *               若指定范围的日志已经滚动删除，则使用尽力原则上传剩余的日志。
    *               若指定范围没有任何日志记录，应上传一个长度为0的日志文件。
    *               日志文件通过PUT方法上传。
    *
    * @param[out] trans_id         必填，字符串；标记此日志上传的任务ID
    * @param[out] start            必填，字符串；日志记录开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32
    * @param[out] end              必填，字符串；日志记录结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32
    * @param[out] url              必填，字符串；日志上传的URL，设备通过PUT方法上传相应的日志文件
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_LogUploadAsync)(OVD_char *trans_id, OVD_char *start, OVD_char *end, OVD_char *url);

    /**
    *【回调】平台扩展指令
    *
    * @description：第三方平台指令可通过此回调下发,2022/8/15,此功能废弃，使用固话原来对接方式
    *
    * @others：目前支持和家智话平台长连接ccrtc
    *
    * @param[in] method                信令里的method字段
    * @param[in] in_data               信令里带的method_param字段
    * @param[out] out_response         第3方回调返回的字符串内容
    * @param[in] in_outresponse_len    预分配的内存字符串长度，目前预分配1024(包括\0)，厂商不能超过1000长度
    *
    * @return
    * 0：成功
    * -1： 通用设备错误
    * -2： 标识mehod字段 厂商不支持
    * -3：参数不合法或无效
    */
    OVD_int32(*OVD_extension_callback)(char *method, char *in_data, char *out_response, OVD_int32 in_outresponse_len,
                                       OVD_int32 *out_outresponse_len);

    /**
    *【回调】消除告警接口
    *
    * @description：消除指定类型告警，若type为0，则消除所有当前告警
    *
    * @param[in] alarmtype  指定告警类型
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_StopAlarm)(OVD_int32 alarmtype);

    /**
    *【回调】低功耗设备休眠控制
    * 1. OVD在没活动的情况下自动关闭主芯片电路，并启动网络接口（卡/板）的休眠功能。
    * 2. 网络接口（卡/板）与OVH（休眠服务器）的休眠服务IP地址/端口建立TCP长连接。
    * 3. 网络接口（卡/板）每60秒向OVH休眠服务发送一个心跳报文。
    * 4. 若收到OVH发出的唤醒报文，则停止网络接口（卡/板）的休眠功能，启动主芯片电路，正常接入OVC其余服务。
    *
    * @description：OVC可以通过该方法阻止设备（及指定通道）在指定时间内休眠，或者唤醒设备（及指定通道）当前所有休眠的部件。
    *               OVD收到此指令后，应该在指定的过期时间内，保证设备（及指定通道）能够完全正常上电工作。收到此命令expired秒后，由设备自行决定是否进入休眠状态。
    *
    * @param[in] channel               需要保持不休眠的channel
    * @param[in] notAllowHibernate     是否允许进入休眠状态，0：允许进入休眠，进入休眠时间根据expired参数确定；1：不允许进入休眠，此种情况下，不管expired设置为多少，设备都要保持上电状态，不休眠，直至收到下一个休眠指令
    * @param[in] expired               从收到命令起，到expired的时间内，保持不休眠
    * @param[in] reason                设置不休眠的原因，详见枚举值定义OVDHibernateReason
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_KeepAwakenUtilExpired)(OVD_int32 channel, OVD_int32 notAllowHibernate, OVD_int32 expired, OVDHibernateReason reason);

    /* p2p相关接口目的是兼容老设备，新设备无需对接 */

    /**
    *【回调】(p2p接口)OVD根据入参的配置，查询SD卡中的文件列表，并返回文件列表信息
    *
    * @description：OVC查看OVD上SD卡的录像文件信息列表时，调用此接口
    *
    * @param[in] channelmask1              需要查询的通道mask，对应低位号的通道         （弃用)
    * @param[in] channelmask2              需要查询的通道mask，对应高位号的通道 弃用
    * @param[in] recordTypemask            文件类型（第0位：视频文件 第1位：告警文件） 弃用
    * @param[in] startStamp                录像查询的起始时间戳(单位为秒)
    * @param[in] endStamp                  录像查询的结束时间戳(单位为秒)
    * @param[in] page                      查询的页码
    * @param[in] numInPage                 页的条数
    * @param[out] fileInPage               录像文件列表信息，详细可见结构体描述OVDRecordFileListPerPage
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    *
    * @others 假设查询的录像文件数有200个，numInPage=10，则最大页码Page为20。若传进的参数numInPage=10，page=2，则fileInPage应该返回第10个到第20个录像的信息;若传进的参数numInPage=10，page=21，则fileInPage返回空录像信息(录像个数为0)
    */
    OVD_int32(*OVD_QueryRecordPage)(OVD_uint32 channelmask1, OVD_uint32 channelmask2, OVD_uint32 recordType, OVD_uint64 StartStamp,
                                    OVD_uint64 EndStamp, OVD_int32 Page, OVD_int32 numInPage, OVDRecordFileListPerPage *FilePage);

    /**
    *【回调】(p2p接口)OVD根据入参的文件名称，读取SD卡上相应文件的音视频数据参数信息及录像时长，反馈到出参
    *
    * @description：OVC根据查询到的SD卡文件列表信息，决定查看一个文件详细信息，调用此接口
    *
    * @param[in] channel               通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] recordname            录像文件名称
    * @param[out] videoInfo            视频信息,详细可见结构体描述OVDVideoDataFormat
    * @param[out] audioInfo            音频信息,详细可见结构体描述OVDAudioDataFormat
    * @param[out] fileTotalTime        该录像的时长(秒)
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_OpenRecordFile)(OVD_int32 channel, OVD_char *recordname, OVDVideoDataFormat *videoInfo,
                                   OVDAudioDataFormat *audioInfo, OVD_int32 *fileTotalTime);

    /**
    *【回调】(p2p接口)OVD根据控制信息，控制文件的内容是否通过OVD_SendRecordAVContent上传文件内容
    *
    * @description：OVC根据查询到的SD卡文件列表信息，控制相应音视频文件的播放、停止、暂停等信息
    *
    * @param[in] channel                通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] controlType            播放控制，详细可见枚举类型OVDCONTROLTYPE
    * @param[in] value                  额外值，目前只有视频拖动时会用到，代表要跳至的视频时间戳(ms)
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_RecordCotrol)(OVD_int32 channel, OVDCONTROLTYPE controlType, OVD_int32 value);

    /**
    *【回调】(p2p接口)OVD打开OVD对讲功能
    *
    * @description：APP端发起对讲时，调用此接口打开OVD对讲
    *
    * @param[in] channel               通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] andioFormat           对讲音频数据的格式信息，见结构体定义OVDAudioOutDataFormat
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_AudioPlayStart)(OVD_int32 channel, OVDAudioOutDataFormat andioFormat);

    /**
    *【回调】(p2p接口)OVD接收数据，并在音箱播放出来
    *
    * @description：OVC把对讲的音频内容，传输给OVD时调用
    *
    * @param[in] channel        通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] buf            音频数据指针
    * @param[in] size           音频数据大小
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_AudioPlayProGress)(OVD_int32 channel, OVD_uchar *buf, OVD_int32 size);

    /**
    *【回调】(p2p接口)OVD关闭对讲功能
    *
    * @description：APP关闭对讲时通知OVD
    *
    * @param[in] channel            通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_AudioPlayStop)(OVD_int32 channel);

    /**
    *【回调】(p2p接口)切换视频清晰度，已弃用
    *
    * @description：OVD根据配置切换到相应的视频清晰度，并返回切换后的视频参数
    *
    * @param[in] channel            通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
    * @param[in] quality            要设置的清晰度，详细参考枚举值OVDEncodeQuality
    * @param[out] vedioInfo         清晰度调整后的视频参数，详细参考结构体OVDVideoDataFormat
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_VedioSwitchQuality)(OVD_int32 channel, OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo);

    /**
    *【回调】(p2p接口)获取SD卡信息，已弃用
    *
    * @description：OVD查询本地SD卡相关信息（OVDSDInfo定义的内容），反馈信息
    *
    * @param[out] sdInfo            存储卡信息,详细可见结构体描述OVDSDInfo
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetSDInfo)(OVDSDInfo *sdInfo);

    /**
    *【回调】获取设备信息，目前用于软探针
    *
    * @param[in] info_e         获取信息类型,详细可见结构体描述OVD_GetDevRunningInfo_e,注意ping和traceroute，不支持回调返回101 ;服务器url不可达，回调返回102
    * @param[out] data          相应信息类型返回数据,使用可见demo,字符串类型长度为64
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_GetDevRunningInfo)(OVD_GetDevRunningInfo_e info_e, void *data);


     /**
     * 语音播放回调函数定义
     * @param [in] cmd  命令类型
     * @param [in] data 回调数据指针，根据不同的回调类型，转成不同的数据结构进行处理
     * @param [in] user_data 用户数据指针
     * @return 返回函数执行结果
     * - DOT_EC_SUCCESS：成功
     * - 其他值：失败
     */
    OVD_int32 (*OVD_AudioOpenDev)(OVD_int32 channel,OVDAudioDataFormat* st);
    OVD_int32 (*OVD_AudioCloseDev)(OVD_int32 channel);
    OVD_int32 (*OVD_AudioPlayData)(OVD_int32 channel,OVD_int64 timestamp, int flags, unsigned char* buff, int len);

    

    /**
    *【回调】设置云存套餐信息
    *
    * @param[in] cloudStatusType    云存状态，开启或者关闭
    * @param[in] info_e             云存套餐信息，详细可见结构体描述OVD_SetCloudInfo_e
    *
    * @return
    * - = 0:  成功
    * - < 0:  失败
    */
    OVD_int32(*OVD_SetCloudStatus)(OVD_bool cloudStatusType, OVD_SetCloudInfo_e info_e);


    

} OVD_CallBackFunList;


/**
 * 功能：设备能力集初始化
 */

/**
 * 能力集初始化
 *
 * @description：将入参capinfo进行默认值初始化。其中设置了默认值的参数有 have_sd:1; video_quality:"sd,hd"; video_formats_supportlists:"h264"
 *
 * @others: 能力集修改后需要变更固件版本号才可生效
 *          能力集详情参考doc目录里cap.txt文件
 *
 * @param[in] capinfo   厂商设备能力集
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_CapInit(OVDCapInfo *capinfo);

/**
 * SDK初始化
 *
 * @param[in] capinfo           设备能力集
 * @param[in] clientParam       设备连接服务器信息
 * @param[in] logParam          日志配置信息
 * @param[in] callBackFunList   响应服务器的回调函数
 * @param[in] jsonParam         json格式化后的字符串，提供动态的参数配置。
 *  json格式信息参考如下：
 *  {
 *      "snapshotSize":<可选，整数： 单位为B，缩略图ovd sdk预分配的内存大小，若厂商不设置，则默认为500KB>
 *      "buffDuration":<可选，整数： 单位为秒，设备从上电到连接平台时，需要sdk缓存的音视频内容的时长，若不设置，则为0秒，主要针对于低功耗唤醒设备，正常非休眠设备，不必启用>
 *      "debug_ts":<可选，布尔型，调试开关，若开启，则设备sdk日志输出传入音视频每一帧的时间戳>
 *
 *      "cseg_maxcount":<可选，整型， 指定云存模块队列缓存最大切片数量，默认值为3个，默认值由平台指令下发>
 *      "snddropdelay":<可选，整型，单位为毫秒，指定底层流媒体srt连接最大缓存时延，默认值为10000 毫秒>
 *   }
 *
 * @return
 *  OVD_RET_SUCCESS：       0  初始化成功
 *  OVD_RET_BADPARAMETER： 1  入参校验失败
 *  OVD_RET_COMMON_ERROR：-1  通用错误，初始化失败
 */
extern OVD_int32 OVD_Init(OVDCapInfo *capinfo, OVDClientParam *clientParam, OVDLogParam *logParam,
                          OVD_CallBackFunList *callBackFunList, OVD_char *jsonParam);


/**
 * 功能：设备获取SDK版本号
 */

/**
 * 获取SDK版本号信息
 *
 * @param [out] verison 设备版本号
 * @param [in] version_len 调用者预分配的字符串长度
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_GetSDKVersion(char *version, int version_len);


/**
 * 功能：设备配网
 * 说明：
 *  （1）设备wifi配网机制：设备通过识别声波或扫描APP二维码获取wifi配置信息及app账号对应的bindid, 设备通过wifi配置信息进行联网，同时将bindid上报给sdk内部（不必等连接网络成功）
 *  （2）设备有线配网机制： app扫描设备二维码，获取设备信息，然后app上报平台，平台进行设备与app账号绑定，无需设备sdk参与。
 */

/* wifi声波配网  */

/**
 * 声波初始化
 *
 * @description：设备检测到无网络配置信息，判断若使用声波配网，则调用此接口，传入声波参数
 *
 * @param[in] sampleRate        采样率
 * @param[in] bitWidth          位宽(8/16bit)
 *
 * @return
 * 成功：返回声波句柄
 * 失败：NULL
 */
extern OVD_void *OVD_SoundWaveInit(OVD_int32 sampleRate, OVD_int32 bitWidth);

//识别开始回调函数
typedef OVD_void(*RecognizerStart)();
//识别结束回调函数
typedef OVD_void(*RecognizerFinish)(OVD_int32 type, OVD_void *info, OVD_int32 infoLen);

/**
 * 声波识别开始
 *
 * @description：设备开始声波配网后，获取到音频文件，发送到SDK识别；声波识别模块识别出数据后，调用回调end_cbfunc返回给上层
 *
 * @param[in] recognizer    声波句柄
 * @param[in] starFunc      识别开始回调函数；回调函数定义为 void (*RecognizStart)(void);
 * @param[in] endFunc       识别结束回调函数（此函数返回wifi信息）；回调函数定义为 void (*RecognizerFinish)(struct OVDWiFiInfo info);
 *
 * @others: 因为受现实环境影响可能传入的音频信息有误，会导致识别出的声波数据异常，所以在end_cbfunc中如果识别到的数据不合预期，
 *          建议用户继续发送声波，厂商继续喂入声波数据，直到反馈的识别结果数据符合预期（即类型为OVD_XX_SSID_WIFI，即包含wifi配置和bindid)
 *
 * @return
 * - =  0 : 成功
 * - <  0 : 失败
 * - = 101: sdk未集成声波识别模块
 */
extern OVD_int32 OVD_SoundWaveStart(OVD_void *recognizer, RecognizerStart starFunc, RecognizerFinish endFunc);

/**
 * 传入声波数据
 *
 * @description：设备开始声波配网后，把采集到声波数据传入声波模块的句柄
 *
 * @param[in] recognizer    声波句柄
 * @param[in] data          声波数据的首字节指针
 * @param[in] len           声波数据长度
 *
 * @return
 * - 成功:  写入的数据长度
 * - 失败:  < 0
 */
extern OVD_int32 OVD_SoundWaveWriteData(OVD_void *recognizer, const OVD_void *data, OVD_uint64 len);

/**
 * 停止声波识别
 *
 * @description：识别完后，调用此接口,回收资源
 *
 * @param[in] recognizer    声波句柄
 *
 * @return
 * - =  0 : 成功
 * - <  0 : 失败
 * - = 101: sdk未集成声波识别模块
 */
extern OVD_int32 OVD_SoundWaveStop(OVD_void *recognizer);


/* wifi二维码配网  */

/**
 * 二维码配网初始化
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_QRString_init();

/**
 * 二维码配网传入数据
 *
 * @description：设备读取字符串,返回解析结果
 *
 * @param[out] info     厂商分配内存的指针，识别成功后，识别出的数据会存入这个内存
 * @param[in] qr_str    具体一张二维码图片厂商解析出的字符串,包含'/0'
 * @param[in] qr_len    具体一张二维码图片厂商解析出的字符串长度,strlen的长度
 *
 * @return
    OVD_RET_RPC_UNIMPLEMENT  101  未集成字符串解析模块
    OVD_RET_BADPARAMETER       1  入参错误
    OVD_RET_COMMON_ERROR      -1  本次输入的字符串无法解析或者最终解析到数据有误，此时厂商可继续输入数据继续识别。
    OVD_NETCONF_SUCCESS        7  识别成功
    OVD_NETCONF_LESS_DATA      9  配网缺失数据，数据太长，二维码分页，需要厂商再次输入
 */
extern OVD_int32 OVD_QRString(OVDXXSSIDWiFiInfo *info, OVD_uchar *qr_str, OVD_uint32 qr_len);

/* AP配网  */
/**
 * AP配网开始
 *
 * @description：厂商需在WiFi热点开启后再调用该函数，WiFi热点名称为CMQLINK-deviceType-XXXX，其中deviceType为
 * 设备在开发者门户申请的设备类型码，XXXX为设备产生的随机四位码，一般是MAC地址后缀
 *
 * @param[out] info       返回wifi信息,需厂家分配内存
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_ApConf_start(OVDXXSSIDWiFiInfo *info);

/**
 * 上报绑定信息phoneid/bindid
 *
 * @description：声波识别或二维码识别获取到信息，即可调用此接口，sdk会保存bindid信息，无需等连接wifi成功后再调用。
 *
 * @param[in] bindid    绑定的app账号信息
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_DeviceBindInfo(OVD_char *bindid);

/**
 * 音视频准备（音视频格式输入）
 *
 * @param[in] channel     通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] videoinfo   视频信息（视频格式要求是h264和h265）
 * @param[in] audeoinfo   音频信息,若无音频则为空（音频格式要求是AAC_with_ADTS）
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_AVPushStart(OVD_int32 channel, OVDVideoDataFormat *videoinfo, OVDAudioDataFormat *audeoinfo);

/**
 * 开启服务
 *
 * @description：设备连接网络成功、音视频接口准备完毕等工作之后再调用.该接口为不阻塞接口
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_ServiceStart();

/**
 * 关闭服务
 *
 * @description：重新配网等场景需停止服务时调用
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_ServiceStop();

/**
 * 告警开始
 *
 * @description：设备检测到告警条件，触发告警。若此时与信令服务器未连接，则sdk会缓存这个信令
 *
 * @param[in] alarmInfo   报警信息结构体，注意startTimeStamp单位为s，绝对时间戳
 *
 * @return
 * - =  0:  成功上报且信息缓存
 * - = -1:  失败
 * - =  1:  服务器连接异常但告警被缓存
 */
extern OVD_int32 OVD_AlarmInfoStart(OVDUpLoadAlarmInfo *alarmInfo);

/**
 * 告警结束
 *
 * @param[in] channel        通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] alarmType      告警类型，详细可见枚举OVD_AlarmType
 * @param[in] endTimeStamp   报警结束时间戳(绝对时间戳)，单位为秒
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_AlarmInfoEnd(OVD_int32 channel, OVDAlarmType alarmType, OVD_uint64 endTimeStamp);

/**
 * 音视频编码参数修改
 *
 * @description：已经启动音视频传送后，若设备的音视频参数修改，则调用此接口通知sdk变动的参数
 *               注意分辨率切换不调用此接口，视频编码切换可以调用此接口
 *               此接口内部会有停止推流、改变参数、重新推流机制，故云存会有缺失
 *
 * @param[in] channel     通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] videoinfo   视频信息,详见结构体OVDVideoDataFormat，若无视频则为空，若无修改也需要携带原来参数
 * @param[in] audeoinfo   视频信息,详见结构体OVDAudeoDataFormat，若无音频则为空，若无修改也需要携带原来参数
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_AVParamModify(OVD_int32 channel, OVDVideoDataFormat *videoinfo, OVDAudioDataFormat *audeoinfo);

/**
 * 音视频内容推送接口
 *
 * @description：设备向SDK推动音视频内容
 *
 * @others：debug_ts置为1，日志将输入每帧参数打印
 *
 * @param[in] channel       通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] contentType   准备传送的内容，详见枚举值OVD_ContentType 音频、视频
 * @param[in] isIFrame      是否是I帧  0：不是 1：是
 * @param[in] contentData   发送数据的首字节指针
 * @param[in] videoDataLen  本次发送数据的长度
 * @param[in] timestamp     该帧时间戳(ms)，厂商可以传入绝对时间戳，也可以传入相对时间戳
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_AVPushData(OVD_int32 channel, OVDContentType contentType, OVD_bool isIFrame, OVD_uchar *contentData,
                                OVD_int32 dataLen, OVD_uint64 timestamp);

/**
 * 音视频内容传送结束接口
 *
 * @description：该接口是同步接口，在弱网下因需要上传云存数据而阻塞一段时间。
 *
 * @param[in] channel   通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_AVPushEnd(OVD_int32 channel);

/**
 * SD卡录像内容回放推送接口（p2p 卡回放推流接口)
 *
 * @param[in] channel   通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] contentType   准备传送的内容，详见枚举值OVD_ContentType 音频、视频
 * @param[in] isIFrame   是否是I帧  0：不是 1：是
 * @param[in] contentData   发送数据的首字节指针
 * @param[in] videoDataLen  本次发送数据的长度
 * @param[in] timestamp   该帧时间戳(ms)
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_SendRecordAVContent(OVD_int32 channel, OVDContentType contentType, OVD_bool isIFrame, OVD_void *contentData,
        OVD_int32 dataLen, OVD_uint64 timestamp);

/**
 * SD卡录像内容回放推送完成 (p2p接口)
 *
 * @description：通知客户端录像文件播放完毕
 *
 * @param[in] channel   通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_RecordAVContentSendOver(OVD_int32 channel);

/**
 * 日志上传结果
 *
 * @description：通知OVC日志文件上传完成
 *
 * @param[in] trans_id    必填，字符串；标记此日志上传的任务ID，与LogUploadAsync保持一致
 * @param[in] result      0 代表success / 1代表fail
 * @param[in] start       必填，字符串；日志记录实际开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32
 * @param[in] end         必填，字符串；日志记录实际结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32
 * @param[in] url         必填，字符串；日志上传的URL
 * @param[in] size        必填，长整性；上传日志文件的大小字节数
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_LogDone(OVD_char *trans_id, OVD_int32 result, OVD_char *start, OVD_char *end, OVD_char *url, OVD_uint64 size);

/**
 * 低功耗设备电量上报
 *
 * @description：设备上电会上报。OVD可以通过该方法向OVC上报当前的电量，一般电量百分比变化时可以发送该通知事件。设备可以决定每隔多少百分比上报一次电量，比如每隔10%上报一次。
 *
 * @param[in] battery    设备当前的电量百分比，整数： 0-100
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 OVD_BatteryChange(OVD_int32 battery);

/**
 * 低功耗设备休眠控制功能
 * 设备获取休眠地址后回调
 *
 * @param[in] hdomain        ovh 域名
 * @param[in] hport          ovh 端口
 * @param[in] hb_interval    可选，整数：设备心跳间隔，单位秒，该字段不存在则默认为10秒
 * @param[in] hb_token       可选， 字符串：休眠服务的令牌，长度不超过16个字符。该字段不存在表示休眠服务不需要提供token
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
typedef OVD_int32(*OVD_GetHServerInfo_handle)(OVD_char *hdomain, OVD_int32 hport, OVD_int32 hb_interval, OVD_char *hb_token);
OVD_int32 OVD_GetHserverInfo_trigger(OVD_GetHServerInfo_handle func);


/*
    注册时机：若厂商 系统命令gettimeofday不支持或者有差异实现，可注册第3方实现
*/
typedef OVD_int32(*OVD_callback_thirdparty_gettimeofday)(struct timeval *tv, struct timezone *tz);

extern OVD_int32 OVD_register_thirdparty_gettimeofday(OVD_callback_thirdparty_gettimeofday func);



/**
 * 软探针模块启动
 *
 * @description：配置参数softProbe_on由0变为1时,调用此函数开启软探针模块。
 *               设备上电连接服务器无需调用此函数。
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 ovd_probe_start();

/**
 * 软探针模块暂停
 *
 * @description：配置参数softProbe_on由1变为0时,调用此函数暂停软探针模块。
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
extern OVD_int32 ovd_probe_paused();



/**
 * SDK内部日志级别调整
 *
 * @param[out] value    日志等级，详见OVDLogLevel
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_setloglevel(OVDLogLevel value);

/**
 * 实时日志控制开关
 * @description 远程配置解析log_contrl时调用，log_contrl需保存在本地配置项中
 *              初始化OVD_Init后根据本地配置项调用，log_contrl配置项默认为1
 * @param[out] value 1 开启  0关闭
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_setlogcontrol(OVD_int32 value);

/**
 * 同步设备状态
 *
 * @description：设备状态变更。应用场景：a.设备升级；      b:设备异常
 *
 * @param[in] state     1：设备在线        2：设备升级        3：异常
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_updateOVDstate(enum ovd_state state);

/**
 * 同步设备channel状态
 *
 * @description：channel状态变更。 应用场景：a.设备休眠；      b:设备唤醒
 *
 * @param[in] channel          通道号,如果是摄像头产品，则通道号为0；如果是nvr等产品，channel则为前端摄像头的索引
 * @param[in] channelstate     0：离线;    1：在线;    3：异常;    4：不使能（休眠）;
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_updatechannelstate(OVD_int32 channel, enum ovd_channel_state channelstate);



/**
 * 媒体调试相关
 */

/**
 * 开始直播抓包模式 （涉及直播，对讲，云存）
 *
 * @description：调用此接口，OVD_AVPushData 接口会将音视频数据分别dump到两个文件
 *               输出文件为：
 *               音频：{path}/{filepre}_liveaudio.adts
 *               视频：{path}/{filepre}_live.video
 *               该接口为不可重入函数接口，抓包模式下不建议切换h264,h265
 *
 * @param[in] filepath     文件输出路径
 * @param[in] filepre      文件输出前缀
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_StartLiveCapture(OVD_char *filepath, OVD_char *filepre);

/**
 * 关闭直播抓包模式
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_StopLiveCapture();

/**
 * 开始卡回放抓包模式 （涉及卡回放）
 *
 * @description：输出文件为：
 *               音频：{path}/{filepre}_recordaudio.adts
 *               视频：{path}/{filepre}_record.video
 *               该接口为不可重入函数接口，抓包模式下不建议切换h264,h265
 *
 * @param[in] filepath     文件输出路径
 * @param[in] filepre      文件输出前缀
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */
OVD_int32 OVD_StartRecordCapture(OVD_char *filepath, OVD_char *filepre);

/**
 * 关闭卡回放抓包模式
 *
 * @return
 * - = 0:  成功
 * - < 0:  失败
 */

OVD_int32 OVD_StopRecordCapture();


#ifdef __cplusplus
}
#endif

#endif //OVDOPENAPI_OPENAPI_H
