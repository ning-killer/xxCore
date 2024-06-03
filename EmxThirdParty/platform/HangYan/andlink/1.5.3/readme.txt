一、SDK相关文档
1、通过第三方平台间接接入Andlink平台的产品（平台接入），一般而言，摄像头、门铃、门禁等是平台接入
包名形如：andlinkDevSdk_Vx.x.x_dm_Vx.x.x.xxxxxx-$PackageDate-$ProductID-$CompilerName-$SSLVer.tar.gz

    1.1、AndlinkDM SDK(Linux)接入指南(平台接入)：
    https://docs.qq.com/doc/DY3pPSlFtTWZ3bVNN

    1.2、AndlinkDM-SDK(linux c) (平台接入)使用常见问题(FAQ)
    https://docs.qq.com/doc/DUFVLbnVTQllvWk1D

2、直接接入Andlink平台的产品（设备接入），一般而言，音箱、台灯、可视门锁、网关等是设备接入
包名形如：andlinkDevSdk_Vx.x.x.xxxxxx-$PackageDate-$ProductID-$CompilerName-$SSLVer.tar.gz

    2.1、Andlink SDK(Linux)接入指南(设备接入)：
    https://docs.qq.com/doc/DUERlaEJIS0RwYXNH

    2.2、Andlink-SDK(linux c) （设备接入）使用常见问题(FAQ)
    https://docs.qq.com/doc/DUHBpUVljbXhteHJr

3、若接入指南和FAQ都无法解决你的问题,可以参考FAQ4.2通过邮件进行提问.

二、工程说明
1.这是提供给厂商使用,可链接andlink库进行二次编译的小工程.

2.工程目录结构如下:
demo(厂商名称)/
|------adapt   厂商需要适配的文件
        |------andlink_adapt.c 集成andlink库,需要适配的文件
        |------xxx_adapt.c 集成其他库,需要适配的文件
        
|------include 头文件存放目录
        |------andlink_adapt.h main.c或andlink_adapt.c需要依赖的头文件,厂商可根据需要更改;
        |------andlink_pub.h   andlink对外提供的头文件,厂商只可引用,不可更改
        
|------lib     库文件存放目录
        |------libandlink.so/a.1.x          andlink库;无须依赖其他库;
        |------libandlink-core.so/a.1.x     andlink核心库;
        须依赖-lcoap -lpaho-mqtt3as -ljson  -lcurl  [-lalsec]  -lssl -lcrypto 或-lcoap -lmqttclient -ljson  -lcurl  [-lalsec]  -lmbedtls -lmbedcrypto -lmbedx509

        
|------bin      可执行demo程序生成目录
|------main.c   主函数所在文件
|------lib.mak  adapt目录需要依赖的Makefile文件
|------Makefile 主Makefile文件
|------run.sh   编译脚本,用于指定工具链路径和名称


三、demo程序验证
1、简单验证demo程序是否可用方法
(1)将bin目录下的demo文件+lib目录下的libandlink.so.1.x 2个文件推送到设备文件系统某个目录下(举例:/data);
(2)登陆到设备后台,并进入到/data目录
(3)创建一个so的软连接;ln -s libandlink.so.1.x libandlink.so
(4)运行demo程序;LD_LIBRARY_PATH=./ ./demo wired 192.168.1.18 192.168.1.255; 
这两个IP是设备的IP和广播地址,测试阶段,可以不做修改.
通过日志查看,若可以成功连接到杭研云平台,说明程序OK;此时厂商可开发自己的应用程序,来集成andlink能力.

注:这是拿到andlink SDK后,第一步要的做的事情.

2、二次编译demo的方法
(1)修改run.sh脚本,指定工具链路径和名称;
(2)执行脚本./run.sh clean  清除编译环境
(3)执行脚本./run.sh all


四、正式移植的方法
1、添加头文件:include/andlink_pub.h

2、添加链接库:
    2.1设备接入产品,有三种链接方式可选:
        2.1.1所有版本:-landlink
        2.1.2通用版本:-landlink-core  -lcoap -lpaho-mqtt3as -ljson  -lcurl  [-lalsec]  -lssl -lcrypto 
        2.1.3轻量版本:-landlink-core  -lcoap -lmqttclient   -ljson  -lcurl  [-lalsec]  -lmbedtls -lmbedcrypto -lmbedx509

    2.2平台接入产品,有三种链接方式可选:
        2.2.1所有版本:-landlink
        2.2.2通用版本:-landlink-core  [-lcoap ]  -ljson  -lcurl   -lalsec   -lssl -lcrypto 
        2.2.3轻量版本:-landlink-core  [-lcoap ]  -ljson  -lcurl   -lalsec   -lmbedtls -lmbedcrypto -lmbedx509

3、参考main.c和adapt/andlink_adapt.c 调用SDK的普通接口,注册和实现SDK的回调接口.


五、其他事项说明

1、libandlink.so/a无外部库的依赖，其内部打包了如下库:
-landlink-core + -lcurl + -ljson + [-lpaho-mqtt3as(lmqttclient)] + [-lcoap] + -lssl -lcrypto (-lmbedtls -lmbedcrypto -lmbedx509)
所以四.2.1.1与四.2.1.2(2.1.3)是等价的,四.2.2.1与四.2.2.2(2.2.3)是等价的,用户可以根据是否需要复用公共库,自由选择.

2、sdk使用的开源库版本如下:
libcoap-4.1.1、curl-7.64.1(平台对接:curl-7.58.0)、paho.mqtt.c-1.3.0(mqttclient-master)、cJSON-1.7.11、openssl-1.1.0h(mbedtls-2.4.2) 

六、版本更新记录
## 2019.05.20 [v0.1初版调试]


## 2019.07.01 [v1.0初版提测]
1.实现了《中国移动And-link协议规范（设备云网关管理分册）v2.0》.


## 2019.09.17 [v1.1.0提测]
1.支持云网关接入新方案《Andlink设备接入实现要求及补充协议V2-190905.docx》.
2.新增了远程诊断、日志分级上传、自检、周期诊断等功能.
3.新增了一机一密功能.
4.简化了OTA方案.


## 2020.04.26 [v1.3.1提测] 
1.新增快联信道(qlink)无感配网功能
(1)支持优先使用qlink方式接入IoT路由器，一旦接入失败，等待用户(APP)配网，接入云网关.
2.简化了对外接口,提升了用户使用体验.
3.优化了多项细节,提升了性能和稳定性.


## 2020.06.18 [v1.3.2提测] 
1.新增引导信道(guidelink)无感配网功能.
2.优化无感配网流程:
(1)优先尝试连接qlink完成配网;
(2)qlink连接失败,连接约定的引导热点完成配网;
(3)引导热点连接失败,启动softAP,等待APP发现.
3.新增密码同步功能.
4.支持普通组网路由器给设备配网.


## 2020.09.16 [v1.4.0提测] 
1.新增了本地诊断功能.
2.集成了中国移动终端公司的DM SDK(richinfo-lwm2msdk-M-v2.0.tar.gz).
3.支持了SDK启动默认userkey的注册.
4.对外接口更改:
adl_dev_attr_t中增加厂商名称字段deviceVendor,假设厂商需要升级SDK,注意头文件andlink_pub.h的同步更新;启动SDK时,需要新增此字段.
5.优化了示例工程(demo),提升了用户使用体验.


## 2020.12.30 [v1.4.2提测] 
1.优化了示例工程(demo),支持主程序对strip后的静态库的依赖.
2.优化了本地诊断功能,支持日志输出级别和日志输出方向的控制.
3.新增了有线设备以子设备的形式接入IOT路由器的逻辑.
4.新增了子设备OTA升级接口.
5.修复了环境中多台coap服务导致的coap广播收不到响应的问题.
6.对外接口更改:对外接口adl_dev_callback_t中增加厂商名称字段扩展升级接口,增加读写配置项回调接口,假设厂商需要升级SDK,注意andlink_pub.h的更新.
7.优化了多项细节,增强了安全性,提升了性能和稳定性.


## 2021.07.14 [V1.5.0提测]
1.对外开放了andlink封装的json编解码能力.
2.优化了本地诊断功能:(1)分开日志文件和配置文件的存储路径;(2)日志文件默认存储到内存;(3)日志文件存储路径见adlLogDebug.conf文件.
3.支持了mbedTLS方式进行链路认证,目前支持mbedTLS-2.4.2版本.
4.对外开放了限制andlink日志文件大小的接口.
5.对外提供了sdk是否支持一机一密的查询接口:getDeviceInfoStr(ADL_AUTH_MODE).
6.新增了OTA文件下载功能;对外开放了设置升级文件存储路径及扩展名设置接口.
7.支持了平台接入Andlink的逻辑,主要支持配网和DM上报.
8.优化了多项细节,提升了稳定性,能力实现了原子化;平台接入SDK支持了线上编译.


## 2021.10.25 [v1.5.1发布] 
1.新增了远程诊断功能.
2.优化了示例代码工程(demo),提升了用户使用体验.
3.修复了扫码绑定失败,设备重启设备直接显示绑定成功的BUG.
4.优化了多项细节,提升了性能和稳定性.


## 2021.12.09 [v1.5.2发布] 
1.发布连接组件的首个轻量版本.
与普通版本(依赖openssl-1.1.1c)相比,SDK(含所有依赖的第三方组件),X86实测,flash占用下降62%，达到1.56MB；内存占用下降52%，达到2.5MB.
2.修复了CPU休眠期间离线解绑失败问题,注意这种场景与断网或关机期间的离线解绑逻辑不同.
3.优化了多项细节,提升了性能和稳定性.


## 2022.05.22 [v1.5.3.230522发布] [V1.5.3_dm_V1.0.2.230522发布] 
1.新增了一机一密缓存目录配置项；增强了IP获取回调接口的安全性.
2.新增了私有版本号,提升了SDK的可维护性.
3.对外开放了用于调试的set_printLog_debug_level接口,仅调试阶段可用,商用阶段不允许使用.
4.平台接入版本:格式化了设备ID,DM SDK按照CMCC-deviceType-deviceMac构造设备ID.
5.设备接入版本:链路维护状态增加了设备复位流程判断;修复了即将上线成功状态,设备复位失败的BUG.
6.设备接入版本:修复了轻量版本,断网链路重建程序崩溃问题.
7.支持了27位的一机一密串码.
8.显式设置了andlink各个任务优先级.
9.优化了配网流程中coap广播发送流程,实现非阻塞方式coap广播发送.
10.优化了多项细节,提升了性能,稳定性和可维护性.


## 2022.06.13 [v1.5.3.220613] [V1.5.3_dm_V1.0.2.220613] 
1.对外接口更改:对外接口ADL_DEV_STATE_e中新增了重新绑定的状态,有线设备以此为时机实现绑定失败逻辑.


## 2022.09.20 [v1.5.3.220920] [V1.5.3_dm_V1.0.2.220920] 
1.规范化了DM上报流程中的6项参数,cpuModel、romStorageSize、ramStorageSize、locationInfo、wlanMac、deviceIP 
这六项参数改为必填;若设备不存在此参数,则填"NONE".


## 2022.09.26 [v1.5.3.220926] [V1.5.3_dm_V1.0.2.220926] 
1.根据最新云网关协议,子设备注册和上线消息中,添加了andlink协议版本号.
2.新增了主动上报其下子设备属性信息的接口childDevDataReport,与设备本身属性上报接口devDataReport加以区分.
3.新增了主动上报其下子设备DM信息的接口childDevDmReport.


## 2022.10.13 [v1.5.3.221013] [V1.5.3_dm_V1.0.2.221013] 
1.针对平台对接的产品,增加了对ssl:mbedtls-2.28.1的支持.


## 2022.10.19 [v1.5.3.221019] [V1.5.3_dm_V1.0.2.221019] 
1.对外接口更改:设备数据上报接口devDataReport入参中添加timestamp,删除childDevId.
2.对外接口更改:子设备数据上报接口childDevDataReport增加timestamp.
3.上述timestamp表示事件发生的时间戳,用于区分当前事件还是历史事件,可用于支持离线事件上传.
4.规范了DM上报流程中moduleType、networkType字段的含义,针对wifi进行统一表述为:Wi-Fi.

## 2023.05.22 [v1.5.3.230522] [V1.5.3_dm_V1.0.2.230522][patch]
1.针对平台接入SDK的DM上报流程,实现了日志文件脱敏.
2.关闭了预约诊断功能.
3.关闭了平台对接的OTA.