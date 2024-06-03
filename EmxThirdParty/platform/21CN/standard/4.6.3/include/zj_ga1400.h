#ifndef _ZJ_GA1400_H_
#define _ZJ_GA1400_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
ZJ_GA1400_FACEEVENT_JSONINFO -- 
人脸抓拍事件信息相关JSON  目前人脸抓拍事件不需要传拓展字段
{
    
}
*/

/*
ZJ_GA1400_CAREVENT_JSONINFO
车牌抓拍事件信息相关JSON  目前车牌抓拍事件仅需要传拓展字段:PlateColor
{
    "PlateColor": "车牌颜色(string) 参考颜色类型枚举:EN_ZJ_ZJ_GA1400_COLORTYPE"
}
*/

/*
ZJ_GA1400_PICTURE_JSONINFO
图像对象相关JSON  目前图片对象不需要传拓展字段
{

}
*/


// GA1400颜色类型枚举
typedef enum enum_ZJ_GA1400_COLORTYPE{
    EN_ZJ_ZJ_GA1400_COLORTYPE_BLACK        = 1,   // 黑
    EN_ZJ_ZJ_GA1400_COLORTYPE_WHITE        = 2,   // 白
    EN_ZJ_ZJ_GA1400_COLORTYPE_GRAY         = 3,   // 灰
    EN_ZJ_ZJ_GA1400_COLORTYPE_RED          = 4,   // 红
    EN_ZJ_ZJ_GA1400_COLORTYPE_BLUE         = 5,   // 蓝
    EN_ZJ_ZJ_GA1400_COLORTYPE_YELLOW       = 6,   // 黄
    EN_ZJ_ZJ_GA1400_COLORTYPE_ORANGE       = 7,   // 橙
    EN_ZJ_ZJ_GA1400_COLORTYPE_BROWN        = 8,   // 棕
    EN_ZJ_ZJ_GA1400_COLORTYPE_GREEN        = 9,   // 绿
    EN_ZJ_ZJ_GA1400_COLORTYPE_PURPLE       = 10,  // 紫
    EN_ZJ_ZJ_GA1400_COLORTYPE_CYAN         = 11,  // 青
    EN_ZJ_ZJ_GA1400_COLORTYPE_PINK         = 12,  // 粉
    EN_ZJ_ZJ_GA1400_COLORTYPE_TRANSPARENT  = 13,  // 透明
    EN_ZJ_ZJ_GA1400_COLORTYPE_OTHER        = 99,  // 其他
}EN_ZJ_ZJ_GA1400_COLORTYPE;

// 设置Gat1400能力
_ZJ_API int ZJ_SetGat1400Ability(int iGAT1400Ability);

// 设置Gat1400回调
_ZJ_API int ZJ_SetGa1400CB(ZJ_PFUN_SET_GA1400SWITCH pfunSetGa1400SwitchCb, ZJ_PFUN_SET_GA1400INFO pFunSetGa1400InfoCb, ZJ_PFUN_SET_GA1400STATUS pFunSetGa1400StatusCb);

// Ai事件信号输入(抓拍图)(传到1400平台)  背景图宽高要求1920*1080
_ZJ_API void ZJ_SetAiPicEventEx2(unsigned int uiAIIoTType, unsigned long long lluAIIoTID,
                                 unsigned int uiEvent, ST_ZJ_AIPIC_EVENT_INFO *pstAiPicEventInfo);

#ifdef __cplusplus
}
#endif

#endif