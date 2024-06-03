#ifndef _ZJ_CAMERAIOT_H_
#define _ZJ_CAMERAIOT_H_
/**
 ** 定义跟摄像头设备相关的IoT事件能力,如:图形侦测报警、声音报警、设备指示灯控制、扬声器控制、本地录制
 **/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
1 添加一个IoT设备,一起设置的是该设备的工作开始/工作结束/获取输入信号/输出信号的回调函数;
2 如果是响应IoT: 比如扬声器: 则不需要注册 start 和 stop 函数,默认开启就行
3 如果是告警IoT: 比如motion: 需要注册 start stop  pfunAIIoTSetProp, 
                然后可以调用 ZJ_IoTEventInPut/ZJ_IotAIEventPVInPut 输出信号
*******************************************************************************************/

/********************************************************************************
1 移动侦测: EN_ZJ_AIIOT_TYPE_MOTION 1000

电子围栏:dx只需要支持人形电子围栏,即EventType:1 ,
prop:{"Motion":{"Sensitive":"灵敏度0-100默认80","Status":"运动侦测开关","Trace":"是否追踪","Interval":"告警间隔"},
      "Human": {"Sensitive":"灵敏度0-100默认80","Status":"人形侦测开关","Trace":"是否追踪","Interval":"告警间隔"},
      "Face":  {"Sensitive":"灵敏度0-100默认80","Status":"人脸抓拍开关","DiscernFlag":"人脸识别(布控)状态","Trace":"是否追踪","Interval":"告警间隔"},
      "Fence": {"EventType":"事件检测类型 0(移动)/1(人形)/2(人脸)","OnlyFlag":"是否只触发电子围栏事件(暂弃)","Status":"电子围栏开关","Direction":"进入区域方向 1(进入)/2(离开)/3(进入或离开)",
                "StayTime":"逗留时长",","StartTime":"当天开始秒数(0)","EndTime":"当天结束秒数(86400)","SpanFlag":"是否跨天","Capture":"图片抓拍开关","Video":"视频抓拍开关",
                "ExpandEventAbility":"拓展事件能力，不支持拓展则该字段缺失，使用掩码或的方式，1.机动车，2.非机动车",
                "Regions":{
                    "RegionNum":"区域坐标数，最多三个",
                    "Regions": [ // 自定义告警区域(电子围栏)坐标。坐标从左上角 0,0 开始，值范围是0~1，保留三位小数
                        {"RegionId":(区域序号)0,"RegionPoints(区域座标)": [{"x": 0.100,"y": 0.001},{"x": 0.100, "y": 0.001},{"x": 0.100, "y": 0.001}]},
                        {"RegionId":(区域序号)1,"RegionPoints(区域座标)": [{"x": 0.100,"y": 0.001},{"x": 0.100, "y": 0.001},{"x": 0.100, "y": 0.001}]}
                    ]}},
      "Car":   {"Sensitive":"灵敏度0-100","Status":"功能开关","Trace":"是否追踪","Interval":"事件检测间隔"},
      "CarNum":{"Sensitive":"灵敏度0-100","Status":"功能开关","Trace":"是否追踪","Interval":"事件检测间隔"},
    }
*********************************************************************************/

/*********************************************************************************
2 内置pir EN_ZJ_AIIOT_TYPE_INNER_PIR 1003
prop:{"Sensitive":"PIR灵敏度设置0-100"}
**********************************************************************************/

/********************************************************************************
3 声音报警 EN_ZJ_AIIOT_TYPE_VOICEALARMDETECT 1004
prop:"{"Sensitive":"灵敏度0-100","Trace":"是否追踪","Interval":"事件检测间隔"}"
*********************************************************************************/

/*******************************************************************************
4 白光灯 EN_ZJ_AIIOT_TYPE_INNER_LAMP 1007
// 如果prop没有 flicker属性 那么在output也没有
"prop":{"Flicker":"白光灯是否为闪烁模式","Brightness","白光灯亮度"} // "Color(暂弃)": 0 白灯 1 红灯,2 蓝灯, 4 黄灯
"Output":{"CtrlType":"白光灯开关","Flicker":"白光灯是否为闪烁模式","Duration":"灯亮的时长(秒)"}
********************************************************************************/

/*********************************************************************************
5 扬声器 EN_ZJ_AIIOT_TYPE_BUZZER 1009
"output": {"CtrlType":"扬声器开关","AlarmType":"报警类型，参考 EN_ZJ_AIIOT_TYPE",
           "SoundName":"指定播放哪个报警声音文件(与AlarmType存在一个就可以)",
           "LoopCnt":"文件播放次数",
           "SoundType":"指定播放声音类型 参考 EN_ZJ_RING_TYPE",
           "SpeakerId":"扬声器id。网关类门铃：0.外机，1.网关内机；单机版默认为0；IPC类：0.默认扬声器，大于1.对应的外接扬声器"}
"prop": {"Volume":"0~100,0为静音，100为最大音量"} // 声音的大小
**********************************************************************************/

/*******************************************************************************
6 录像 EN_ZJ_AIIOT_TYPE_RECORD 1002 厂商不需关注
"prop":{"Status":"本地录像当前状态，0.未录制；1.录制中",
        "RecordLoop":"本地是否循环录像",
        "RecordFull":"本地全天录像标志，1.全天录像；0.按策略进行录像；当设置策略时，由设置者来修改此项取值；",
        "StreamID":"录制码流选择。0.主码流；1.次码流",
        "Duration":"报警录制时长",}
"OutPut":{"CtrlType":"录制操作：0.停止录制；1.开启录制；",
	      "StreamID":"录制码流通道",
	      "Duration":"录制时长(开启事件录制时带有该参数)"
          "RecordMode":"录像模式，1.前7秒后8秒，2.前10秒后20秒"}
********************************************************************************/

/*******************************************************************************
7 本地抓图 EN_ZJ_AIIOT_TYPE_SNAPSHORT 1005 厂商不需关注
"prop":{"Status":"本地抓图当前状态，0.抓图中；1.录制中",
        "LoopFlag":"本地是否循环抓图",
        "Interval":"定时抓图的时间间隔",
        "PicType":"全天抓图的图片尺寸类型",
        "FullFlag":"本地全天抓图标志，1.全天抓图；0.按策略进行抓图；}
"OutPut":{"PicType":"抓图大小",
	      "Interval":"抓图间隔 定时需要,事件 输出不需要",
	      "GifFlag":"是否动图抓图， 事件抓图需要"}
********************************************************************************/

/*******************************************************************************
8 强制白天晚上 EN_ZJ_AIIOT_TYPE_DNSET 1014
"prop":{"IrcutType":""}
"Output":{"CtrlType":"日夜模式 参考: EN_ZJ_IRMODE"}
********************************************************************************/

/*******************************************************************************
9 逗留 EN_ZJ_AIIOT_TYPE_STAY 1016
"prop":{"ActiveTime":"逗留时长 0秒表示实时，>0秒表示滞留N秒"}
********************************************************************************/

/*******************************************************************************
10 碰撞 EN_ZJ_AIIOT_TYPE_COLLISION 1017
"prop":{"Sensitive":"0~100"} 灵敏度可调,数值越高,越灵敏。
********************************************************************************/

/*******************************************************************************
12 跌倒 EN_ZJ_AIIOT_TYPE_FALLDOWN 1024
"prop":{"Sensitive":"灵敏度0-100","Interval":"事件检测间隔"}
********************************************************************************/

/*******************************************************************************
13 跌倒 EN_ZJ_AIIOT_TYPE_FALLDOWN 1024
"prop":{"Sensitive":"灵敏度0-100","Interval":"事件检测间隔"}
********************************************************************************/

/*******************************************************************************
14 火焰 EN_ZJ_AIIOT_TYPE_FLAMEDETECT 1025
"prop":{"Sensitive":"灵敏度0-100","Interval":"事件检测间隔"}
********************************************************************************/

/*******************************************************************************
15 烟雾 EN_ZJ_AIIOT_TYPE_SMOKEDETECT 1026
"prop":{"Sensitive":"灵敏度0-100","Interval":"事件检测间隔"}
********************************************************************************/

/*******************************************************************************
16 哭声 EN_ZJ_AIIOT_TYPE_CRYALARM 1027
"prop":{"Sensitive":"灵敏度0-100","Trace":"是否追踪","Interval":"事件检测间隔"}
********************************************************************************/

/*******************************************************************************
17 客流预警 EN_ZJ_AIIOT_TYPE_HUMANCOUNT 1028
"prop":{"Interval":"事件检测周期", "Count":"检测周期内的检测数量"}
********************************************************************************/

/*******************************************************************************
18 宠物检测 EN_ZJ_AIIOT_TYPE_PETDETET 1029
"prop":{"Sensitive":"灵敏度0-100","Status":"检测状态","Trace":"是否追踪","Interval":"检测间隔","Status":"功能开关"}
********************************************************************************/

/*******************************************************************************
19 电瓶车 EN_ZJ_AIIOT_TYPE_BATTERYBIKE 1031
"prop":{"Sensitive":"灵敏度0-100","Status":"功能开关","Trace":"是否追踪","Interval":"检测间隔",
        "RegionNum":"区域个数","Capture":"抓拍开关","Video":"视频开关",
        "Regions":[ // 自定义告警区域(电子围栏)坐标。坐标从左上角 0,0 开始，值范围是0~1，保留三位小数
            {"RegionId":"区域序号(0)","RegionPoints(区域坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}]},
            {"RegionId":"区域序号(1)","RegionPoints(区域坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}]}
        ]}
********************************************************************************/

/*******************************************************************************
20 高空抛物 EN_ZJ_AIIOT_TYPE_HIGHPARABOLIC 1032
"prop":{"Sensitive":"灵敏度0-100","Status":"功能开关","Interval":"检测间隔","Capture":"抓拍开关","Vedio":"视频开关",
        "RegionNum":"区域个数",
        "Regions":[ // 自定义告警区域(电子围栏)坐标。坐标从左上角 0,0 开始，值范围是0~1，保留三位小数
            {"RegionId":"区域序号(0)","RegionPoints(区域坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}]},
            {"RegionId":"区域序号(1)","RegionPoints(区域坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}]}
        ], 
        "WaterMarkNum":"楼层水印个数 最多15个水印,不超过5个字",
        "WaterMarks":[  楼层水印 (数组 最多15个水印,不超过5个字)
            {"WaterMarkId":"楼层水印序号(0)","FontSize":"字号 6","FontContent":"字体内容 6Floor","WaterMarkPoints(楼层水印坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}}]},
            {"WaterMarkId":"楼层水印序号(0)","FontSize":"字号 8","FontContent":"字体内容 8Floor","WaterMarkPoints(楼层水印坐标)":[{"x":"0.100","y":"0.001"},{"x":"0.100","y":"0.001"}}]}
        ]}
********************************************************************************/

/*******************************************************************************
21 口罩 EN_ZJ_AIIOT_TYPE_MASK 1033
"prop":{"Sensitive":"灵敏度0-100","Status":"功能开关","Trace":"是否追踪","Interval":"事件检测间隔","Capture":"抓拍开关","Vedio":"视频开关"}
********************************************************************************/

/*******************************************************************************
22 烟火 EN_ZJ_AIIOT_TYPE_FLAMEDETECT 1025
"prop":{"Sensitive":"灵敏度0-100","Status":"功能开关","Trace":"是否追踪","Interval":"事件检测间隔","Capture":"抓拍开关","Vedio":"视频开关"}
********************************************************************************/

// 厂商主动调用添加Iot设备
_ZJ_API int ZJ_AddIoTDevice(unsigned int uiAIIoTType, int iCamId,
                            ZJ_PFUN_AIIOT_START pfunAIIoTStart, ZJ_PFUN_AIIOT_STOP pfunAIIoTStop, 
                            ZJ_PFUN_AIIOT_GETINPUT pfunAIIoTGetInput, ZJ_PFUN_AIIOT_OUTPUT pfunAIIoTOutPut,
                            ZJ_PFUN_AIIOT_SETPROP pfunAIIoTSetProp, ZJ_PFUN_AIIOT_CHECKEVENT pfunAIIotCheckEvent);

// 设置IoT设备的默认属性,IoT设备属性以一个JSON字符串传入
_ZJ_API int ZJ_SetIoTDefaultProp(unsigned int uiAIIoTType, int iCamId, unsigned char* pucProp);

// 删除IOT设备属性 (字段)
_ZJ_API int ZJ_DelIoTProp(unsigned int uiAIIoTType, int iCamId, unsigned int uiAIIoTEventId, unsigned char* pucDelProp, unsigned char* pucObjName);

// 增加告警IOT设备策略类型
_ZJ_API int ZJ_AddIoTDefaultPolicy(ST_ZJ_IOT_POLICY_INFO *pstIoTPolicyInfo);
// 删除告警IOT设备策略类型
_ZJ_API int ZJ_DelIoTDefaultPolicy(unsigned int uiInIoTType, int iInCamId);

// 告警IOT设备 策略增加 联动响应的IOT设备
_ZJ_API int ZJ_AddAlarmPolicyOutput(unsigned int uiInIoTType,  int iInCamId,  unsigned int uiInIoTEventId, 
                                    unsigned int uiOutIoTType, int iOutCamId, unsigned char *pucOutIoTParam);
// 告警IOT设备 策略删除 联动响应的IOT设备
_ZJ_API int ZJ_DelAlarmPolicyOutput(unsigned int uiInIoTType,  int iInCamId,  unsigned int uiInIoTEventId, 
                                    unsigned int uiOutIoTType, int iOutCamId);

 // 删除 联动响应的IOT设备的动作属性(字段)
_ZJ_API int ZJ_DelAlarmPolicyOutputProp(unsigned int uiInIoTType,  int iInCamId,  unsigned int uiInIoTEventId,
                                        unsigned int uiOutIoTType, int iOutCamId, unsigned char* pucDelOutputProp);

//IoT设备输入信号接口, 输入信号类型和对应的Json字符串作为信号描述
_ZJ_API int ZJ_IoTInPut(unsigned int uiAIIoTType, unsigned long long lluAIIoTID, char* pcInPut);

//IoT设备事件输入接口,IoT设备为事件单一设备,触发时直接根据属性输出事件;
_ZJ_API int ZJ_IoTEventInPut(unsigned int uiAIIoTType, unsigned long long lluAIIoTID, int iEvent);

// 设置默认录像的属性 或者 调用 ZJ_SetIoTDefaultProp 设置
_ZJ_API int ZJ_SetDefaultRecordProp(unsigned int uiLoopFlag,unsigned int  uiFullFlag,unsigned int uiStreamId);

// 设置本地抓图的 默认属性
_ZJ_API int ZJ_SetDefaultSnapProp(unsigned int uiLoopFlag,unsigned int uiFullFlag,unsigned int uiInterval,unsigned int uiPicType);

// 设置当前设备的工作状态 0 关闭, 1 工作
_ZJ_API int ZJ_SetInIotWorkStatus(unsigned int  uiAotType,unsigned long long lluAotId,int iWorkStatus);

//设置默认的iot策略
_ZJ_API int ZJ_SetIotDefaultPolicy(unsigned int uiIotType, unsigned int uiOpenFlag, unsigned int uiEventId, unsigned int uiTraceFlag, 
    unsigned int uiBuzzerFlag,unsigned int uiSenstive,unsigned int uiRecordFlag, unsigned int uiSnapFlag, unsigned int uiEventFlag, 
    unsigned int uiEventInterval,unsigned int uiDuration, unsigned int uiActiveTime,unsigned int uiSoundType,unsigned char *pucSoundFile);

//设置默认策略（通过结构体入参）,ST_ZJ_DEFAULT_POLICY
_ZJ_API int ZJ_SetIotDefaultPolicyEx(ST_ZJ_DEFAULT_POLICY *pstPolicyInfo);

/***************************************************************
ai图片相关
***************************************************************/
// 检测到ai图片列表调用这个,  不用调用 ZJ_IoTEventInPut   非压缩上传,函数return之后,pstAiPicEventInfo 厂商释放。
// Ai事件信号输入(布控)
_ZJ_API void ZJ_SetAiPicEvent(unsigned int uiAIIoTType,
                              unsigned long long lluAIIoTID,
                              unsigned int uiEvent,
                              ST_ZJ_AIPIC_EVENT_INFO *pstAiPicEventInfo);

// 检测到ai图片列表调用这个,  不用调用 ZJ_IoTEventInPut   压缩上传 ,函数return之后,pstZipEventInfo 厂商释放。
// Ai事件信号输入(抓拍 压缩方式)
_ZJ_API void ZJ_SetAiPicEventEx(unsigned int uiAIIoTType,
                                unsigned long long lluAIIoTID,
                                unsigned int uiEvent,
                                ST_ZJ_AI_ZIP_EVENT_SIGNAL *pstZipEventInfo);

// 注册底图操作相关回调:添加底图、删除底图
_ZJ_API int ZJ_SetAiPicCB(ZJ_PFUN_CREAT_AILABLE pFunCreateLabel,
                          ZJ_PFUN_MODIFY_PICINF pFunModifyPicLabel,
                          ZJ_PFUN_ADDAIPICCB    pFunAddSingleAiPic, 
                          ZJ_PFUN_DELAIPICSCB   pFunDelSingleAiPic,
                          ZJ_PFUN_DELAILABLE    pFunDelLabel);

// 注册回调:释放上报人脸图和背景图的缓存
_ZJ_API int ZJ_SetFreeAiPicCacheCB(ZJ_PFUN_FREEAIPICCACHE pFunFreeAiPicCache);

// 注册函数,图片自动录入开关
_ZJ_API int ZJ_SetAiAutoInputCb(ZJ_PFUN_SETAUTOINPUTFLAG pFunSetAutoInputFlag);

// IoT设备AI告警事件输入接口 (支持上传AI告警图片和视频的接口)
_ZJ_API int ZJ_IotAIEventPVInPut(unsigned int uiAIIoTType,
                                unsigned long long lluAIIoTID,
                                unsigned int uiEvent,
                                ST_ZJ_AI_AlARM_UPLOAD_INF *pstAIAlarmUploadInf);

// 注册回调:释放上报IOT AI告警事件的视频图片的缓存
_ZJ_API int ZJ_SetFreeUploadAIAlarmPVCacheCB(ZJ_PFUN_FREEUPLOADAIALARMPVCACHE pFunFreeUploadAIAlarmPVCache);

// 通知sdkAI告警的视频生成结果 iIsFinishFlag 1:成功 2:失败 (该接口目前只支持AI告警视频上传情况)
_ZJ_API int ZJ_SetUploadAIAlarmPVFileFinish(unsigned char* pucFilePath, int iIsFinishFlag);

// 注册回调: 释放启动前可能存在的缓存
_ZJ_API int ZJ_SetFreeBeforeStartUpCacheCB(ZJ_PFUN_FREEBEFORESTARTUPCACHE pFunFreeBeforeStartUpCache);

// AI互斥接口兼容
// 移动、人形pushflag回调
int ZJ_SetEventPushFlagCb(ZJ_PFUN_SET_EVENT_PUSHFLAG pfunSetEventPushFlag);

#ifdef __cplusplus
}
#endif

#endif
