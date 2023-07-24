#ifndef OVDOPENAPI_OPENAPI_H
#define OVDOPENAPI_OPENAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdarg.h>
#include "OVD_define.h"




typedef struct{

    /*
     * 调用时机：OVC获取OVD设备信息时调用
     * 功能介绍：根据结构体OVDDeviceInfo的定义，返回相应的设备参数信息，包括设备ID、软硬件版本信息、wifi信息、ip\mac等信息
     *
    **参数说明:
    **    [out]deviceInfo:    需要设备返回的信息，详细见结构体OVDDeviceInfo
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_GetOVDDeviceInfo)(OVDDeviceInfo *deviceInfo);


    /*
    *  ####接口描述：
        OVC可以通过该命令获取OVD的配置。
        RPC ovc-->ovd
    *  ####参数说明：
       [out] output_ovdconfig: 字符串，厂商负责分配内存malloc,由设备sdk负责释放
       [out] output_size:  字符串长度
    
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */

    OVD_int32 (*OVD_GetOVDConfigureInfo)(OVD_char** output_ovdconfig, OVD_int32 *output_size);


    /*
     * 调用时机：OVC对相应的参数进行修改时，通过此接口下发需要修改的参数
     * 功能介绍：OVD根据收到的参数信息，启用相应配置的参数。没有携带的信息，设备保持原参数配置
     *
     *
    **参数说明:
    **    [in]in_ovdconfig:    需要设置的信息，json格式信息，见具体反馈信息如上“设备配置信息定义”说明
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVC_SetOVDConfigureInfo)(OVD_char* in_ovdconfig);




    /*
     * 调用时机：当SDK与OVC之间的连接状态发生变化时（连接上/连接断开），通过此接口通知OVD
     * 功能介绍：OVD根据状态的通知，决定相应处理
                要求厂商处理成异步，不能阻塞
     *
    **参数说明:
    **    [in]connectStatus:    //0:连接成功     -1:连接失败   1:重连中
    **
    **返回值：
    **    无
    */
    OVD_void (*OVD_OVCConnectStatus)(OVD_int32 connectStatus);


    /*
     * 调用时机：当用户通过APP远程控制channel重启时，调用此接口
     * 功能介绍：OVD根据入参，重新启动相应的channel；若OVD不支持单channel重启，那么就重启设备
     *
    **参数说明:
    **    [in]channel:    若重启channel，则为需要重启的channel。 注：如设备不支持单独重启channel，则直接重启设备。
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_ReBootChannel)(OVD_int32 channel);


    /*
     * 调用时机：当用户通过APP远程控制设备重启时，调用此接口
     * 功能介绍：OVD重启设备
     *
    **参数说明:
    **    无
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_ReBootDevice)();


    /*
     * 调用时机：OVC可以通过该方法阻止设备（及指定通道）在指定时间内休眠，或者唤醒设备（及指定通道）当前所有休眠的部件
     * 功能介绍：OVD收到此指令后，应该在指定的过期时间内，保证设备（及指定通道）能够完全正常上电工作。收到此命令expired秒后，由设备自行决定是否进入休眠状态
     *
    ** 参数说明：
    **    [in]channel:           需要保持不休眠的channel
    **    [in]notAllowHibernate:    是否允许进入休眠状态，0：允许进入休眠，进入休眠时间根据expired参数确定；1：不允许进入休眠，此种情况下，不管expired设置为多少，设备都要保持上电状态，不休眠，直至收到下一个休眠指令
    **    [in]expired:           从收到命令起，到expired的时间内，保持不休眠
    **    [in]reason:            设置不休眠的原因，详见枚举值定义OVDHibernateReason
    ** 返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_KeepAwakenUtilExpired)(OVD_int32 channel, OVD_int32 notAllowHibernate, OVD_int32 expired, OVDHibernateReason reason);


    /*
     * 调用时机：当OVC决定设备恢复出厂配置时，调用此接口
     * 功能介绍：设备收到该请求后，应该将所有配置恢复到出厂状态（包括wifi配置），但不能断开当前网络连接，并返回成功应答。 OVC稍后会再下发一个重启指令将设备重启，默认配置生效。
     *
    **参数说明:
    **   
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_ResetConfiguration)();


    /*
     * (p2p接口)
     * 调用时机：OVC查看OVD上SD卡的录像文件信息列表时，调用此接口
     * 功能介绍：OVD根据入参的配置，查询SD卡中的文件列表，并返回文件列表信息
     *
    **参数说明:
    **    [in]channelmask1:   需要查询的通道mask，对应低位号的通道         （弃用)
    **    [in]channelmask2:   需要查询的通道mask，对应高位号的通道 弃用
    **    [in]recordTypemask:     文件类型（第0位：视频文件 第1位：告警文件） 弃用
    **    [in]startStamp:     录像查询的起始时间戳(单位为秒)
    **    [in]endStamp:       录像查询的结束时间戳(单位为秒)
    **    [in]page:           查询的页码
    **    [in]numInPage:      每页的条数
    **    [out]fileInPage:    录像文件列表信息，详细可见结构体描述OVDRecordFileListPerPage
    **
    **返回值：
    **    成功：0
    **    失败：-1
    **
    **其他说明:
    **    假设查询的录像文件数有200个，numInPage=10，则最大页码Page为20。若传进的参数numInPage=10，page=2，则fileInPage应该返回第10个到第20个录像的信息;若传进的参数numInPage=10，page=21，则fileInPage返回空录像信息(录像个数为0)
    */
    OVD_int32 (*OVD_QueryRecordPage)(OVD_uint32 channelmask1,OVD_uint32 channelmask2,OVD_uint32 recordType, OVD_uint64 StartStamp,OVD_uint64 EndStamp,OVD_int32 Page,OVD_int32 numInPage,OVDRecordFileListPerPage *FilePage);


    /*
     * (p2p接口)
     * 调用时机：OVC根据查询到的SD卡文件列表信息，决定查看一个文件详细信息，调用此接口
     * 功能介绍：OVD根据入参的文件名称，读取SD卡上相应文件的音视频数据参数信息及录像时长，反馈到出参
     *
    **参数说明:
    **    [in]channel:          通道号
    **    [in]recordname:       录像文件名称
    **    [out]videoInfo:       视频信息,详细可见结构体描述OVDVideoDataFormat
    **    [out]audioInfo:       音频信息,详细可见结构体描述OVDAudioDataFormat
    **    [out]fileTotalTime:   该录像的时长(秒)
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_OpenRecordFile)(OVD_int32 channel,OVD_char* recordname,OVDVideoDataFormat* videoInfo,OVDAudioDataFormat* audioInfo,OVD_int32* fileTotalTime);


    /*
     * (p2p接口)
     * 调用时机：OVC根据查询到的SD卡文件列表信息，控制相应音视频文件的播放、停止、暂停等信息
     * 功能介绍：OVD根据控制信息，控制文件的内容是否通过OVD_SendRecordAVContent上传文件内容
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]controlType:     播放控制，详细可见枚举类型OVDCONTROLTYPE
    **    [in]value:           额外值，目前只有视频拖动时会用到，代表要跳至的视频时间戳(ms)
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_RecordCotrol)(OVD_int32 channel,OVDCONTROLTYPE controlType,OVD_int32 value);

 

    //srt方案 卡录像机制,卡录像信令切换到DME（websocket)
    /*
    ###接口描述：
        RecordSearch
        OVC可以通过该指令后去获取指定时间区间的卡录像记录
        RCP OVC-->OVD
    ### 参数说明
    [in] channel: 通道
    [in] starttime: 查询的起始时间,单位为秒,绝对时间戳
    [in] endtime: 查询的结束时间，单位为秒，绝对时间戳
    [in] page: 查询的页面索引，若一次查询区间不够，sdk会回调第二次
    [in] numinpage: 该页面最大的区间数量
    [out] fileinpage: 厂商回填区间信息
    
    */
    OVD_int32 (*OVD_DMEAPI_callback_RecordSearch)(OVD_int32 channel,OVD_uint64 starttime, OVD_uint64 endtime, OVD_int32 page, OVD_int32 numInPage, OVD_DMERecordFileListPerPage *fileinpage  );


    /*
    
        ### 回调接口描述：
    
          OVD_DMEAPI_callback_RecordOpen
    
          通知厂商打开一条某一channel下的卡录像会话。
    
        ### 参数说明
    
          [in] channel: 通道
    
         
    
        ### 返回值：
    
            失败： NULL
    
             成功： 返回卡录像会话厂商侧的上下文（context）标识，ctx
    
      */

     OVD_void * (*OVD_DMEAPI_callback_RecordOpen)(OVD_int32 channel);


     /*
     
        ### 回调接口描述：
     
        OVD_DMEAPI_callback_RecordSeek
     
        将指定卡录像会话游标定位到某一时间点，
     
        注：该调用后，sdk下一次读取音视频数据帧时，厂商需从相近时间点的I帧（优先向前）开始返回数据。
     
          若对应时间点没有录像数据则返回错误。
     
        ### 参数说明
     
        [in] ctx:厂商卡录像会话上下文标识
     
        [in] timestamp: 绝对时间戳，seek的绝对时间点,单位为秒
     
        ###返回值：
     
        成功：0
     
        失败：-1 通用错误
     
             OVD_RET_SEEK_NODATA 对应时间点没有录像数据
     
        */

    OVD_int32 (*OVD_DMEAPI_callback_RecordSeek)(OVD_void* ctx, OVD_int64 timestamp);

    
    /*
    ###接口描述:
        OVD_DMEAPI_callback_RecordReadFrame
    ###参数说明:
        [in] ctx: 厂商卡录像会话上下文
        [in] pframe_info： 帧数据信息
            注：pframe_info 结构体内存由sdk分配, 由sdk 负责释放
                pframe_info里的frame_buf 由厂商分配，也由厂商释放;建议厂商在下次read或者会话结束释放内存；
     ##返回值：
           OVD_RET_COMMON_ERROR = -1,    //通用错误码
    			OVD_RET_SUCCESS = 0,           //成功 
    			OVD_RET_READ_FRAME_RETRY=201, //读帧重试错误码
    			OVD_RET_READ_FRAME_EOF=202,   //卡录像读取完毕，厂商反馈EOF

          若收到通用错误，则sdk会调用销毁卡录像会话资源机制
    */
    OVD_int32 (*OVD_DMEAPI_callback_RecordReadFrame)(OVD_void* ctx, OVD_FrameInfo *pframe_info);




    /*
    ###接口描述：
        OVD_DMEAPI_callback_RecordClose
    ###参数说明：
        [in] ctx： 厂商卡录像会话上下文
    ### 返回值：
        0：代表成功
        其他：代表失败
    */
    OVD_int32 (*OVD_DMEAPI_callback_RecordClose)(OVD_void *ctx);

    //srt 卡录像机制

    /*
     * 调用时机：OVC根据配置及OVD当前的版本，触发OVD进行固件升级时调用
     * 功能介绍：OVD对比要升级的固件版本号和当前自身的固件版本号，若版本号不一致，那么到相应的URL去下载升级固件
     *
    **参数说明:
    **    [in]firmware_model:  要升级的固件的版本号
    **    [in]upgradeURL:      升级固件的远程url，由设备主动去下载、升级
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_FirmwareUpgrade)(OVD_char *firmware_model, OVD_char *upgradeURL);


    /*
     * 调用时机：OVC下发升级版本的命令后，会周期性的查询OVD的升级状态及进度
     * 功能介绍：OVD根据当天升级状态及进度，反馈相应的值
     *
    **参数说明:
    **    [out]upgradeStatus:    升级状态，详细见枚举值OVDUpgradeStatus
    **    [out]upgradeProgress:  升级进度，整数值 0-100
    **    [out]version:          //当前系统版本，version所占内存由sdk分配
    **    [in]version_len:      //sdk分配version内存的最大长度
    **    [out]last_upgrade_time:  //<字符串，最近一次处理升级指令的时间，格式”YY-MM-DDTHH:MM:SS”>,sdk 已分配内存
    **    [in]time_len:          //sdk分配的last_upgrade_time的最大长度
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_QueryFirmwareUpgradeStatus)(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress,OVD_char *version, OVD_int32 version_len, OVD_char *last_upgrade_time,OVD_int32 time_len);


    /*
     * 调用时机：OVC需要与设备同步时间时调用
     * 功能介绍：OVD根据传入的时间及偏差，决定是否根据下发的参数修改时间
     *
    **参数说明:
    **    [in] datetime  <必填，字符串；格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
    **    [in] tz        <可选，整数：时区数，东为正数，西为负数。例如东八区为+8，默认为+8>
    **    [in] offset: <必填，整数；可接受的偏差，单位秒，若摄像机时间与上面给定的时间的偏差在offset秒之内，则摄像机无需同步时间>
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_SyncTime)(OVD_char *datetime,OVD_int32 tz, OVD_int32 offset);


    /*
     * 调用时机：OVC需要获取设备时间时调用
     * 功能介绍：OVD查询本地时间，反馈参数
     *
    *  ####参数说明：
        [out] datetime  <必填，字符串；格式YY-MM-DDTHH:MM:SS，例子：2016-12-05T02:15:32>,sdk分配内存
        [in] time_len  datatime 分配的长度
        [in] tz  时区
    *   ###返回值：
    *       成功：0 
    *       失败：其他值

    */
    OVD_int32 (*OVD_QueryTime)(OVD_char* out_datetime, OVD_int32 time_len, OVD_int32 *tz);


    /*
     * (p2p接口，目的为兼容老设备)（已弃用）
     * 调用时机：OVC需要获取SD卡信息时调用
     * 功能介绍：OVD查询本地SD卡相关信息（OVDSDInfo定义的内容），反馈信息
     *
    **参数说明:
    **    [out]sdInfo:   存储卡信息,详细可见结构体描述OVDSDInfo
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_GetSDInfo)(OVDSDInfo *sdInfo);


    /*
     * 调用时机：OVC需要格式化SD卡时调用
     * 功能介绍：OVD格式化SD卡
     *
    **参数说明:
    **    无
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_SetSDCardFormat)();


    /*
     * 调用时机：APP端控制OVD进行转动时，调用此接口
     * 功能介绍：OVD根据控制命令，执行动作
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]ptzcmd:          控制命令,详细可见枚举类型OVCPTZControlCmd
    **    [in]ptzvalue:        参考下图   
    
    | op | 操作 | value |
    | --- | --- | --- |
    | up | 上 | 可选，整数，速度，0-100，0最慢，100最快，默认50 |
    | down | 下 | 同上 |
    | left | 左 | 同上 |
    | right | 右 | 同上 |
    | upleft | 左上 | 同上 |
    | upright | 右上 | 同上 |
    | downleft | 左下 | 同上 |
    | downright | 右下 | 同上 |
    | zoomin | 拉近 | 同上 |
    | zoomout | 拉远 | 同上 |
    | stop | 停止 | 可选，整数，但数值没有意义 |
    | goto_preset | 跳转预置位 | 预置位ID， 0-255 |
    | set_preset | 设置预置位 | 同上 |
    | clear_preset | 清除预置位 | 同上 |
    | up_step | 单步上 | 可选，整数，单步步长，0-100，0最小，100最大，默认0 |
    | down_step | 单步下 | 同上 |
    | left_step | 单步左 | 同上 |
    | right_step | 单步右 | 同上 |
    | upleft_step | 单步左上 | 同上 |
    | upright_step | 单步右上 | 同上 |
    | downleft_step | 单步左下 | 同上 |
    | downright_step | 单步右下 | 同上 |
    | zoomin_step | 单步拉近 | 同上 |
    | zoomout_step | 单步拉远 | 同上 |
    
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_PTZCmd)(OVD_int32 channel,OVCPTZControlCmd ptzcmd,OVD_int32 ptzvalue);


    /*
     * 调用时机：OVC获取OVD预置点列表信息
     * 功能介绍：OVD返回前期配置的预置点信息
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]presetList:     返回的预置点列表,sdk已分配内存
    **    [in]array_malloc_num:  sdk内部分配的预置点数组大小
    **    [out]count:          预置点实际个数
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_GetPresetList)(OVD_int32 channel,OVD_int32 *presetList,OVD_int32 array_malloc_num, OVD_int32 *count);


    /*
     * (p2p接口)
     * 调用时机：APP端发起对讲时，调用此接口打开OVD对讲
     * 功能介绍：OVD打开OVD对讲功能
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]andioFormat:     对讲音频数据的格式信息，见结构体定义OVDAudioOutDataFormat
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_AudioPlayStart)(OVD_int32 channel, OVDAudioOutDataFormat andioFormat);


    /*
     * (p2p接口)
     * 调用时机：OVC把对讲的音频内容，传输给OVD时调用
     * 功能介绍：OVD接收数据，并在音箱播放出来
     *
    **参数说明:
    **    [in]channel:   通道号
    **    [in]buf:      音频数据指针
    **    [in]size:     音频数据大小
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_AudioPlayProGress)(OVD_int32 channel,OVD_uchar* buf, OVD_int32 size);


    /*
     * (p2p接口)
     * 调用时机：APP关闭对讲时通知OVD
     * 功能介绍：OVD关闭对讲功能
     *
    **参数说明:
    **    [in]channel:         通道号
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_AudioPlayStop)(OVD_int32 channel);


    /*
     * (p2p接口，目的为兼容老设备,已弃用)
     * 调用时机：OVC通知OVD切换视频清晰度时调用
     * 功能介绍：OVD根据配置切换到相应的视频清晰度，并返回切换后的视频参数
     *  该接口已弃用，分辨率切换放置于get/set config 里的调用里做
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]quality:         要设置的清晰度，详细参考枚举值OVDEncodeQuality
    **    [out]vedioInfo:      清晰度调整后的视频参数，详细参考结构体OVDVideoDataFormat
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_VedioSwitchQuality)(OVD_int32 channel, OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo);


    /*
     * 调用时机：SDK在收到上传视频、P2P播放等命令时，为了服务器/客户端能够最短时间内播放视频，调用此接口强制OVD出一个I帧
     * 功能介绍：OVD收到后，在此channel上的数据流上强制出一个I帧，并继续上传数据流
     *
    **参数说明:
    **    [in]channel:         通道号
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_ForceIFrame)(OVD_int32 channel);


    /*
     * 调用时机：OVC通知OVD截取channel上的当前图片
     * 功能介绍：OVD截取channel上的当前图片，反馈截图内容及信息。若截图大小大于maxImageSize，则返回-2并在OVDImageInfo.size中带回所需要图片大小
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]ImageInfo;	   截取的图片信息
    **
    **返回值：
    **    成功：0
    **    截图失败：-1
    **    空间不足：-2,  maxImageSize空间不足，并在OVDImageInfo.size中带回所需要图片大小
    */
    OVD_int32 (*OVD_Snapshot)(OVD_int32 channel,OVDImageInfo *imageInfo, OVD_int32 maxImageSize);


    /*
     * 调用时机：OVC通知OVD播放音乐时调用,暂不启用
     * 功能介绍：OVD去url下载音乐内容，并在channel的音箱上播放
     *
    **参数说明:
    **    [in]channel:      通道号
    **    [in]url:          歌曲下载的url
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_SetAudioOutPlay)(OVD_int32 channel,OVD_char *url);


    /*
     * 调用时机：OVC控制OVD的音乐播放，暂不启用
     * 功能介绍：OVD根据控制信息，控制音乐的播放 停止、暂停、继续
     *
    **参数说明:
    **    [in]channel:      通道号
    **    [in]ctrl:         播放控制,详细可见枚举类型OVDMp3PlayCtrl
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_AudioOutPlayCtrl)(OVD_int32 channel,OVDMp3PlayCtrl ctrl);


    /*
     * 调用时机：OVC查询OVD上当前的音乐播放情况，暂不启用
     * 功能介绍：OVD根据当前的播放情况，反馈播放音乐的URL及播放状态（播放 停止、暂停、继续）；若未播放任何音乐，则URL为空
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]status:         播放状态,见OVDAudioPlayStatus枚举值定义
    **    [out]url:            正在播放的歌曲的下载url，该域不存在或者空串表示当前未播放，url最长1024字节
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    OVD_int32 (*OVD_GetAudioOutPlayStatus)(OVD_int32 channel, OVD_int32* status, OVD_char* out_url);

    //从ovd设备中获取时间戳函数，精确度毫秒， 暂不启用
    OVD_int32 (*OVD_gettime)(OVD_uint64 *out_time);


    /*
        回调时机：重新调度时，调度服务器时间同步
        参数说明：
        [in]: input_time 设置的时间戳，单位为毫秒
        [in]:tolerance_value 设置的容忍值，单位为秒,即当前系统时间戳和预设置的时间相差小于容忍值，则不需要设置系统时间
        返回值：
        成功：0
        失败：-1
    */
    OVD_int32 (*OVD_settime)(OVD_uint64 input_time, OVD_int32 tolerance_value);

	/*同步软探针模块阈值
		packetLossRateThreshold：丢包率阈值（高于）
		avgRTTThreshold：平均RTT阈值 单位毫秒（高于）
 		wifiQualityThreshold：WiFi信号周期内平均信号强度阈值（低于）
		cpuLoadThreshold：CPU使用率阈值（高于）
		memoryLoadThreshold：内存使用率阈值（高于）
		memoryAvaliable：剩余内存 单位kB （低于）
		checkPeriod：检测周期（默认2分钟  单位秒）
		reportSwitch：上报开关配置（默认开  不固化到设备） 1：开关打开，0：开关关闭
	*/
	OVD_int32 (*OVD_syncProbeDetector)(OVD_int32 avgRTTThreshold,OVD_int32 checkPeriod, OVD_int32 cpuLoadThreshold, 
	OVD_int32 memoryLoadThreshold,OVD_int32 memoryAvaliable,OVD_int32 packetLossRateThreshold,OVD_int32 reportSwitch,OVD_int32 wifiQualityThreshold);

	/*
    *  ####接口描述：
        同步软探针配置项softprobe_on
	####功能介绍：
		返回结果为softprobe_on的值，返回1sdk会开启软探针模块，返回0则不开启软探针模块。默认开启。
    *  ####参数说明：
       无
    *   ###返回值：
    *       成功：0/1 
    *       失败：其他值
    */
	OVD_int32 (*OVD_ProbeByval)();

    /*
        ##接口描述
        回调时机：服务启动初期
        ##参数说明
        [out] out_simpleovd sdk已分配内存
        channel_count不能少于1
    */
    OVD_int32 (*OVD_getsimpleovdinfo)(SimpleOVDinfo *out_simpleovdinfo);

    /*
    *  ####接口描述：
        获取sd卡状态


    *  ####参数说明：
        out_state: <必填，整数；sd卡状态，0：正常，1：未插卡，2：卡异常,3格式化中>,
        "out_total": <必填，整数, sd卡总容量，单位为MB>,
        "out_free":  <必填, 整数, sd卡剩余容量，单位为MB> 
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */
    OVD_int32 (*OVD_GetDiskInfo)(OVD_int32 *out_state, OVD_int32 *out_total, OVD_int32 *out_free);

    /*
    *  ####接口描述：
        OVD下发软探针命令触发软探针实时上报
	####功能介绍：
		管理平台信令下发上报探针指标指令，设备接收到指令后无需和阈值做比较而是立刻调用SDK指标上报接口（OVD_Probe_StatusReport）上报当前设备各项指标数据。注意实时上报OVD_Probe_StatusReport中key传1

    *  ####参数说明：
       无
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */
	OVD_int32 (*OVD_TriggerSoftProbe)();

    /*
    *  ####接口描述：
        LogUploadAsync
        OVC可以通过该命令控制OVD上传相关本地日志，供调试分析使用。OVD收到该指令后，通过异步方式将指定时间范围日志上传到相应的URL。 若指定范围的日志已经滚动删除，则使用尽力原则上传剩余的日志。若指定范围没有任何日志记录，应上传一个长度为0的日志文件。 日志文件通过PUT方法上传。
    
    *  ####参数说明：
        "trans_id": <必填，字符串；标记此日志上传的任务ID>
        "start": <必填，字符串；日志记录开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
        "end": <必填，字符串；日志记录结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32>,
        "url": <必填，字符串；日志上传的URL，设备通过PUT方法上传相应的日志文件>

    *   ###返回值：
    *       成功：0 
    *       失败：其他值
*/
    OVD_int32 (*OVD_LogUploadAsync)(OVD_char *trans_id, OVD_char *start, OVD_char* end,OVD_char* url);

	/*
    *  ####接口描述：
        OVD_extension_callback
        OVC可以通过该命令下发指令给OVD
    
    *  ####参数说明：
		[in]method:信令里的method字段
		[in]in_data:信令里带的method_param字段
		[out]out_response: 第3方回调返回的字符串内容
		[in]in_outresponse_len: out_repsonse 预分配的内存字符串长度，目前预分配1024(包括\0)，厂商不能超过1000长度
    *   ###返回值：
    *       成功：0 
    *       失败：
	*			-1： 通用设备错误
	*			-2： 标识mehod字段 厂商不支持
	*			-3：参数不合法或无效
	*/
    OVD_int32 (*OVD_extension_callback)(char *method, char *in_data, char *out_response, OVD_int32 in_outresponse_len, OVD_int32 *out_outresponse_len);

}OVD_CallBackFunList;




////接口定义

/*
### 接口描述
    将capinfo进行默认值初始化。
    默认值的数值参考对接文档
### 参数说明：
[in] capinfo:  厂商设备能力集

### 返回值：
0：成功
其他： 失败
*/
extern OVD_int32 OVD_CapInit(OVDCapInfo *capinfo);

/*
SDK初始化
### 接口描述
        设备上电后调用
### 参数说明：
[in]capinfo            设备能力集
[in]clientParam:       设备连接服务器信息，见OVDClientParam结构体
[in]logParam:          输出日志配置信息，见LogParam结构体
[in]callBackFunList:   提供给服务器端调用的回调函数，以相应服务器端的请求，见OVD_CallBackFunList。   注：若未提供相关的回调函数，则相关请求被丢弃，及设备端不提供对应的功能。
[in]jsonParam:         json格式化后的字符串，提供动态的参数配置。目前提供 1) snapshotSize: int型数值，设备截图的默认大小；2) buffDuration: int型数值，设备缓存的默认时长，单位为秒；
     json格式信息参考如下：
         {
             "snapshotSize":<可选，整数： 设备默认清晰度下的截图大小。若不设置，则SDK中默认设置为500k>
             "buffDuration":<可选，整数： 设备从上电到连接到平台时，需要SDK缓存的音视频内容的时长，单位为秒。若不设置，则SDK中默认设置为0s，建议低功耗设备设置此值，一般设置为一个数据分片的大小（约10s）>
             "debug_ts":<可选，布尔型，是否开启媒体数据输出ts开关
         }
### 返回值：
OVD_RET_SUCCESS：0  初始化成功
OVD_RET_BADPARAMETER：1  入参校验失败
OVD_RET_COMMON_ERROR：-1 通用错误，初始化失败

 */
extern OVD_int32 OVD_Init(OVDCapInfo *capinfo,OVDClientParam *clientParam, OVDLogParam *logParam, OVD_CallBackFunList *callBackFunList, OVD_char *jsonParam);


/*
开启服务
### 接口描述
        设备服务准备好后调用，即设备连接网络成功、音视频接口准备完毕等工作之后再调用
        该接口为不阻塞接口
### 参数说明：
无
### 返回值：
0:成功
-1：失败
 */
 extern OVD_int32 OVD_ServiceStart();

/*
关闭服务
### 接口描述
        设备服务停止时后调用
### 参数说明：
无
### 返回值：
无
*/
 extern OVD_int32 OVD_ServiceStop();



/*
 告警开始
### 接口描述
        设备检测到告警条件，触发告警
### 参数说明：
[in]alarmInfo:    报警信息结构体，详细可见结构体描述OVD_UpLoadAlarmInfo
### 返回值：
//return 0: 成功上报且信息缓存；
//return -1: 异常
//return 1: 服务器连接异常但告警被缓存
*/
extern OVD_int32 OVD_AlarmInfoStart(OVDUpLoadAlarmInfo *alarmInfo);

/*
告警结束
### 接口描述
        设备告警结束
### 参数说明：
[in]channel;        通道号
[in]alarmType:      告警类型，详细可见枚举OVD_AlarmType
[in]endTimeStamp:   报警结束时间戳(绝对时间戳)
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_AlarmInfoEnd(OVD_int32 channel, OVDAlarmType alarmType, OVD_uint64 endTimeStamp);

/*
### 接口描述
        设备准备喂流，先输入音视频格式
### 参数说明：
[in]channel:         通道号
[in]videoinfo:       视频信息,详见结构体OVDVideoDataFormat，若无视频则为空
[in]audeoinfo:       视频信息,详见结构体OVDAudeoDataFormat，若无音频则为空
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_AVPushStart(OVD_int32 channel,OVDVideoDataFormat *videoinfo,OVDAudioDataFormat *audeoinfo);


/*
音视频参数修改
### 接口描述
        已经启动音视频传送后，若设备的音视频参数修改，则调用此接口通知sdk变动的参数
### 参数说明：
[in]channel:         通道号
[in]videoinfo:       视频信息,详见结构体OVDVideoDataFormat，若无视频则为空，若无修改也需要携带原来参数
[in]audeoinfo:       视频信息,详见结构体OVDAudeoDataFormat，若无音频则为空，若无修改也需要携带原来参数
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_AVParamModify(OVD_int32 channel,OVDVideoDataFormat *videoinfo,OVDAudioDataFormat *audeoinfo);

/*
视频加密接口
### 接口描述
    视频加密接口。设备调用OVD_AVPushData前先调用OVD_enc_frame进行视频加密。
    注意加密接口返回成功，OVD_AVPushData的入参为加密后的数据，加密接口返回失败，OVD_AVPushData的入参为加密前的数据。
### 参数说明：
[in]channel:         通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     待加密帧內容
[in]videoDataLen:    待加密数据长度
[in]enc_buf:       加密结果內容
[in]enc_buf_len:       加密后数据内存分配大小 (enc_buf比contentData内存大128)
[in]enc_len:       加密结果长度
[in]codec_enc:    视频格式，1表示h264，2表示h265
### 注意：
	enc_buf比contentData内存大128
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_enc_frame(OVD_int32 channel,OVD_bool isIFrame,OVD_uchar* contentData,OVD_int32 videoDataLen, OVD_uchar* enc_buf,OVD_int32 enc_buf_len ,OVD_int32 *enc_len, OVD_int32 codec_enc);

/*
音视频内容推送接口
### 接口描述
        设备向SDK推动音视频内容
### 参数说明：
[in]channel:         通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引，
[in]contentType:     准备传送的内容，详见枚举值OVD_ContentType 音频、视频
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     发送数据的首字节指针
[in]videoDataLen:    本次发送数据的长度
[in]timestamp:       该帧时间戳(ms)
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_AVPushData(OVD_int32 channel,OVDContentType contentType,OVD_bool isIFrame,OVD_uchar * contentData,OVD_int32 dataLen, OVD_uint64 timestamp);


/*
音视频内容传送结束接口
该接口是同步接口，在弱网下因需要上传云存数据而阻塞一段时间。
 ### 参数说明：
[in]channel:         通道号
### 返回值：
成功：0
失败：-1
*/
extern OVD_int32 OVD_AVPushEnd(OVD_int32 channel);



/*
SD卡录像内容回放推送接口（p2p 卡回放推流接口)
### 接口描述
        APP打开相关录像文件后，设备推送相关内容
        *录像内容查询、打开录像文件、录像文件控制、录像删除等功能，由回调函数定义，详见（SDK初始化）的参数定义*
### 参数说明：
[in]channel:         通道号
[in]contentType:     准备传送的内容，详见枚举值OVD_ContentType 音频、视频、音视频
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     发送数据的首字节指针
[in]dataLen:    本次发送数据的长度
[in]timestamp:       该帧时间戳(ms)，相对于对应的逻辑卡录像文件头
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_SendRecordAVContent(OVD_int32 channel,OVDContentType contentType,OVD_bool isIFrame,OVD_void* contentData,OVD_int32 dataLen,OVD_uint64 timestamp);

/*
SD卡录像内容回放推送完成 (p2p接口)
### 接口描述
    通知客户端录像文件播放完毕
### 参数说明：
[in]  channel [in] - 通道号
### 返回值：
成功：0
失败：-1
 */

extern OVD_int32 OVD_RecordAVContentSendOver(OVD_int32 channel);




/*
 声波初始化
### 接口描述
        设备检测到无网络配置信息，判断若使用声波配网，则调用此接口，传入声波参数
### 参数说明：
[in]sampleRate:      采样率
[in]bitWidth:        位宽(8/16bit)
### 返回值：
成功：返回声波句柄
        失败：NULL
*/
extern OVD_void* OVD_SoundWaveInit(OVD_int32 sampleRate,OVD_int32 bitWidth);


typedef OVD_void (*RecognizerStart)();
typedef OVD_void (*RecognizerFinish)(OVD_int32 type, OVD_void *info, OVD_int32 infoLen);

/*
开始声波识别
### 接口描述
        设备开始声波配网后，获取到音频文件，发送到SDK识别；声波识别模块识别出数据后，调用回调end_cbfunc返回给上层
### 参数说明：
[in]recognizer:      声波句柄
[in]starFunc:        识别开始回调函数；回调函数定义为 void (*RecognizStart)(void);
[in]endFunc:         识别结束回调函数（此函数返回wifi信息）；回调函数定义为 void (*RecognizerFinish)(struct OVDWiFiInfo info);
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_SoundWaveStart(OVD_void *recognizer,RecognizerStart starFunc,RecognizerFinish endFunc);


/*
采集到的声波数据，写入识别器
### 接口描述
        设备开始声波配网后，把声波数据传入SDK的识别器
### 参数说明：
[in]recognizer:      声波句柄
[in]data:            声波数据的首字节指针
[in]len:             声波数据长度
### 返回值：
成功：成功写入的数据长度
        失败：小于0的值
 */
extern OVD_int32 OVD_SoundWaveWriteData(OVD_void *recognizer,const OVD_void *data,OVD_uint64 len);


/*
 停止声波识别
### 接口描述
        识别完后，调用此接口,回收资源
### 参数说明：
[in]recognizer:      声波句柄
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_SoundWaveStop(OVD_void *recognizer);


/*
 二维码配网开始
### 接口描述
        设备检测到无网络配置信息，判断若使用二维码配网，则调用此接口
### 参数说明：
    [in]cbfun:      成功的回调函数
    [in]reverse:    图片是否翻转
### 返回值：
    成功：返回二维码识别句柄
    失败：NULL
 */

/* 方式一（已弃用）： 使用看护平台设备sdk中编译的zbar库       OVD_QRInit、OVD_QRStart、OVD_QRDestroy*/

extern OVD_void *OVD_QRInit(RecognizerFinish cbfun,OVD_int32 reverse);

/*
 二维码配网识别
### 接口描述
        传入图片进行识别
### 参数说明：
    [in]handle:      二维码识别句柄
    [in]raw:         二维码内容
    [in]width:       宽
    [in]height:      高
    [in]bitCount:    位宽
### 返回值：
    成功：0
    失败：-1
 */
extern OVD_int32 OVD_QRStart(OVD_void *handle, OVD_void *raw, OVD_int32 width, OVD_int32 height, OVD_uint32 bitCount);


/*
 二维码配网结束
### 接口描述
        设备停止二维码配网
### 参数说明：
    [in]handle:      二维码识别句柄
### 返回值：
    成功：0
    失败：-1
 */
extern OVD_int32 OVD_QRDestroy(OVD_void *handle);



/* 方式二：不使用看护平台设备sdk中编译的zbar库 OVD_QRString_init、OVD_QRString*/

/*
    二维码配网输入字符串流程初始化

### 返回值：
   OVD_RET_COMMON_ERROR   -1 内部错误
   OVD_RET_SUCCESS  0   初始化成功
*/
extern OVD_int32 OVD_QRString_init();

/*
 * 二维码配网输入字符串
 * ### 接口描述
 *      设备读取字符串,返回解析结果
 * ### 参数说明：
        [out] info,厂商分配内存的指针，识别成功后，识别出的数据会存入这个内存
        [in] qr_str，具体一张二维码图片厂商解析出的字符串
        [in] qr_len, 具体一张二维码图片厂商解析出的字符串长度
 * ### 返回值：
        OVD_RET_RPC_UNIMPLEMENT  101 未集成字符串解析模块
        OVD_RET_BADPARAMETER     1 入参错误
        OVD_RET_COMMON_ERROR    -1 本次输入的字符串无法解析或者最终解析到数据有误，此时厂商可继续输入数据继续识别。
        OVD_NETCONF_SUCCESS   7 识别成功
        OVD_NETCONF_LESS_DATA  9  配网缺失数据，数据太长，二维码分页，需要厂商再次输入
 */
//extern int OVD_QRString(OVDXXSSIDWiFiInfo *info, unsigned char *qr_str,unsigned int qr_len);
extern OVD_int32 OVD_QRString(OVD_void *info, OVD_uchar *qr_str,OVD_uint32 qr_len);



/*
## 接口描述
  设备上报绑定信息。
### 参数说明：
[in]bindid:      绑定的app账号信息
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_DeviceBindInfo(OVD_char* bindid);

/*
通知OVC日志文件上传完成
### 参数说明：
[in]
  "trans_id": <必填，字符串；标记此日志上传的任务ID，与LogUploadAsync保持一致>
  result: 0 代表success / 1代表fail
  "start": <必填，字符串；日志记录实际开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
  "end": <必填，字符串；日志记录实际结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32>,
  "url": <必填，字符串；日志上传的URL>
  "size": <必填，长整性；上传日志文件的大小字节数>
### 返回值：
成功：0
失败：-1
*/
extern OVD_int32 OVD_LogDone(OVD_char* trans_id, OVD_int32 result, OVD_char* start, OVD_char *end, OVD_char *url,OVD_uint64 size);

/*
 设置SDK与云服务器之间的心跳周期，暂不需启用）
### 接口描述
        一般不用配置
### 参数说明：
[in]interval:     心跳周期，单位为秒
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_SetKeepaliveIntervel(OVD_int32 interval);

/*
接口描述：从平台获取token，同步接口，暂不需启用
设备上传bindid后，app识别bindid存token，设备通过平台取token
[in]serviceurl :平台地址
[in]userId:bindid
[in]ovdId:设备序列号
[in]ovdPassword：设备密码
[in]io_timeout:连接服务器超时时间
[in]betoken:用于存放获取到的token值，自行分配内存

*/
extern OVD_int32 OVD_GetIotToken(OVD_char* serviceurl,OVD_char* userId,OVD_char* ovdId,OVD_char * ovdPassword,OVD_int32 io_timeout,OVD_char * betoken);

/*
 OVD电池电量变化通知
### 接口描述
        OVD可以通过该方法向OVC上报当前的电量，一般电量百分比变化时可以发送该通知事件。设备可以决定每隔多少百分比上报一次电量，比如每隔10%上报一次。
### 参数说明：
[in]battery:     设备当前的电量百分比，整数： 0-100
### 返回值：
成功：0
失败：-1
 */
extern OVD_int32 OVD_BatteryChange(OVD_int32 battery);




/*
设备获取休眠地址后回调

    [in] "hdomain": ovh 域名
    [in] "hport": ovh 端口
    [in] "hb_interval": <可选，整数：设备心跳间隔，单位秒，该字段不存在则默认为10秒>,
    "hb_token": <可选， 字符串：休眠服务的令牌，长度不超过16个字符。该字段不存在表示休眠服务不需要提供token>
*/
typedef OVD_int32 (*OVD_GetHServerInfo_handle)(OVD_char *hdomain, OVD_int32 hport, OVD_int32 hb_interval,OVD_char *hb_token);

OVD_int32 OVD_GetHserverInfo_trigger(OVD_GetHServerInfo_handle func);


/*
    调用时机：设备不重启的情况下更新版本号,暂不需厂商启用
*/
extern OVD_int32 OVD_update_systemversion(OVD_char *version);


/*
    注册时机：若厂商 系统命令gettimeofday不支持或者有差异实现，可注册第3方实现
*/
typedef OVD_int32 (*OVD_callback_thirdparty_gettimeofday)(struct timeval *tv, struct timezone *tz);

extern OVD_int32 OVD_register_thirdparty_gettimeofday(OVD_callback_thirdparty_gettimeofday func);


#ifdef CONFIG_MODULE_softprobe
/* 
软探针模块详述
1、能力集：设备具备软探针能力集。
2、上线阈值获取：设备开机会向软探针监测平台发起请求，获取到token和软探针监测阈值。监测阈值包括6项指标阈值（丢包率、平均RTT、WiFi信号强度、CPU使用率、内存使用率、剩余内存）、自检周期、阈值上报开关。
3、指标自检：设备按照获取阈值中的自检周期进行自检，默认10分钟一次。
4、软探针开关：阈值上报开关（不固化到设备）为0或管理平台软探针开关（化到设备）为0则不进行自检。
5、异常告警上报：自检时与监测指标进行比较，当指标中有一项未达标则触发异常告警上报，调用SDK指标上报接口进行上报，两次异常上报时间间隔>=20分钟（SDK可配置）。
6、实时上报：管理平台信令下发上报探针指标指令，设备接收到指令后调用SDK指标上报接口上报当前设备各项指标数据。
7、复用信令服务设备信息查询接口DevInfo，增加当前需求中的指标数据。
*/

/*
###软探针指标上报
软探针指标上报接口
### 接口描述：
异常告警上报和实时上报都调用此接口。区别在两个方面，一方面异常告警上报key传0，实时上报key传1，另一方面异常告警上报需要判断阈值，实时上报不需要判断阈值。
### 参数说明：
int packetLossRate;  //整数：0-100 ,丢包率 例如：10， 表示丢包率为10%。针对丢包率，设备端使用ifconfig指令来获取网卡的数据传输情况。packetLossRate=(drop_rx+drop_tx)/(rx+tx)
int maxRTT;   //整数： 0-100, 最大RTT       例如：10，表示周期内网络时延最大rtt为10毫秒。通过sdk接口获取
int minRTT;   //整数： 0-100, 最小RTT       例如：10，表示周期内网络时延最小rtt为10毫秒。通过sdk接口获取
int avgRTT;   //整数： 0-100, 平均RTT       例如：10，表示周期内（默认5分钟）网络时延平均rtt为10毫秒。通过sdk接口获取
int wifiQuality;   //整数：负值（dBm） 例如：-80 ，表示WiFi信号周期内平均信号强度为-80
int cpuLoad;   //整数：0-100, cpu占有率 例如：80，表示cpu单核占用率为80%，使用TOP指令，获取设备CPU占用情况。阈值大致为90%
int memoryLoad;   // 整数：0-100, 内存占有率 例如：80，表示当前内存占用率为80%。使用TOP指令，获取设备整个内存占用情况。监测buff字段，阈值大致为1024K
int memoryAvailable;   //剩余内存（kB）
int key; //是否使用上传时间的标志位。设备主动上报，置0，采用上报时间间隔。平台下发上报，置1，不采用上报时间。


### 返回值：
成功：0
失败：-1


*/
	
extern OVD_int32 OVD_Probe_StatusReport(OVD_int32 packetLossRate,OVD_int32 maxRTT,OVD_int32 minRTT,OVD_int32 avgRTT,OVD_int32 wifiQuality,OVD_int32 cpuLoad,OVD_int32 memoryLoad,OVD_int32 memoryAvailable,OVD_int32 key);

/*
*	####接口描述:
	改变软探针模块状态
*	####参数说明：
	[in]new_status：要改变成的模块状态      
    Probe模块未启动(停止软探针模块)：0 
    Probe模块启动(开始软探针模块)：1
*   ####返回值：
    成功：0
    失败：其他值
*   ####调用方法：
	收到配置参数softProbe_on为0，调用OVD_Probe_status_change(0)；
	收到配置参数softProbe_on为1，调用OVD_Probe_status_change(1)；	
*/
extern OVD_int32 OVD_Probe_status_change(OVD_int32 new_status);


#endif



//设置日志级别
OVD_int32 OVD_setloglevel(OVDLogLevel value);



#if 0
//ovd 状态
enum DME_ovd_state {
    DME_OVD_ONLINE = 1, //设备在线
    DME_OVD_UPDATING=2,  //设备升级
    DME_OVD_ABNORMAL=3 //异常
};


//channels 状态
enum DME_channel_state {
    DME_CHANNEL_OFFLINE=0, //离线
    DME_CHANNEL_ONLINE=1, //在线
 
    DME_CHANNEL_ABNORMAL=3, //异常
    DME_CHANNEL_DISABLE=4,   //不使能
};
#endif
//设备同步设备状态
//state值选项: 1：设备在线        2：设备升级        3：异常
//主要场景：a.设备升级      b:设备异常
OVD_int32 OVD_updateOVDstate(enum ovd_state state);


//设备同步channel状态
//channelstate 值选项：0：离线           1：在线    3：异常    4：不使能
OVD_int32 OVD_updatechannelstate(OVD_int32 channel, enum ovd_channel_state channelstate);



//媒体调试相关
/*
开始直播抓包模式 （涉及直播，对讲，云存）         
    [in] "filepath": 文件输出路径
    [in] "filepre": 文件输出前缀

     OVD_AVPushData 接口会将音视频数据分别dump到两个文件
 输出文件为 音频：{path}/{filepre}_liveaudio.adts
            视频：{path}/{filepre}_live.video
            

 该接口为不可重入函数接口，抓包模式下不建议切换h264,h265
 返回值： 0代表成功
        非0 代表失败
*/
OVD_int32 OVD_StartLiveCapture(OVD_char *filepath, OVD_char* filepre);

/*
关闭直播抓包模式
返回值 0代表成功
       非0 代表失败
*/
OVD_int32 OVD_StopLiveCapture();

/*
开始卡回放抓包模式(涉及卡回放)
[in] "filepath"： 文件输出路径
[in] "filepre":   文件输出前缀
OVD_SendRecordAVContent 接口会将音视频数据分别dump到两个文件
输出文件为 音频：{path}/{filepre}_recordaudio.adts
            视频：{path}/{filepre}_record.video
  
*/
OVD_int32 OVD_StartRecordCapture(OVD_char *filepath, OVD_char* filepre);

/*
关闭卡回放抓包模式
*/
OVD_int32 OVD_StopRecordCapture();


/*
*  ####接口描述：
    OVD_get_RTTInfo
    获取DME RTT 统计信息, 每次获取，会将sdk 内部的统计信息清0
*  ####参数说明：
    [out] 最大RTT
    [out] 最小RTT
    [out] 平均RTT
    [out] 统计次数
          若输出值为0，则表明对应指标为无效值。
*   ###返回值：
*       成功：0 
*       失败：其他值
*/
OVD_int32 OVD_get_RTTInfo(OVD_int64 *max_RTT_value, OVD_int64 *min_RTT_value, OVD_int64* mean_RTT_value, OVD_int32 *count);

#ifdef __cplusplus
}
#endif

#endif //OVDOPENAPI_OPENAPI_H
