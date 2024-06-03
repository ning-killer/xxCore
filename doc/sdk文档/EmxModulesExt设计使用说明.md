# EmxModulesExt设计使用说明

# net模块
&emsp;&emsp;net模块负责管理网络设备，以单独服务形式运行，对外提供动态库进行控制与访问。net模块支持各种网卡类型的接入，对应用层提供对网卡的各种配置，例如ip地址，mac，dhcp等，同时会通过topic广播的形式向应用层广播各个网卡的连接状态，包括phy/lan/internet的连接状态

&emsp;&emsp;NetServer类是net模块的实现主体，包含了若干的NetServerDev用来管理具体的网卡，每个NetServerDev包含了一个NetServerLInk负责监测网卡的联网状态

&emsp;&emsp;NetServer的大体流程如下图所示：
<div drawio-diagram="646"><img src="https://docs.eapil.com/uploads/images/drawio/2024-03/ypS80adNo2pIdd0b-drawing-8-1710323869.png"></div>
具体使用的时候应用层通过NetServer.Create创建net模块，通过NetServer.Destroy销毁net模块。NetServer.Create初始化一个loop作为整个net模块的主loop，并且为这个loop分配缓冲区，这里有个骚操作是根据设备烧录的mac.json对网卡的实际mac根据某种规则进行重新配置。注册的RPC-Resp事件用于接收并处理来自应用层的远程调用，这里的调用根据module的不同会被进行分配处理，其中主设备和Dns模块由NetServer处理，如果module等于某个网卡子类型，则由此子类型内部进行处理。注册的Topic-Pub事件用于net模块向外发送Topic广播。注册的Launcher事件用来在loop循环中启动网卡设备。NetServer.Destroy遍历并销毁所有已经创建的NetServerDev并阻塞的退出loop和销毁缓冲区

&emsp;&emsp;两个重要的类NetServerDev和NetServerLInk，相关类图如下：
<div drawio-diagram="647"><img src="https://docs.eapil.com/uploads/images/drawio/2024-03/RFuUQIKUnDRu3hpj-drawing-8-1710397222.png"></div>

## NetServerDev

&emsp;&emsp;NetServerDev是所有网卡需要继承的基类，包含了创建/销毁/地址配置等大量通用实现，并且为每个派生类提供了CreateDev等override接口用于特殊实现。目前net模块实现了3中类型的网卡支持，分别是有线网卡(eth)，WiFi网卡(wifi)，3G/4G等移动网络网卡(mobile)。

&emsp;&emsp;eth类型网卡比较简单，直接继承NetServerDev类即可实现所有功能。

&emsp;&emsp;wifi类型比较特殊，拥有很多特殊接口，例如AP/STA模式的配置和联网等等，均已在NetServerDev的派生类NetServerWifi中实现，这些特殊接口的应用层配置会通过NetServer分发RPC调用到此网卡来实现。另外wifi的联网和配置需要基于第三方用具wpa_supplicant实现，区别于以往使用wpa_supplicant+wpa.conf的方式配置wifi联网，本模块使用wpa_cli的方式。这种方式会更加的高效和灵活，wpa_supplicant依然会被启动作为后台运行的服务，net模块通过wpa_cli.so与wpa_supplicant进行跨进程通信，向wpa_supplicant发送一些列的命令来控制网卡的运行，例如扫描/SSID/PSK等等，并且通过这种方式获取网卡的状态，例如当前运行模式/PSK是否错误等等

&emsp;&emsp;mobile类型的网卡目前处于预留待实现阶段

&emsp;&emsp;NetServerDev除了配置网卡外还有一项很重要的功能是检测网络连接状态，这部分功能是通过成员变量NetServerLink类来实现的

## NetServerLInk

&emsp;&emsp;NetServerLInk创建后会创建Phy的连接监听接口，当Phy的状态发生改变时会触发OnPhy回调，如果Phy连接了，则启动Lan的检测，如果Phy断开连接了，则销毁Lan和Internet的检测并将这两种层次的连接状态置为断开连接

&emsp;&emsp;Phy成功连接会启动Lan检测，Lan的状态改变时会触发OnLanX(X代表4或6，ipv4或ipv6)回调，如果OnLanX连接了，则启动InternetX检测，如果OnLanX断开连接了，则销毁InternetX的检测并将InternetX的连接状态置为断开连接

>**注意:** OnPhy/OnLanX/OnInternetX这些回调需保证只有状态反转的时候才触发，不要相同状态重复触发

### NetServerLinkPhy
&emsp;&emsp;Phy的检测与网卡类型强相关，不同类型的网卡实现方式不同，
### NetServerLinkLan
&emsp;&emsp;NetServerLinkLan的设计比较简单，启动一个定时器，每隔3s获取一下本机地址，成功获取认为lan连接成功，否则认为连接失败
### NetServerLinkInternet
&emsp;&emsp;NetServerLinkInternet用于检测外网是否可以访问，外网地址列表来自配置文件，检测过程比较复杂，整体思路是未连接的状态下走大循环，成功连接状态走小循环，大循环就是DNS解析全部域名->按照域名顺序ping某个域名下的所有ip，直到收到有效的pong进入小循环。小循环就是记录上次成功pong的ip，然后每隔一段时间去ping这单独的ip，循环往复，一旦超时后进入大循环，大循环超时则意味着外网断开连接
<div drawio-diagram="638"><img src="https://docs.eapil.com/uploads/images/drawio/2024-01/qaJurBS9vwmQ3Krs-drawing-8-1705893704.png"></div>


# TimeZone2模块
&emsp;&emsp;EmxCore提供了模块TimeZone用于配置和获取设备时区，配置接口是以IANA提供的时区标准中的时区相对路径的方式实现，例如配置中国大陆的时区，那么配置接口就是"Asia/Shanghai"。这种方式也是众多linux发行版的配置方式，可以提供精确的时区和夏令时配置，但是同样存在一些问题

- 如果需要包含全世界所有地区的时区，那么需要集成大约6M的时区配置文件，有点大
- 时区的配置文件需要定期进行维护更新，主要是由于有些国家的夏令时机制会变化，例如巴西由于政策原因会经常变动，以及1986到1991年间中国其实也是实行过夏令时的，后来取消了
- 不同系统间配置时区会产生理解上的问题，例如安卓/IOS/windows通常有自己的时区配置机制，无法理解或提供类似"Asia/Shanghai"这种时区

&emsp;&emsp;另外，为何不适用TZ环境变量的方式配置时区，这个是由于环境变量的动态配置只对当前进程有效，对其他进程是隔离的无效的，由于EMX是多进程的，所以只能通过localtime的方式

&emsp;&emsp;综上，经过各种考虑，提出了TimeZone2模块，主要目标是解决上面的问题，切入点是IANA组织提供的zic工具，这个工具可以根据一个时区描述文本文件生成一个时区文件，拷贝到为localtime后就可以生效了，所以TimeZone2模块可以根据用户提供的GMT时区偏移以及夏令时规则动态的生成时区文件，zic工具的大小为strip后64KB左右，相比大量的时区配置文件无疑是相当小的

> 可能有人会认为经常用到的时区就那么几个，甚至一个GMT+8就可以解决99%的产品了，几百个字节就可以了，这里我也顺便说一下EMX的设计思想之一，那就是解决一个问题，就尽量解决到没有退路的地步，一次解决，终生受用(理论上0_0)，这很重要

&emsp;&emsp;下面先来了解一下这个时区描述文件，下面是一个简单的时区描述文件，也是本模块动态生成的文件
```
# Rule  NAME    FROM    TO      TYPE    IN      ON      AT      SAVE    LETTER/S
Rule	EMX     1970    max     -       Jan     25      18:14   1:00    D
Rule	EMX     1970    max     -       Jan     25      19:15   0       S

# Zone  NAME        STDOFF   RULES   FORMAT  [UNTIL]
Zone    localtime   8:00     EMX     X%sT

```
首先看`# Zone  NAME        STDOFF   RULES   FORMAT  [UNTIL]`

- Zone代表这是一条时区字段
- localtime指定了这个描述文件同坐zic命令后生成的时区文件名为localtime，本模块固定为localtime
- STDOFF就是时区偏移
- RuLES代表这个时区所使用的规则名称，也就是上面Rule的NAME，本模块固定起名EMX
- FORMAT代表这个时区命名，本模块固定X%sT，类似GMT/CST/EST之类的，中间的%s是动态的，其值取决于Rule中的LETTER/S

再看`# Rule  NAME    FROM    TO      TYPE    IN      ON      AT      SAVE    LETTER/S`

- Rule代表这是一条规则字段
- NAME是这条规则的名字，可以被Zone引用从而生效，本模块固定为EMX
- FROM/TO代表这条规则的生效时间段，本模块固定为从1970年到未来无限长的时间都生效
- TYPE保留字段
- IN生效月份，月份的简写
- ON日的生效规则，这部分比较负责，后面详细说明
- AT生效时间点
- SAVE夏令时的推进时长，上面的例子中夏令时的时候时间向后推进1小时
- LETTER/S给这条规则起个字母名字，例如上面的例子中进入夏令时后的字母是D结束时的字母是S，结合下面Zone中的FORMAT，当进入夏令时后我们使用date命令就会得到"Thu Jan 25 19:14:24 XDT 2024"离开夏令时后date命令就会得到"Thu Jan 25 18:30:21 XST 2024"

&emsp;&emsp;时区描述文件[/tmp/tz.tmp]生成好之后就可以使用zic命令`zic -d /tmp /tmp/tz.tmp`生成/tmp/localtime时区文件

&emsp;&emsp;json配置文件timezone2.json如下
```
{
  "gmt": "GMT+08:00",
  "daylight": {
    "ena": false,
    "save": "1:00",
    "into": { "in": "Apr", "on": "lastSun", "at": "02:00" },
    "leave": { "in": "Apr", "on": "lastSun", "at": "02:00" }
  }
}
```


| 字段<div style="width: 120pt"> | 类型  |含义|
| ------------ | --------------- |----|
| gmt       | string|时区偏移eg."GMT+08:45" |
| daylight  |obj|夏令时相关 |

| daylight字段<div style="width: 120pt"> | 类型  |含义|
| ------------ | --------------- |----|
| ena      | bool|是否使能夏令 |
| save  |string|夏令时调整时常eg."1:10"，调快一个小时零十分钟 |
| into      | obj|进入夏令时的时机 |
| leave      | obj|离开夏令时的时机 |

| into&leave字段<div style="width: 120pt"> | 类型  |含义|
| ------------ | --------------- |----|
| in      | string|规则月份 eg."Jan"/"Feb"/"Aug"... |
| on      | string|规则日期，这部分规则比较复杂 |
| at      | string|规则时间 eg."2:00"，凌晨2点 |


&emsp;&emsp;on规则日期解析，当on为具体数字时，代表这个月的几号，例如"25"代表in这个月的25开始/结束，夏令时一般不是具体几号，而是以某个周日或周几为开始/结束，on支持这种方式，例如"Sun>=1"代表in这个月1号或1号后的首个周日开始结束，"Thu>19"代表in这个月19号后的首个周四开始/结束，另外还可以使用lastXxx规则，"lastXxx"代表这个月的最后一个周几，例如"lastSun"代表最后一个周日