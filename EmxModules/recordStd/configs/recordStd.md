### 配置文件定义

字段<div style="width: 120pt">|类型<div style="width: 20pt">|描述
---|---|---
path|string|录像文件存储顶层目录路径
reservedSpace|uint32|保留的存储空间大小单位MB，当小于此空间时，定时类操作会进行空间释放，非定时类录制操作将无法进行
record|obj|录像相关配置
snap|obj|抓图相关配置

record字段<div style="width: 120pt">|类型<div style="width: 20pt">|描述
---|---|---
vencChn|int32|录像使用的视频编码通道号
aencChn|int32|录像使用的音频编码通道号
mode|int32|录像模式 0:定时录像;1:手动录像;2:事件录像
splitTimeS|int32_t|录像分割时间，取值范围60-300单位秒
timerBaseRecord|obj|定时录像的配置信息
eventBaseRecord|obj|事件录像的配置信息

timerBaseRecord字段<div style="width: 120pt">|类型<div style="width: 20pt">|描述
---|---|---
splitTimeS|int32_t|录像分割时间，录像文件超过此时间后会自动分割成一个新的文件，取值范围60-300单位秒
schedule|obj|排期表

eventBaseRecord字段<div style="width: 120pt">|类型<div style="width: 20pt">|描述
---|---|---
event|obj|关联的事件类型，待补充
schedule|obj|排期表

schedule字段<div style="width: 120pt">|类型<div style="width: 20pt">|描述
---|---|---
Mon|array|星期一的触发时间段，每个元素代表，当天的[start,end]时间，end必须大于start,并且后面元素的start要大于前面元素的end
Tue|array|星期二的触发时间段
Wed|array|星期三的触发时间段
Thu|array|星期四的触发时间段
Fri|array|星期五的触发时间段
Sat|array|星期六的触发时间段
Sun|array|星期日的触发时间段
