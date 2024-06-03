# EmxCore设计使用说明

<!-- ---
html:
    toc: true
    number_sections: true
    toc_depth: 2
    toc_float: true
    smooth_scroll: true
    collapsed: true
--- -->

# 0.版本信息

| 版本 | 发布日期       | 作者      | 修改说明 |
| ---- | -------------- | --------- | -------- |
| v1.0 | 2022年10月13日 | xiaoxiong | 初版发布 |

# 1.EmxCoreServer
&emsp;&emsp;EmxCore是整个SDK运行的基础，提供EmxCoreServer服务程序,实现了进程间通信的转发服务，参数的加密存取服务，日志的收集和输出服务，提供libEmxCore.so动态库，方便用户使用这些服务。用户使用的时候只需要include头文件EmxCore.hpp以及连接libEmxCore.so动态库即可

&emsp;&emsp;EmxCoreServer需要在所有基于EMX2.0开发的框架之前运行，运行前需要提前配置好如下系统环境变量，例如

```
export EMX_APP_CONFIGS_DIR=/root/firmware/app/configs
export EMX_MEDIA_CONFIGS_DIR=/root/firmware/media/configs
export EMX_DATA_DIR=/root/data
```
其中EMX_APP_CONFIGS_DIR是一般应用程序存放配置文件的顶层目录，EMX_MEDIA_CONFIGS_DIR是EmxMediaServer专用的配置文件顶层目录，EMX_DATA_DIR是可读写的目录，用于程序存放一些可读写的数据文件。之所以需要指定这3条环境变量是因为EMX2.0的所有模块设计中不允许出现绝对路径(/tmp/dev等目录除外)，绝对路径的出现会影响系统的移植性和灵活性

&emsp;&emsp;EMX2.0中关于参数配置文件有两个定义，一个是Param(参数)，另一个是Config(配置),两者都是保存在flash中的某些json文件(EMX2.0中统一使用json来存储参数和配置文件)，但是他们有一些区别。

| Param                             | Config         |
| --------------------------------- | -------------- |
| 加密                              | 不加密         |
| 可动态修改                        | 静态的不可修改 |
| 通过ParamServer提供的接口远程读写 | 直接读取文件   |

EMX_XXX_CONFIGS_DIR指的就是静态的配置文件，这些文件在系统的运行过程中只存在读取操作是不会被修改的，直接使用EasyJson提供的方法指定路径即可读取，Param参数是加密存储的，运行过程中可能会被修改，所有进程使用的Param参数的读写是统一通过EmxCore中提供的Param接口进行的，详细内容可参考3.Param章节


&emsp;&emsp;EmxCoreServer启动时会去EMX_APP_CONFIGS_DIR/core.json中读取配置信息，各项配置信息含义如下：

```
{
  "output": false 
  "log": {
    "shared": {
      "async": false,
      "level": 5
    },
    "normal": {
      "maxSizeKB": 128,
      "path": "/tmp/eapilNormal.log"
    },
    "flash": {
      "maxSizeKB": 32,
      "path": "/tmp/eapilFlash.log"
    }
  },
  "param": {
    "normal": "/root/param/normal",
    "backup": "/root/param/backup",
    "default": "/root/app/configs/param"
  }
}
```

| 字段<div style="width: 120pt"> | 含义                                                                                                                                                                                                           |
| ------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| output                         | 是否启动第三方日志收集，主要用于第三方厂商使用本模块时便于收集日志</br>true:启动，日志服务将收集到的日志通过AF_UNIX的方式转发到固定的端口，日志服务将不进行本地的normal/flash等日志的本地存储</br>false:不启动 |
| log                            | 日志服务相关配置参数                                                                                                                                                                                           |
| log.shared                     | 各个使用日志模块进程间通过共享内存方式共享的配置参数                                                                                                                                                           |
| log.shared.async               | true:以异步的方式进行日志的打印，各个进程日志产生后会非阻塞方式发送到日志服务程序，然后调用printf进行打印</br>false:以同步方式进行日志打印，各个进程日志产生后会直接调用printf进行打印                         |
| log.shared.level               | 0:CRT 紧急</br>1:ERR 错误</br>2:WARN 告警</br>3:INFO 关键</br>4:DEBUG 调试</br>5:TRACE 跟踪                                                                                                                    |
| log.normal                     | 普通日志相关配置参数                                                                                                                                                                                           |
| log.normal.maxSizeKB           | 普通日志文件最大Size，单位KB，-1表示无限制                                                                                                                                                                     |
| log.normal.path                | 普通日志文件存储路径                                                                                                                                                                                           |
| log.flash                      | 持久化存储日志相关配置参数                                                                                                                                                                                     |
| log.flash.maxSizeKB            | 持久化存储日志文件最大Size，单位KB，-1表示无限制                                                                                                                                                               |
| log.flash.path                 | 持久化存储日志文件存储路径                                                                                                                                                                                     |
| param                          | 参数配置服务相关参数                                                                                                                                                                                           |
| param.normal                   | 正常参数保存目录路径                                                                                                                                                                                           |
| param.backup                   | 备份参数保存目录路径                                                                                                                                                                                           |
| param.default                  | 默认参数目录路径                                                                                                                                                                                               |

# 2.Log

&emsp;&emsp;日志服务LogServer是EmxCoreServer的一个线程，启动后会在后台运行，并收集来自各个进程的日志，控制打印等级，进行日志文件的存储，使用此服务的进程通过连接libEmxCore.so而得到一个全局的对象emxLog，此对象被封装为与日志服务程序通信的接口对象。

<div drawio-diagram="582"><img src="https://docs.eapil.com/uploads/images/drawio/2023-03/KN0nfwvoPFN73YfT-drawing-8-1679299004.png"></div>

日志打印可使用如下预定义的简便接口

```c++
emxlogc() //致命日志
emxloge() //错误日志
emxlogw() //警告日志
emxlogi() //关键信息日志
emxlogd() //调式日志
emxlogt() //跟踪日志
```

当服务启动后，客户端可以通过全局对象emxLog的相应接口控制日志的打印等级和同步打印或者异步打印

实例代码

```c++
#include "EmxCore.hpp"
using namespace Emx;

int main(){
    emxlogc("this is a critical log\n");
    emxloge("this is an error log\n");
    emxlogw("this is a warning log\n");
    emxlogi("this is an info log\n");
    emxlogd("this is a debug log\n");
    emxlogt("this is a trace log\n");
    //如果此时想更改一下日志等级为跟踪，注意这个更改是全局的，整个系统的所有进程的打印等级都会改变
    emxLog.SetLevel(Log::LevelE::TRACE);
    //如果想更改为同步打印，这个也是全局的，所有进程都会在elogx函数内部调用printf，异步情况下printf是由LogServer执行的
    emxLog.SetAsync(false);
    return 0;
}

```

# 3.Param

&emsp;&emsp;如1.EmxCoreServer章节中提到的Param(参数)和Config(配置)，实际开发中需要注意他们各自的特性，例如某些敏感信息，需要加密存储，那么就需要使用Param，如果是一些不敏感的信息，也不会去修改它，那么Config更合适，也便于日常调试的时候查看其内容。Config的读写可以使用Core中的Utils模块中的EasyJson::Load(EMX_XXX_CONFIGS_DIR/"name.json")来读取。下面来重点说一下Param，Param模块包含一个ParamServer,ParamServer是EmxCoreServer的一个线程，启动后会在后台运行,负责实现参数的读写等实际操作，使用者通过libEmxCore.so来调用ParamServer的服务。EMX2.0中使用的所有的param都是由EmxCoreServer中的ParamServer服务进行统一加解密读写处理的

&emsp;&emsp;ParamServer集成在EmxCoreServer中，其配置文件也是在EmxCoreServer启动的时候传入，配置文件的内容和含义参看1.EmxCoreServer中的core.json解析。Param的参数中包含了normal/backup/default三个路径，分别用于保存正常参数文件，备份参数文件和默认参数文件，其中默认参数文件是随固件一起打包出来的，一般处于只读分区，只会对其读取，不会写入。当某个参数文件需要被改写的时候，如果normal下已经存在此文件，则将此文件复制到backup目录，然后再写入normal目录，读取的时候也是按照normal->backup->default的顺序，当前一级的文件不存在或无法成功解析时，会读取下一级的目录。其中的normal和backup目录如果flash空间允许的话，建议放置到两个分区中，以便提供更安全的备份功能

&emsp;&emsp;实际使用中可以直接调用Param来进行参数的读写，也可以继承Param后再进行读写

```c++
//直接使用Param来读写mac
ErrCodeE ReadMac() {
    Param param("mac");//mac文件的默认存放路径为core.json中的param.default,当文件修改后，保存在param.normal和param.backup中
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load mac failed\n");
        return ErrCodeE::Failure;
    }
    printf("%s\n",json["mac"].asCString());
    return ErrCodeE::Success;
}
ErrCodeE WriteMac(std::string &mac) {
    Param param("mac");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load mac failed\n");
        return ErrCodeE::Failure;
    }
    json["mac"] = mac;
    if (param.Set(json) != ErrCodeE::Success) {
        emxloge("save mac failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
```

```c++
//继承Param来读写mac
class Mac:public Param {
public:
    Mac():Param("mac"){}
  
    ErrCodeE Read() {
      Json::Value json;
      if (Get(json) != ErrCodeE::Success) {
          emxloge("load mac failed\n");
          return ErrCodeE::Failure;
      }
      printf("%s\n",json["mac"].asCString());
      return ErrCodeE::Success;
    }
  
    ErrCodeE Write(std::string &mac) {
    Json::Value json;
    if (Get(json) != ErrCodeE::Success) {
        emxloge("load mac failed\n");
        return ErrCodeE::Failure;
    }
    json["mac"] = mac;
    if (Set(json) != ErrCodeE::Success) {
        emxloge("save mac failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
}
```

# 4.Gpio

&emsp;&emsp;EmxCore模块提供了Gpio子模块来提供通用GPIO的操作，这里只提供了linux下/sys/class/gpio的方式来操作gpio。

&emsp;&emsp;1.0中的xgpio初始化传入的是bank和pin，后来随着芯片类型的增加发现不同芯片gpioNum的计算都不一样，然后这个接口就被玩儿坏了，现在EMX2.0统一传入最终计算后的gpioNum，也就是最终用来export的num

&emsp;&emsp;Gpio模块将gpio底层的极性进行了封装，将GPIO的电平高低按照实际电路使能的极性进行直传或者反转，最终对外提供的是使能类型的操作，例如控制led的gpio是低电平亮，那么这个gpio就是低有效，配置这个GPIO的极性为`ActiveE::LOW`之后，想控制这个led亮，就只需要`Gpio::SetEna(true)`，屏蔽极性之后，使用者再操作Gpio就比较直观了，所有想让led亮的操作都对应着`Gpio::SetEna(true)`。获取操作类似，例如所有板子的reset按键的按下最终都会对应`Gpio::GetEna==true`

# 5.Utils

&emsp;&emsp;Core模块中的Utils子模块提供了很多有用的小功能，包括
| 功能     | 说明                                      |
| -------- | ----------------------------------------- |
| Base64   | Base64的编解码                            |
| Cmd      | 封装了popen和system这些调用其他进程的接口 |
| Crc      | crc校验的接口                             |
| Crypto   | 一些加密算法                              |
| EasyJson | 对jsoncpp进行了进一步封装，方便json操作   |
| File     | 封装了一些文件操作                        |
| MD5      | MD5操作接口                               |
| Misc     | 不太好分类的一些操作                      |
| Reboot   | 重启操作                                  |
| Sem      | 封装了进程间信号量操作                    |
| Shm      | 封装了共享内存的操作                      |
| Socket   | 封装了socket操作                          |
| Time     | 封装了时间相关的接口                      |

开发过程中需要尽量的使用这些工具进行开发，一方面提高效率，另一方面也可以防止出现bug，其中EasyJson/File/Misc/Time/Socket/Shm/Sem等还是经常需要用到的

# 6.Euv

>EMX2.0的日常编程中会经常使用到Euv提供的各种组件，需要开发人员熟练掌握Euv的使用方法和内部原理

&emsp;&emsp;Euv模块是对开源libuv库的C++封装，其拥有跨平台的特性，内部使用select/epoll/kqueue/IOCP等技术来完成IO的多路复用，EMX2.0的整体架构都是基于此特性构建出来，关于libuv的详细介绍可参考[libuv](http://docs.libuv.org/en/stable/)

&emsp;&emsp;Euv的核心是EuvLoop，一个EuvLoop就是一个线程，是各路IO复用的载体，负责如EuvAsync/EuvDns/EuvTimer/EuvTcp等句柄(事件)的监听和运行，EuvLoop启动运行后，对参与EuvLoop循环的所有句柄的操作都必须在EuvLoop内进行(EuvAsync除外)，否则将面临线程安全的问题。如上如图所示，在EuvLoop启动之前可以注册若干个事件，启动后也可在loop内执行注册事件操作(不可在loop外执行)，EuvLoop内部使用select/epoll等去监听这些注册的事件，举个EuvTimer定时器的例子

```c++
#include "EmxCore.hpp"

// Emx2.0统一使用Emx作为顶级命名空间
using namespace Emx;

//一个定时器的例子
class Timer {
public:
    //启动例子
    void Start() {
        //初始化一个EuvLoop,并将EuvLoop命名为"TimerExample"，
        //这个名字可以通过pstree命令看到
        //使用lambda表达式来定义当此EuvLoop结束时需要做的操作
        //注意：想要EuvLoop顺利退出，必须在这里关闭此EuvLoop下监听的所有句柄才行
        m_loop.Init("TimerExample", [this]() {
            //StopAndDeInit执行后会在某次循环中运行这里
            //销毁timer
            m_timer.Destroy();
        });
        //创建并初始化一个timer，绑定到m_loop上运行
        m_timer.Create(m_loop);
        //启动(注册)timer,将启动时间定为1000ms，表示循环启动后的1s会触发超时回调
        //将重复间隔定位2000ms，表示第一次超时后每隔2s将再次超时
        //使用lambda表达式定义一个timer定时器超时触发的回调，也可使用下面的bind方式
        //m_timer.Start(1000, 2000, std::bind(&Timer::OnTimer,this));
        m_timer.Start(1000, 2000, [this] { OnTimer(); });
        //启动EuvLoop线程，循环开始
        m_loop.Start();
    }

    //结束例子
    void Stop() {
        //结束并关闭一个EuvLoop，此时会导致EuvLoop Init时注册的OnQuit回调被调用
        //然后等待EuvLoop监听的所有句柄都被关闭了，EuvLoop就顺利退出了，否则会一直阻塞在这里
        //此函数可以在EuvLoop之外的线程调用，例如此例子中就是在main线程调用的此函数，而不是EuvLoop线程
        m_loop.StopAndDeInit();
    }

private:
    void OnTimer() {
        //此打印将在EuvLoop启动后1s时打印一次，然后每隔2s打印一次
        printf("%s:%d reached timeout\n", __FUNCTION__, __LINE__);
        //注意类似OnTimer这种注册的回调函数都是运行在EuvLoop线程中的
        //在这些回调函数内部可以使用如m_timer.Create/m_timer.Start/m_timer.Stop/pipe.Create等操作句柄的参数。
        //当EuvLoop运行起来后，在EuvLoop外,也就是其他线程中是不允许操作这些函数的，唯一的例外是async.Send()函数
    }

private:
    EuvLoop m_loop;//定义一个EuvLoop循环
    EuvTimer m_timer;//定义一个定时器
};


int main(int argc, char *argv[]) {
    Timer timer;//定义Timer对象
    timer.Start();//启动Timer
    sleep(8);//等一等
    timer.Stop();//结束Timer
    return 0;
}
```

## 6.1.EuvTimer

&emsp;&emsp;EuvTimer是非常常用的事件句柄(好在它非常的简单)，如上述例子，EuvTimer是一个运行在EuvLoop上的定时器，一个EuvLoop对定义的timer数量并没有限制，原理是通过如select/epoll上的timeout机制和堆排序实现。在EuvLoop的大循环内，我们可以在任何时刻去启动/停止/销毁定时器。经过Destroy的timer再次Create后可以继续使用。

## 6.2.EuvAsync

&emsp;&emsp;EuvAsync提供了一种跨线程的通知方式，其他线程可通过此句柄向该句柄所在的EuvLoop发送简单的通知，触发EuvLoop相应的回调执行。需要注意的是当EuvAsync所在的EuvLoop执行此回调之前，所有的再次EuvAsync通知都将被忽略，直到回调被执行后才会再次监听通知。也就是说，针对同一个EuvAsync句柄，回调执行前的多次通知将会合并成一个触发回调。EuvAsync是唯一一个可以在所属EuvLoop之外线程进行调用触发的句柄,EuvLoop内部就是使用这个句柄，从而实现外部线程发送信号停止此EuvLoop

举个简单的例子

```c++
#include "EmxCore.hpp"

using namespace Emx;

// this class is the simplest usage of async
class AsyncStatic {
public:

    //starting EuvLoop and listening async trigger
    void Start() {
        // create one EuvLoop named 'ThisIsAsyncStaticLoop', it will appear in 'pstree'
        m_loop.Init("ThisIsAsyncStaticLoop", [this]() {
            // destroy async on quit
            m_async.Destroy();
        });

        //create one async and register one callback
        m_async.Create(m_loop, [this]() {
            printf("Async Triggered!\n");
        });

        // run EuvLoop
        m_loop.Start();
    }

    void Stop() {
        // it will be blocked util all opened fd closed
        m_loop.StopAndDeInit();
    }

    // trigger async
    void Send() {
        m_async.Send();
    }

private:
    EuvLoop m_loop;
    EuvAsync m_async;
};

int main(int argc, char *argv[]) {
    AsyncStatic asyncStatic;
    asyncStatic.Start();
    for (int i = 0; i < 5; i++) {
        // send async event out of the m_loop
        asyncStatic.Send();
        sleep(1);
    }
    asyncStatic.Stop();
    return 0;
}
```

## 6.3.EuvPoll

&emsp;&emsp;EuvPoll句柄可以用来监听那些已经打开的fd，比较通用的一个句柄，可以用来监听例如media中的GetVencFd返回的fd并加入到EuvLoop，可以对这个fd进行读/写/连接状态的监听（注：海思支持，晶视182x暂不支持这种方式）。应为相比于Euv提供的其他IO监听的句柄，这个句柄并不会去帮忙处理接收过程，所以像VencFrame的接收本身就不是标准的read，所以只能使用EuvPoll来监听变化，自己手动的接收

举个简单的示例

```c++
void MediaPlatVenc::Start(){
    ...
    m_loop.Init("test", [this]() {
        // destroy async on quit
        m_poll.Stop();
    });
            
    int fd = HI_MPI_VENC_GetFd(0);
    if (fd < 0) {
        emxloge("chn %d HI_MPI_VENC_GetFd failed with %d\n", m_chn, fd);
        return ErrCodeE::Failure;
    }
    m_poll.Init(m_loop, fd);
    m_poll.Start(std::bind(&MediaPlatVenc::OnRecvVencFrame, this), nullptr, nullptr);
    m_paramChange = true;
    m_loop.Start();
    ...
}

void MediaPlatVenc::OnRecvVencFrame(){
    VENC_CHN_STATUS_S stStat = {};
    VENC_STREAM_S stStream = {};
    HI_S32 s32Ret;
    memset(&stStream, 0, sizeof(stStream));
    s32Ret = HI_MPI_VENC_QueryStatus(m_vencChn, &stStat);
    stStream.pstPack = (VENC_PACK_S *) malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    stStream.u32PackCount = stStat.u32CurPacks;
    s32Ret = HI_MPI_VENC_GetStream(m_vencChn, &stStream, HI_TRUE);
    PushFrame(stStream);
    HI_MPI_VENC_ReleaseStream(m_vencChn, &stStream);
    free(stStream.pstPack);
}
```

## 6.4.EuvUdp

&emsp;&emsp;EuvUdp是为UDP的传输提供方便的异步接口。关于这个接口的使用可以参考设备响应配置工具搜索的代码实现，如下所示的不完整代码
```c++
#include "EmxCore.hpp"

namespace Emx {
    class DetectionService {
    public:
        //创建配置工具搜索响应服务
        void Create(Json::Value &json){
            m_port = 4941;//定义需要监听的端口号
            //定义一个buffer用来接收数据
            m_bufferSize = 64 * 1024;//buffer大小
            m_buffer = (char *) malloc(m_bufferSize);//开辟内存
            //初始化一个EuvLoop，命名为DetectionService，注册一个退出回调，当EuvLoop退出的时候销毁udp
            m_loop.Init("DetectionService", [this]() { m_udp.Destroy();});
            //创建udp并绑定到EuvLoop上
            m_udp.Create(m_loop);
            //构造ipv4地址
            sockaddr_in addr = {};
            uv_ip4_addr("0.0.0.0", m_port, &addr);
            //绑定这个地址
            m_udp.Bind((sockaddr *) &addr, 0);
            //由于是广播发送，所以这里配置为广播模式
            m_udp.SetBroadcast(true);
            //注册接收回调，需要提供buffer，buffer的大小，以及接收到数据之后的回调函数（udp会自动的读取数据到m_buffer中）
            m_udp.StartRecv(m_buffer, m_bufferSize,
                            std::bind(&DetectionService::OnRecv, this, 
                            ph_1, ph_2, ph_3, ph_4));
            //EuvLoop开始运行，udp开始监听接收
            m_loop.Start();
        }
        //销毁配置工具搜索响应服务
        void Destroy(){
            //退出EuvLoop
            m_loop.StopAndDeInit();
            free(m_buffer);
        }
    private:
        //接收到数据之后的回调函数
        void OnRecv(ErrCodeE e, const char *data, int32_t size,
                    const struct sockaddr *addr){
            if (e != ErrCodeE::Success)
                return;
            char ip[16];
            uv_ip4_name((sockaddr_in *) addr, ip, 16);
            //这里可以用data，也可以直接用m_buffer，应为他们其实指向的是同一个地址
            auto header = (Header *) m_buffer;
            header->cmd = (CmdE) ntohl((uint32_t) header->cmd);
            header->size = ntohl(header->size);
            if (header->size + sizeof(Header) >= (uint32_t) m_bufferSize) {
                emxloge("recv boardcast cmd %#x from %s, size = %u error %u\n", header->cmd, ip, ntohl(header->size));
            }
            if (header->cmd == CmdE::REQ) {
                emxlogi("recv boardcast cmd %#x from %s\n", header->cmd, ip);
                ... ...//组装需要返回的数据
                //可以把组装后的数据放到m_buffer中，反正现在处理完了里面的数据也没啥用，然后发送出去，目的地是对端地址端口
                m_udp.Send((uint8_t *) m_buffer,
                       (int) respSize + (int) sizeof(Header),
                       (sockaddr *) &addrPeer, sizeof(addrPeer));
            }
        }
    private:
        EuvUdp m_udp;
        EuvLoop m_loop;
        uint16_t m_port;
        char *m_buffer;
        int m_bufferSize;
    };
}
```

## 6.5.EuvStm

&emsp;&emsp;EuvStm是使用libuv并对其进行了封装实现的面向连接类的客户端/服务器编程框架，之前的MsgRpc和MsgTopic的客户端侧和服务端侧都是基于此模块实现的。EuvStm主要做了4件事

1. 把面向连接通信类型的接口进行了抽象，把tcp和pipe抽象成了EuvStreamInf类
2. 基于EuvStreamInf这个抽象接口实现了面向连接类的客户端的编程框架
3. 基于EuvStreamInf这个抽象接口实现了面向连接类的服务端的编程框架
4. 基于上面3点分别实现了tcp和pipe的服务端和客户端的编程框架

&emsp;&emsp;下面逐一的介绍一下这几件事

### 6.5.1.EuvStreamInf类

&emsp;&emsp;假设我想写一个客户端，这个客户端最终可能使用tcp进行通信，也可能使用pipe进行通信，那么我可能需要完整的实现两个客户端代码，但是tcp和pipe是有很多相似性的(都是面向连接的)，所以就把他们的共性提取出来，形成一个EuvStreamInf，这样我在写客户端的时候可以使用EuvStreamInf而不是具体的tcp/pipe,只需要实现一个客户端代码，这样实现完之后，在具体使用的是候，如果是tcp就让EuvStreamInf指向EuvTcp，如果是pipe就让EuvStreamInf指向EuvPipe(即C++的多态)

其中EuvPipe和EuvTcp就是继承了EuvStreamInf后具体的实现，实现了成员函数中的那些virtual虚函数

###  6.5.2.Client端

**EuvStmClientBase**

&emsp;&emsp;EuvStreamInf类已经出来了，下面先使用这个抽象出来的类来实现Client端的编程框架。可以看到，类在构造的时候传入了一个EuvStreamInf \*stm对象指针，后面对具体接口的操作均使用这个指针。EuvStmClientBase自身控制着整个连接过程，使用者只需要Create时填写好Client需要的资源，然后就可以专注于实现自己的功能了，也就是填写protected下面的几个虚函数OnConnect/OnDisConnect/OnRecv/OnSendDone(optional)，是不是很方便？
```c++
    /*!
     * @brief 面向连接的socket通信Client端基类，实现了这种类型客户端的创建销毁发送接收，
     * 内部使用EuvStreamInf抽象接口作为底层通信接口，实际实现客户端的时候，需要传入继承EuvStreamInf的实例
     * 并且最终的客户端需要继承EuvStmClientBase类，实现其各种虚函数。可参MsgRpcRep的实现
     * @see MsgRpcRep
     */
    class EuvStmClientBase {
    public:

        EuvStmClientBase(EuvStreamInf *stm) : m_stm(stm) {
            m_isConnected = false;
        }

        virtual ~EuvStmClientBase() {}

        /*!
         * @brief 创建客户端并连接服务端
         * @param loop [in] 需要绑定的loop
         * @param buffer [in] 需要传入一个buffer，用于接收数据
         * @param size [in] buffer的大小
         * @param serverIp [in] 需要连接的服务端ip
         * @param serverPort [in] 需要连接的服务端端口
         * @param localIp [in] 需要绑定的本地ip，默认不绑定本地ip
         * @param localPort [in] 需要绑定的本地端口，默认不绑定本地端口
         * @return ErrCodeE
         * @see ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop, char *buffer, int32_t size,
                        const char *serverIp, uint16_t serverPort,
                        const char *localIp = nullptr, uint16_t localPort = 0);

        //!@brief 断开链接并销毁客户端
        void Destroy();

        /*!
         * @brief 向服务端发送数据
         * @param data [in] 发送的数据，采用共享指针的方式，方便内存空间的回收
         * @return ErrCodeE
         */
        ErrCodeE Send(std::shared_ptr<std::string> &data);

        ... ...

    protected:

        //!@brief 当与服务端的链接建立成功后触发此函数的调用
        virtual void OnConnect() = 0;

        //!@brief 当与服务端的链接断开后触发此函数的调用
        virtual void OnDisConnect() = 0;

        /*!
         * @brief 当有数据到来时触发此函数的调用
         * @param data [in] 到来的数据
         * @param size [in] 到来的数据大小
         */
        virtual void OnRecv(const char *data, int32_t size) {};

        /*!
         * @brief 当调用Send函数发送的数据成功发送至服务端，或出现错误时触发次回调
         * @param e [in] 发送是否成功
         * @param buffer [in] 发送的数据
         * @param size [in] 发送的数据大小
         */
        virtual void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {};
        ... ...
    };
```
### 6.5.3.Server端

**EuvStmServerXXXBase**

&emsp;&emsp;EuvStmServerHostBase和EuvStmServerClientBase这两个基类实现了服务端的抽象，其中EuvStmServerHostBase负责监听和处理数据流的接收和分发，当监听到新的客户端到来时，会通过虚函数重载的方式，让具体实现EuvStmServerHostBase的子类通过OnAllocClient创建一个继承EuvStmServerClientBase的子对象。EuvStmServerClientBase是accept后的某个客户端的基类，实现对某个具体客户端的数据发送和接收，这里可能比较容易迷糊，关键要理解这个EuvStmServerClientBase到底是个什么东西。这里我详细的解释一下为什么会有EuvStmServerClientBase这个东西，结合通常的socket server端编程，首先server端需要建立一个socket，这里命名为ssock，这个ssock是存储在EuvStmServerHostBase中的，然后我们会让ssock去listen客户端，当客户端到来时再用ssock去accept，accept后会产生一个新的socket，这里命名为csock，这个csock就是存储在EuvStmServerClientBase中，后续针对这个新客户端的数据的收发都是通过csock进行的。另外这个类的对象创建不是由使用者发起的，而是由EuvStmServerHostBase发起，当accept到一个fd之后，EuvStmServerHostBase会去调用OnAllocClient这个虚函数，使用者需要重载这个虚函数，实现EuvStmServerClientBase对象的创建，使用者可以继承EuvStmServerClientBase，实现自己对客户端侧的特殊需求。Server端的两个类定义代码比较长，我不想放进来了，大家可以参考头文件core/euv/EuvStmServerBase.hpp
&emsp;&emsp;直接举例说明，下面的例子分别实现了一个客户端，一个服务端

> 首先是客户端，声明一个客户端类Client，继承自EuvTcpClientBase，EuvTcpClientBase继承自EuvStmServerClientBase，是一个以TCP为底层通信机制的客户端
```c++
class Client : public EuvTcpClientBase {
public:
    //启动客户端
    void Start(EuvLoop &loop) {
        //启动客户端，Create为父类EuvStmClientBase中实现的函数
        //绑定到外部传入的EuvLoop
        Create(loop, m_buffer, sizeof(m_buffer), "127.0.0.1", 9998);
        
        //启动定时器，每200ms向服务器发送一串字符，绑定到同一个EuvLoop
        m_timer.Create(loop);
        m_timer.Start(200, 200, [this]() {
            //client send的数据需要使用共享指针std::make_shared<std::string>，方便内部复用和回收
            auto data = std::make_shared<std::string>("[This is a Tcp client]");
            data->append(1, 0);
            Send(data);
        });
    }

    //关闭timer，销毁客户端
    void Stop() {
        m_timer.Destroy();
        //Destroy为父类EuvStmClientBase中实现的函数
        Destroy();
    }

private:
    //重载父类函数，当客户端连接上服务器时此函数会被调用
    void OnConnect() override {
        printf("%s:%d\n", __FUNCTION__, __LINE__);
    };

    //重载父类函数，当客户端与服务器断开连接时此函数会被调用
    void OnDisConnect() override {
        printf("%s:%d\n", __FUNCTION__, __LINE__);
    };

    //重载父类函数，当客户端有数据需要接收时此函数会被调用
    void OnRecv(const char *data, int32_t size) override {
        printf("Client recv#%s#\n", data);
    };
    
    //重载父类函数，当客户端的一个数据包被发送完成时此函数会被调用
    void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override {
        printf("%s:%d\n", __FUNCTION__, __LINE__);
    };
private:
    char m_buffer[4096];
    EuvTimer m_timer;
};
```

> 然后是服务端，服务端分为host侧的ServerHost和client侧的ServerClient，其中host侧继承自EuvTcpServerHost，用于实现客户端的accept和管理，client侧继承自EuvTcpServerClient，承接accept后的客户端fd。
```c++

class ServerClient : public EuvTcpServerClient {
public:
    ServerClient(EuvTcpServerHost *serverHost) : EuvTcpServerClient(serverHost) {}
};

class ServerHost : public EuvTcpServerHost {
public:
    //重载父类函数，当listen到客户端准备accept的时候此函数会被调用，这里创建了一个
    //ServerClient对象给父类
    EuvStmServerClientBase *OnAllocClient() override {
        printf("ServerHost %s:%d\n", __FUNCTION__, __LINE__);
        return new ServerClient(this);
    }
    
    //当某个客户端连接成功时，给他发送一串字符
    void OnClientConnect(EuvStmServerClientBase *client) override {
        printf("ServerHost %s:%d\n", __FUNCTION__, __LINE__);
        auto data = std::make_shared<std::string>("[This is a Tcp Host]");
        data->append(1, 0);
        client->Send(data);
    }

    //当某个客户端断开连接时，删除此客户端，也就是删除当初连接时OnAllocClient()创建的对象
    void OnClientDisConnect(EuvStmServerClientBase *client) override {
        printf("ServerHost %s:%d\n", __FUNCTION__, __LINE__);
        delete client;
    }

    //接收到来自某个客户端的消息，这里的操作是直接把他断掉
    void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override {
        printf("ServerHost ServerHost recv#%s#\n", data);
        DisConnectClient(client);
    }

};
```

> 三个必要的类ServerHost/ServerClient和Client都定义好了，下面就开始使用，首先创建一个类TestTcpStream来管理客户端和服务端的启动

```c++
class TestTcpStream {
public:
    TestTcpStream() : m_client(nullptr) {}

    //启动服务器和客户端
    void Start() {
        StartServer();
        StartClient();
    }

    //停止服务器和客户端
    void Stop() {
        m_loopClient.StopAndDeInit();
        m_loopServer.StopAndDeInit();
    }

private:
    void StartServer() {
        m_loopServer.Init("TcpStmServer", [this]() {
            m_server.Destroy();
        });
        m_server.Create(m_loopServer, "127.0.0.1", 9998, 8, m_buffer, 4096);
        m_loopServer.Start();
    }

    void StartClient() {
        static int x = 0;
        m_loopClient.Init("TcpStmClient", [this]() {
            m_client.Destroy();
            m_timer.Destroy();
        });
        启动客户端连接服务器
        m_client.Start(m_loopClient);
        m_timer.Create(m_loopClient);
        //客户端每隔500ms就断开连接重新连接服务器
        m_timer.Start(0, 500, [this]() {
            m_client.Stop();
			m_client.Start(m_loopClient);
        });
        m_loopClient.Start();
    }

private:
    //定义两个EuvLoop分别来运行客户端和服务端，当然运行在同一个EuvLoop下面也行
    EuvLoop m_loopClient;
    EuvLoop m_loopServer;
    
    //这里定义的是Client类的对象
    Client m_client;
    
    //这个是Server对象
    ServerHost m_server;
    
    //定义一个定时器玩儿
    EuvTimer m_timer;
    
    //给Server端定义一个buffer
    char m_buffer[4096];
};

int main(int argc, char *argv[]) {
    TestTcpStream test1;
    test1.Start();
    sleep(5);
    test1.Stop();
    return 0;
}
```

&emsp;&emsp;以上演示了基于TCP的Server和Client的用法，基于Pipe的用法同理，这里的EuvPipe底层使用的实际上是基于AF_UNIX的通信机制。Server端的实现可以参考配置工具emxTools中的ConfigService

## 6.6. EuvWork

&emsp;&emsp;EuvWork提供了一种线程池的机制来在EuvLoop之外做一些比较耗时或引起阻塞的操作，注册了work之后会自动将此work挂到线程池中，当work被执行时会调用注册的回调，回调函数完成具体的耗时操作，work结束后会触发另一个回调，表示work完成。例如，我们有个存放音频的目录，想逐一的播放这些音频文件，那么就可以使用EuvWork，让EuvWork来执行比较耗时的播放操作，播放完成后会通知loop，在loop的回调中再次让EuvWork播放下一个音频文件

举个小例子

```c++
#include "EmxCore.hpp"

using namespace Emx;


class Work {
public:
    void Start() {
        //启动一个小EuvLoop
        m_loop.Init("ThisIsWorkLoop", [this]() {
            //EuvLoop结束时把自定义参数的run设为false
            m_arg.run = false;
        });
        
        m_arg.run = true;
        m_arg.hello.assign("Hello");
        
        //把一个work绑定到EuvLoop上并注册执行回调OnWorkCallBack和结束回调OnAfterWorkCallBack
        m_work.Run(&m_loop, &m_arg, std::bind(&Work::OnWorkCallBack, this, ph_1),
                   std::bind(&Work::OnAfterWorkCallBack, this, ph_1, ph_2));
        //EuvLoop开始
        m_loop.Start();
    }

    void Stop() {
        m_loop.StopAndDeInit();
    }

private:

    //这里是运行在EuvLoop之外的，执行一些耗时操作，执行完毕后会回到EuvLoop，触发OnAfterWorkCallBack回调
    void OnWorkCallBack(void *arg) {
        auto myArg = (Arg *) arg;
        if (myArg->run) {
            printf("I'm running on another thread %d [%s]\n", pthread_self(), myArg->hello.data());
            sleep(2);
        }
    }

    //这里是运行在EuvLoop内的，当OnWorkCallBack执行完返回后，会运行到这里
    void OnAfterWorkCallBack(Emx::ErrCodeE e, void *arg) {
        printf("I'm running on main thread %d\n", pthread_self());
      	//work结束后这里的操作是重新再开始一次
        if (m_arg.run)
            m_work.Run(&m_loop, &m_arg, std::bind(&Work::OnWorkCallBack, this, ph_1),
                       std::bind(&Work::OnAfterWorkCallBack, this, ph_1, ph_2));
    }

private:
    //定义一个用户自定义的数据结构体
    struct Arg {
        bool run;
        std::string hello;
    } m_arg;
    
    EuvLoop m_loop;
    EuvWork m_work;
};

int main(int argc, char *argv[]) {
    Work work;
    work.Start();
    sleep(5);
    work.Stop();
}
```

# 7.Msg

>EMX2.0的模块开发中会经常使用到Msg提供的MsgRpc&MsgTopic跨进程调用功能，需要开发人员熟练掌握Msg的使用方法和内部原理

&emsp;&emsp;Msg包含了用于进程间通信的服务，EMX2.0将通信方式抽象为两种，一种是RPC跨进程函数调用MsgRpc，另一种是消息的发布订阅MsgTopic。EmxCore分别提供了MsgRpcServer和MsgTopicServer用于消息在进程间的转发。之所以设立这样一个消息转发中心，主要是由于客户端往往需要即作为一个Server对外提供服务又作为Client请求外部服务，作为服务端的时候需要维护连接到自身的各个客户端的链接状态，处理每一个客户端的连接/断开/消息，这样编程的复杂性会提高，采用中心转发的机制可以让每一个需要与外界通信的进程都连接到MsgXXXServer，这样所有的进程都是一个Client，只需要维护好与MsgXXXServer间的一条链路就好，拓扑结构很清晰，缺点是所有消息通过中心转发一定程度上增加了延时和内存消耗，不过针对大数据（如video/audio）的传输，会提供共享内存+消息转发的机制（类似1.0中的avbuf）。另外，针对于需要广播消息的场景，例如tf卡插拔的事件可能很多模块都需要知道，去中心化的方式就需要tf卡模块管理很多来自其他模块的连接，当有事件发生时，将这个消息一个一个的发送到这些客户端，如果使用中心服务器的话就可以只把消息发送给服务器，广播的操作由中心服务器MsgTopicServer实现，tf卡模块不需要知道其他模块的存在更不需要管理他们。

&emsp;&emsp;MsgXXXServer连接了很多的客户端，如何区分他们并将消息正确的转发到相应的客户端呢？答案是使用某种全局的ID。MsgRpcServer需要知道请求的消息是需要转发给哪个客户端的，也需要知道响应的消息是需要转发给哪个客户端的，对应的MsgTopicServer收到了一条消息之后需要知道这条消息的TopicID是什么，哪些客户端订阅了这个类型的消息。MsgRpc需要ClientID来识别每一个客户端，MsgTopic需要TopicID来识别每一条Topic消息，他们各自具有全局性，需要统一的分配，并且由于ClientID和TopicID其实并没有什么关系，属于两个系统，所以为了简便将他们合并为MsgID来统一分配。下表来自《EMX2.0_ID_Resource》,当有新的模块需要提供Msg服务时，需要编辑此文档，加入新的MsgID
| 前缀<div style="width: 120pt"> | 所属模块    |
| ------------------------------ | ----------- |
| 0x00010000                     | ParamServer |
| 0x00020000                     | MediaServer |
| 0x00030000                     | Tools       |
| 0x00040000                     | Net         |
| 0x00050000                     | Reboot      |
| 0x00060000                     | SDCard      |
| 0x00070000                     | Update      |
  
其中ParamServer/MediaServer等模块都有自己的MsgID，这个ID可以同时用来作为RPC ClientID和TopicID使用，这里只分配了前缀，后面的字段可由模块在自己的内部任意分配，这里有两点需要说明
1. 只有RPC的响应端需要指定全局ClientID，请求端不需要，响应端之所以需要是因为大家需要知道他的全局唯一ID才能给他发消息，请求端不需要是因为可以随机生成(基于某种机制使得他们不会冲突)，只需要请求的时候携带在消息里就可以保证Server能识别到并把Resp响应消息转发回来(具体机制参考源代码)
2. MsgTopic只需要指定全局TopicID，不需要全局ClientID，每个Topic客户端的ClientID也是随机生成的(基于某种机制使得他们不会冲突)，TopicServer只需要记录这些ID对应订阅了哪些Topic，不需要知道他们来自哪个具体模块。

&emsp;&emsp;一般来讲，只有模块的开发者会直接和MsgRpc&MsgTopic打交道，模块需要对外暴露一些接口供其他模块调用，这些接口可能是需要其他进程远程调用，开发者需要屏蔽掉这些远程调用的细节，将远程的请求和远程的响应进行封装，也就是对MsgRpc&MsgTopic进行封装，最终给使用者提供类似本地调用的函数形式

## 7.1. MsgRpc
&emsp;&emsp;MsgRpcServer是EmxCoreServer中的一个线程（loop），负责转发连接到MsgRpcServer的各种客户端(进程)的各种请求消息/响应消息，实现各个客户端之间的跨进程通信。

&emsp;&emsp;为了方便请求端和响应端的编程，MsgRpc的接口中加入了Module和Method的参数。所谓的Module就是一个请求需要对应的子模块ID，是一个uint32_t类型的整数，例如media模块有很多子模块(venc/aenc/snap/isp等)Module的加入有利于media去分发这个请求，假设有个进程需要获取venc的参数，那么就需要发送一个Module等于venc的请求，这样media收到请求后就可以分给子模块venc去处理这个请求并返回。Method类似，也是一个uint32_t代表最终需要调用的函数ID，上面说的获取venc的参数，这个Method就对应着GetParam这个方法，venc收到media分发过来的Method后就知道需要执行GetParam这个方法。所以最终，目标ClientID+目标Module+目标Method三者组合就可以定位到一个具体的远程函数了。当然可能有的模块没有子模块，那么Module也可以不使用，Module和Method都是由模块编写者自己定义和分配的。

&emsp;&emsp;关于远程调用的返回值，MsgRpc内部默认实现了该机制，请求端调用请求后得到的返回值对应响应端执行某个功能函数后的返回值，例如请求端封装了一个函数ErrCodeE GetParam(Param &param)，对应响应端也封装了一个同样的实现具体功能的函数ErrCodeE GetParam(Param &param)，后者执行完毕后的返回值ErrCodeE最终会传递到前者的返回值ErrCodeE中。当然，如果请求的途中出错，比如响应进程挂掉了，没有连接到MsgRpcServer，那么返回值就无法由响应端提供，会由MsgRpcServer给出一个资源不存在的错误码。MsgRpc实现的返回值只能为ErrCodeE类型，需要复杂类型的返回值只能通过参数传递

&emsp;&emsp;MsgRpc为请求端与响应端都封装了接口，方便两种客户端的实现，其中请求端分为同步请求和异步请求，响应端固定为异步

### 7.1.1.同步请求
&emsp;&emsp;**MsgRpcReqSync**同步请求比较简单，直接调用一个静态成员函数MsgRpcReqSync::Request即可。但是同步的方式需要每次建立一个socket,然后以阻塞的方式经历connect-send-recv-close，稍微耗时和占用资源，频繁的请求以及对阻塞非常敏感的loop中建议使用异步方式，MsgRpcReqSync接口如下
```c++
//!@brief 同步阻塞RPC请求
class MsgRpcReqSync {
public:
    /*!
     * @brief 发起同步阻塞RPC请求，比较耗时耗资源，尽量使用MsgRpcReqAsync异步请求
     * @param dstClientId [in] 响应端的ClientID，由响应端定义
     * @param module [in] 目标模块，由响应端定义
     * @param method [in] 目标方法，由响应端定义
     * @param req [in] 请求携带的数据，默认不携带数据
     * @param size [in] 请求携带数据大小，默认携带数据为0
     * @param resp [out] 响应返回的数据，默认不需要返回数据
     * @param timeoutMs [in] 请求超时时间，默认5s
     * @return ErrCodeE 请求端/响应端/请求转发端返回的错误码
     */
    static ErrCodeE Request(uint32_t dstClientId,
                            uint32_t module, uint32_t method,
                            const char *req = nullptr, int32_t size = 0,
                            std::string *resp = nullptr, int32_t timeoutMs = 5000);
};
```
### 7.1.2.异步请求
&emsp;&emsp;**MsgRpcReqAsync**相比于同步请求，异步请求需要绑定到某个EuvLoop才能运行，创建的时候绑定一个接收到响应的回调函数，执行Request后会立刻返回，当有响应消息到达时，会触发注册的响应回调函数。
```c++
//!@brief 异步非阻塞RPC请求
class MsgRpcReqAsync : public EuvPipeClientBase {
public:
  //!@brief 连接状态回调
  using Callback = std::function<void()>;
  /*!
     * @brief 接收到响应数据回调
     * @param e [in] 请求端/响应端/请求转发端返回的错误码
     * @param clientId [in] 接收到的响应端的clientID
     * @param module [in] 接收到的响应端的模块
     * @param method [in] 接收到的响应端的方法
     * @param data [in] 接收到的响应数据
     * @param size [in] 接收到的响应数据大小
     */
  using RecvRespCallback = std::function<void(Emx::ErrCodeE e, uint32_t clientId,
                                              uint32_t module, uint32_t method,
                                              const char *data, int32_t size)>;

  //!@brief 启动异步请求时携带的配置参数
  struct Config {
    EuvLoop *loop; //!<需要绑定的loop
    char *buffer; //!<需要提供一个buffer来接收数据
    int32_t size; //!<buffer的大小
    Callback OnConnected; //!<注册一个当与MsgRpcServer成功建立连接后的回调，连接成功后才能发送请求
    Callback OnDisConnected; //!<注册一个当与MsgRpcServer断开连接后的回调
    RecvRespCallback OnRecvResp; //!<注册一个用于接收响应消息的回调
  };

  /*!
     * @brief 创建异步消息请求体
     * @param cfg [in] 配置参数
     * @return ErrCodeE
     */
  ErrCodeE CreateMsgRpcReqAsync(Config &cfg);

  //!@brief 销毁请求体
  void DestroyMsgRpcReqAsync() { Destroy(); }

  /*!
     * @brief 发起异步请求
     * @param dstClientId [in] 响应端的ClientID，由响应端定义
     * @param module [in] 目标模块，由响应端
     * @param method [in] 目标方法，由响应端定义
     * @param req [in] 请求携带的数据，默认不携带数据
     * @param size [in] 请求携带数据大小，默认携带数据为0
     * @return ErrCodeE 请求端/响应端/请求转发端返回的错误码
     */
  ErrCodeE Request(uint32_t dstClientId, uint32_t module, uint32_t method,
                   const char *req = nullptr, int32_t size = 0);
  ... ...
};
```
### 7.1.3.响应
&emsp;&emsp;**MsgRpcRep**响应都是异步的，都需要绑定到一个EuvLoop下面
```c++
//!@brief 异步非阻塞响应端
class MsgRpcRep : public EuvPipeClientBase {
public:

  //!@brief 连接状态回调
  using Callback = std::function<void()>;
  /*!
     * @brief 接收到RPC请求回调
     * @param module [in] 请求的模块
     * @param method [in] 请求的方法
     * @param data [in] 请求携带的数据
     * @param size [in] 请求携带的数据大小
     * @param out [out] 返回的数据
     */
  using RecvReqCallback = std::function<ErrCodeE(uint32_t module, uint32_t method,
                                                 const char *data, int32_t size,
                                                 std::string &out)>;
  //!@brief 启动异步响应监听时携带的配置参数
  struct Config {
    EuvLoop *loop; //!<需要绑定的loop
    uint32_t clientId; //!<此响应端的ClientID
    char *buffer; //!<需要提供一个buffer来接收数据
    int32_t size; //!<buffer的大小
    Callback OnConnected; //!<注册一个当与MsgRpcServer成功建立连接后的回调
    Callback OnDisConnected; //!<注册一个当与MsgRpcServer断开连接后的回调
    RecvReqCallback OnRecvReq; //!<注册一个用于接收请求消息的回调
  };

  MsgRpcRep() : EuvPipeClientBase() {
    m_waitSize = sizeof(MsgRpc::Header);
    m_waitHeader = true;
    m_clientId = 0;
  }

  /*!
     * @brief 创建异步消息响应体
     * @param cfg [in] 配置参数
     * @return ErrCodeE
     */
  ErrCodeE CreateMsgRpcRep(Config &cfg);

  //!@brief 销毁请求体
  void DestroyMsgRpcRep() { Destroy(); }
  ... ... 
};
```
### 7.1.4.举个例子
&emsp;&emsp;下面的例子分别结合了上面的**RPC同步请求 MsgRpcReqSync**，**RPC异步请求 MsgRpcReqAsync**和**响应 MsgRpcRep**,例子里面涉及到了EuvLoop和EuvTimer，前者定义了一个线程循环loop，后者定义了一个定时器timer，关于Euv的详细信息参考2.2的Euv模块
```c++
#include "EmxCore.hpp"
using namespace Emx;

//首先这里先定义响应端需要的一大堆ID

//模块ID
// defined one responder id
#define RespClientId 0x12400000

//子模块ModuleID+子模块的方法MethodID，这里定义了两组，对应响应端的2个子模块和4个功能函数
//defined module0 and 0 methods 0/1
#define Module0 0x11110000
#define Module0Method0 0x22220000
#define Module0Method1 0x22220001

//defined module1 and 2 methods 0/1
#define Module1 0x33330000
#define Module1Method0 0x44440000
#define Module1Method1 0x44440001

//定义一个响应端
// message RPC responder with async mode
class MsgRpcResponder {
public:
  
    void Start() {
      	//初始化一个loop，名字姑且命名为"MsgRpcResponder"，这个就是给loop所管控的线程命名,方便pstree查看
        m_loop.Init("MsgRpcResponder", [this]() {
            //当loop被销毁时(StopAndDeInit)会进入到这里,关闭打开的组件
            m_rep.DestroyMsgRpcRep();
        });

        //填充一个cfg，用来初始化配置MsgRpcRep
        MsgRpcRep::Config cfg = {};
        cfg.loop = &m_loop;//MsgRpcRep需要绑定一个loop
        cfg.clientId = RespClientId;//MsgRpcRep自身的ID
        cfg.buffer = m_buffer;//给MsgRpcRep一个buffer用来接收数据
        cfg.size = sizeof(m_buffer);//buffer的大小
        //注册一个回调，当MsgRpcRep与MsgRpcServer成功建立后会执行此回调，用lambda表达式的方式赋值回调
        cfg.OnConnected = []() { printf("Rep Connected\n"); };
        //注册一个回调，当MsgRpcRep与MsgRpcServer断开连接后会执行此回调
        cfg.OnDisConnected = []() { printf("Rep Disconnected\n"); };
        //注册一个回调，当MsgRpcRep接收到请求后会执行此函数，用bind的方式注册回调，lambda和bind都可以用可以看心情
        cfg.OnRecvReq = std::bind(&MsgRpcResponder::OnRepRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
        //创建
        m_rep.CreateMsgRpcRep(cfg);
        //启动loop，MsgRpcRep开始运行
        m_loop.Start();
    }

    void Stop() {
        //停止loop
        m_loop.StopAndDeInit();
    }

private:
    //当MsgRpcRep接收到请求后会执行此函数
    ErrCodeE OnRepRecvReq(uint32_t module, uint32_t method,
                          const char *data, int32_t size,
                          std::string &out) {
        emxlogd("recv req module %#x, method %#x, data[%s], size[%d]\n", module, method, data, size);
        //开始对这个请求进行分发
        switch (module) {
            case Module0:
                switch (method) {
                    case Module0Method0:
                    	//这里的out就是最终会发送会请求端的数据，使用string类型作为存储数据的buffer
                        //请求和响应一般来讲是一个人写的，自己需要定义好数据的组装和解析
                        out.assign("resp from Module0Method0");
                        out.append(1, 0);//for the terminal of the string
                    	//这个返回值最终会返回到请求端
                        //针对同步请求就是MsgRpcReqSync::Request的返回值
                        //针对异步请求就是OnRecvResp中的参数ErrCodeE e
                        return ErrCodeE::Success;
                    case Module0Method1:
                        out.assign("resp from Module0Method1");
                        out.append(1, 0);
                        return ErrCodeE::Success;
                    default:
                        emxloge("unsupported method %#x\n", method);
                        break;
                }
                break;
            case Module1:
                switch (method) {
                    case Module1Method0:
                        out.assign("resp from Module1Method0");
                        out.append(1, 0);
                        return ErrCodeE::Success;
                    case Module1Method1:
                        out.assign("resp from Module1Method1");
                        out.append(1, 0);
                        return ErrCodeE::Success;
                    default:
                        emxloge("unsupported method %#x\n", method);
                        break;
                }
                break;
            default:
                emxloge("unsupported module %#x\n", module);
                break;
        }
        return ErrCodeE::ResNotExists;
    }

private:
    EuvLoop m_loop;//定义一个loop
    MsgRpcRep m_rep;//定义一个MsgRpcRep
    char m_buffer[4096];//定义一个buffer
};

// 响应端定义完了，下面定义一个请求端，请求端里面分别使用了同步请求和异步请求
// 异步请求通过定义一个MsgRpcReqAsync m_rpcReqAsync来实现，
// 在m_rpcReqAsync与MsgRpcServer连接成功后发送一个请求消息过去，然后就完了
// 同步请求不需要定义，他是一个静态成员函数，直接调用MsgRpcReqSync::Request即可
// 这里定义了一个定时器，用来定期向响应端发送同步请求消息
// message RPC requester
class MsgRpcRequester {
public:
    void Start() {
        //初始化一个loop
        m_loop.Init("MsgRpcRequester", [this]() {
            //当loop销毁时同步销毁rpc和timer
            m_rpcReqAsync.DestroyMsgRpcReqAsync();
            m_timer.Destroy();
        });

        //填充一个cfg，用来初始化配置MsgRpcReqAsync
        MsgRpcReqAsync::Config cfg = {};
        cfg.loop = &m_loop;//MsgRpcReqAsync需要绑定一个loop
        cfg.buffer = m_buffer;//给MsgRpcReqAsync一个buffer用来接收数据
        cfg.size = sizeof(m_buffer);//buffer的大小
        //注册一个回调，当MsgRpcReqAsync与MsgRpcServer成功建立后会执行此回调
        cfg.OnConnected = [this]() {
            emxlogd("rpcReqAsync connected\n");
            char content[256] = {};
            // request through async
            strcpy(content, "request through async");
            m_rpcReqAsync.Request(RespClientId, Module1, Module1Method0, content, strlen(content) + 1);
        };
        //注册一个回调，当MsgRpcReqAsync与MsgRpcServer断开连接后会执行此回调
        cfg.OnDisConnected = []() { emxlogd("rpcReqAsync disconnected\n"); };
        //注册一个回调，当MsgRpcReqAsync接收到请求后会执行此函数
        cfg.OnRecvResp = std::bind(&MsgRpcRequester::OnRecvResp, this, ph_1, ph_2, ph_3, ph_4, ph_5, ph_6);
        //创建
        m_rpcReqAsync.CreateMsgRpcReqAsync(cfg);
        //初始化并创建一个timer，用来定时发送同步请求
        m_timer.Create(m_loop);
        m_timer.Start(1000, 100, [this] { OnTimer(); });
        //开启loop，timer和MsgRpcReqAsync开始启动
        m_loop.Start();
    }

    void Stop() {
        //停止loop
        m_loop.StopAndDeInit();
    }
private:
    //当MsgRpcReqAsync接收到异步响应后会执行此函数
    void OnRecvResp(Emx::ErrCodeE e, uint32_t clientId,
                    uint32_t module, uint32_t method,
                    const char *data, int32_t size) {
        emxlogd("recv resp client %#x module %#x, method %#x,data[%s] size[%d]\n", clientId, module, method, data, size);
    }
    //当定时器超时时会执行此函数
    void OnTimer() {
        char content[256] = {};
        //request through sync
        strcpy(content, "request through sync");
        std::string resp;
        MsgRpcReqSync::Request(RespClientId, Module0, Module0Method1, content, strlen(content) + 1,
                               &resp, 1000);
        emxlogd("resp=[%s]\n", resp.data());
    }
private:
    EuvLoop m_loop;//定义一个loop
    EuvTimer m_timer;//定义一个timer
    MsgRpcReqAsync m_rpcReqAsync;//定义一个异步Req
    char m_buffer[4096];//定义一个buffer
};

int main(int argc, char *argv[]) {
    MsgRpcResponder rep;
    MsgRpcRequester req;

    rep.Start();
    req.Start();
    sleep(30);
    rep.Stop();
    req.Stop();
}
```

## 7.2.MsgTopic
&emsp;&emsp;MsgTopicServer是EapilCoreServer中的一个线程（loop），负责转发连接到MsgTopicServer的各种客户端(进程)的各种Topic消息，实现各个客户端之间的跨进程通信。

<div drawio-diagram="581"><img src="https://docs.eapil.com/uploads/images/drawio/2023-03/nYBXylcTCLRggDPZ-drawing-8-1678693273.png"></div>

&emsp;&emsp;上图中AppA是一个发布者，AppB&C&D是订阅者，B向MsgTopicServer订阅了123和456这两个TopicID，C订阅了456和789，D订阅了123和789。当A向MsgTopicServer发布了TopicID=123的消息时，MsgTopicServer会将这条消息转发到B和D，当A向MsgTopicServer发布了TopicID=789的消息时，MsgTopicServer会将这条消息转发到C和D。当客户端连接到MsgTopicServer时会携带自己是Publisher还是Subscriber，Subscriber类型的客户端会注册自己喜好的Topic，MsgTopicServer会把这些Topic和客户端的ClientID(客户端自己以某种方式自动生成)绑定，用于后续topic到来时的转发。

&emsp;&emsp;MsgTopic为发布端与订阅端都封装了接口，方便两种客户端的实现，其中发布端分为同步发布和异步发布，订阅端分为异步订阅和同步订阅

### 7.2.1.同步发布
&emsp;&emsp;**MsgTopicPublisherSync**同步发布比较简单，直接调用一个静态成员函数MsgTopicPublisherSync::Publish即可。但是同步的方式需要每次建立一个socket,然后以阻塞的方式经历connect-send-close，稍微耗时和占用资源，频繁的发布消息以及对阻塞非常敏感的loop中建议使用异步方式，MsgTopicPublisherSync接口如下
```c++
/*!
 * @brief 同步阻塞发布消息
 */
class MsgTopicPublisherSync {
public:
    /*!
     * @brief 发送同步阻塞消息，比较耗时耗资源，尽量使用MsgTopicPublisherAsync异步请求
     * @param topic [in] 消息的topic
     * @param data [in] topic携带的数据
     * @param size [in] topic携带数据的大小
     * @param timeoutMs [in] 超时时间，默认1s
     * @return ErrCodeE
     */
    static ErrCodeE Publish(uint32_t topic, const char *data, int32_t size, int32_t timeoutMs = 1000);
};
```
### 7.2.2.异步发布
&emsp;&emsp;**MsgTopicPublisherAsync**异步发布是比较常用的发布方式，因为使用2.0的绝大多数模块都会运行在loop中，那么就只需要在loop中定义一个MsgTopicPublisherAsync，loop中的诸多子模块都可以使用这个MsgTopicPublisherAsync来发布自己的Topic，因为Publiser并不绑定某个Topic
```c++
/*!
 * @brief 用于Topic的异步发布
 */
class MsgTopicPublisherAsync : public EuvPipeClientBase {
public:
    /*!
     * @brief 连接状态回调
     */
    using Callback = std::function<void()>;

    /*!
     * @brief 启动异步发布时携带的配置参数
     */
    struct Config {
        EuvLoop *loop; //!<需要绑定的loop
        Callback OnConnected; //!<注册一个当与MsgTopicServer成功建立连接后的回调，连接成功后才能发布消息
        Callback OnDisConnected; //!<注册一个当与MsgTopicServer断开连接后的回调
    };

    /*!
     * @brief 创建异步Topic发布
     * @param cfg [in] 配置参数
     * @return ErrCodeE
     */
    ErrCodeE CreateMsgTopicPublisherAsync(const Config &cfg);

    /*!
     * @brief 销毁异步Topic发布
     */
    void DestroyMsgTopicPublisherAsync() { Destroy(); }

    /*!
     * @brief 发布一条Topic
     * @param topic [in] 消息的topic
     * @param data [in] topic携带的数据
     * @param size [in] topic携带数据的大小
     * @return ErrCodeE
     */
    ErrCodeE Publish(uint32_t topic, const char *data, int32_t size);
	... ...
};
```
### 7.2.3.同步订阅
&emsp;&emsp;**MsgTopicSubscriberSync**同步订阅在2.0已经实现的模块中使用的比较少，但是某些特殊情况下会需要。同步异步订阅操作里面有两个比较特殊的参数，sendByPack和maxSend，功能是为了让MsgTopicServer控制向本订阅客户端发送数据的发送缓冲区的大小，当发布者发布消息比较频繁的时候，MsgTopicServer不能无限制的进行缓存，需要配置缓存大小，超过限制的消息将被丢掉，注意这里的丢弃只针对某个缓冲区溢出的客户端，对其他客户端接收此消息没有影响。media中的音视频传输会使用这两个参数来控制消息队列的大小。其中的sendByPack说明缓冲区大小的控制方式是按照消息个数来算还是按照字节来算，maxSend就是缓冲区缓存的最大消息包个数或者最大字节数
```c++
/*!
 * @brief 同步阻塞订阅
 */
class MsgTopicSubscriberSync {
public:
    /*!
     * @brief 启动时携带的配置参数
     */
    struct Config {
        bool sendByPack; //!< true代表根据包的数量判断，false代表根据字节数判断
        int32_t maxSend; //!< TopicServer针对此订阅端缓存的最大数据量，单位由‘sendByPack’决定，-1表示无限制
        std::vector<uint32_t> topicArray; //!<需要订阅的topic数组
    };
    /*!
     * @brief 连接TopicServer
     * @param cfg [in] 启动参数
     * @param timeoutMs [in] 超时时间
     * @return ErrCodeE
     */
    ErrCodeE Connect(const Config &cfg, int32_t timeoutMs);

    /*!
     * @brief 获取订阅的消息
     * @param topic [in] 消息的topic
     * @param data [in] topic携带的数据
     * @param timeoutMs [in] 超时时间
     * @return ErrCodeE
     */
    ErrCodeE GetMsg(uint32_t &topic, std::string &data, int32_t timeoutMs);

    /*!
     * @brief 断开与TopicServer的链接
     */
    void DisConnect();
};
```
举个例子
```c++
int main(){
    MsgTopicSubscriberSync::Config cfg;
    //填充配置
    cfg.sendByPack = true;
    cfg.maxSend = -1;
    cfg.topicArray.append(123);//需要订阅topicID==123的消息
    cfg.topicArray.append(456);//需要订阅topicID==456的消息
    MsgTopicSubscriberSync sub;
    sub.Connect(cfg,1000);//连接MsgTopicServer并订阅
    while(true){
        uint32_t topic;
        std::string data;
        //等待消息到来，超时时间1s
    	if(ErrCodeE::Success == sub.GetMsg(topic,data,1000)){
            if(topic == 123){
                //获取到123消息
            	//do sth
            }else if(topic == 456){
                //do sth else
            }else{
               //quit here
               break;
            }
        }
    }
	sub.DisConnect();//最后需要断开连接
}
```
### 7.2.4.异步订阅
&emsp;&emsp;**MsgTopicSubscriberAsync**异步订阅是比较常用的订阅方式，需要绑定到一个loop使用，创建的时候绑定一个回调函数，当有订阅的消息到达时触发回调,创建的时候携带的config里存在的两个参数sendByPack和maxSend与同步调用中一致
```c++
/*!
 * @brief 用于Topic的异步订阅
 */
class MsgTopicSubscriberAsync : public EuvPipeClientBase {
public:
    /*!
      * @brief 连接状态回调
      */
    using Callback = std::function<void()>;

    /*!
     * @brief 接收到订阅消息的回调函数
     * @param topic [in] 消息的topic
     * @param data [in] topic携带的数据
     * @param size [in] topic携带数据的大小
     */
    using RecvMsgCallback = std::function<void(uint32_t topic, const char *data, int32_t size)>;

    /*!
     * @brief 启动异步订阅时携带的配置参数
     * @note 每个订阅端在TopicServer上都对应着一个发送端，可以通过sendByPack和maxSend字段控制这个发送端
     * 缓存的大小，缓存过大当订阅端处理不即时时可能会占用很多内存，分配过小可能导致丢掉一些订阅信息
     */
    struct Config {
        EuvLoop *loop; //!<需要绑定的loop
        bool sendByPack; //!< true代表根据包的数量判断，false代表根据字节数判断
        int32_t maxSend; //!< TopicServer针对此订阅端缓存的最大数据量，单位由‘sendByPack’决定，-1表示无限制
        std::vector<uint32_t> topicArray; //!<需要订阅的topic数组
        char *buffer; //!<需要提供一个buffer来接收数据
        int32_t size; //!<buffer的大小
        Callback OnConnected; //!<注册一个当与MsgTopicServer成功建立连接后的回调
        Callback OnDisConnected; //!<注册一个当与MsgTopicServer断开连接后的回调
        RecvMsgCallback OnRecvMsg; //!<注册一个用于接收响应消息的回调
    };

    MsgTopicSubscriberAsync() : EuvPipeClientBase() {
        m_waitSize = sizeof(MsgTopic::Header);
        m_waitHeader = true;
        m_reg = nullptr;
    }

    /*!
     * @brief 创建异步订阅
     * @param cfg [in] 配置参数
     * @return ErrCodeE
     */
    ErrCodeE CreateMsgTopicSubscriberAsync(const Config &cfg);

    /*!
     * @brief 销毁异步订阅
     */
    void DestroyMsgTopicSubscriberAsync() { Destroy(); }
	... ...
};
```

### 7.2.5.举个例子

&emsp;&emsp;下面举例说明Topic的异步订阅和异步发布的使用方法

```c++
#include "EmxCore.hpp"

using namespace Emx;

//随意定义两个topicID
//defined 2 topics
#define Topic0 0x12345678
#define Topic1 0x87654321

//这里定义一个使用异步订阅方式的客户端
// message topic subscriber with async mode
class MsgTopicSub {
public:
    void Start() {
        //初始化一个loop，名字姑且命名为"MsgTopicSub"，这个就是给loop所管控的线程命名,方便pstree查看
        m_loop.Init("MsgTopicSub", [this]() {
            //当loop被销毁时(StopAndDeInit)会进入到这里,关闭打开的组件，m_subAsync内部使用了Euv组件
            m_subAsync.DestroyMsgTopicSubscriberAsync();
        });
        //填充一个cfg，用来初始化配置MsgTopicSubscriberAsync
        MsgTopicSubscriberAsync::Config cfg = {};
        cfg.loop = &m_loop;//MsgTopicSubscriberAsync需要绑定一个loop
        cfg.sendByPack = true;//按包统计缓冲区大小
        cfg.maxSend = -1;//不限制缓冲区大小
        // subscribe 2 topics, Topic0 and Topic1
        cfg.topicArray.push_back(Topic0);
        cfg.topicArray.push_back(Topic1);
        cfg.buffer = m_buffer;//给MsgRpcRep一个buffer用来接收数据
        cfg.size = sizeof(m_buffer);//buffer的大小
        //注册一个回调，当MsgTopicSubscriberAsync接收到请求后会执行此函数
        cfg.OnRecvMsg = std::bind(&MsgTopicSub::OnMsgRecvSub, this, ph_1, ph_2, ph_3);
        //创建
        m_subAsync.CreateMsgTopicSubscriberAsync(cfg);
        //启动loop，MsgTopicSubscriberAsync开始运行
        m_loop.Start();
    }

    void Stop() {
        //结束loop
        m_loop.StopAndDeInit();
    }

private:
    //注册的回调，当消息到达时触发，这里仅打印一下topicID和消息内容
    void OnMsgRecvSub(uint32_t topic, const char *data, int32_t size) {
        emxlogd("topic[%#x]data[%s]size[%d]\n", topic, data, size);
    }

private:
    EuvLoop m_loop;
    MsgTopicSubscriberAsync m_subAsync;
    char m_buffer[4096];
};

class MsgTopicPub {
public:
    void Start() {
        //初始化一个loop
        m_loop.Init("MsgTopicPub", [this]() {
            //当loop被销毁时(StopAndDeInit)会进入到这里,关闭打开的组件，m_pubAsync内部使用了Euv组件
            m_pubAsync.DestroyMsgTopicPublisherAsync();
            m_timer.Destroy();
        });
        
        //填充一个cfg，用来初始化配置MsgTopicPublisherAsync
        MsgTopicPublisherAsync::Config cfg = {};
        cfg.loop = &m_loop;//需要绑定一个loop
        //注册一个连接回调，当连接上MsgTopicServer时触发
        cfg.OnConnected = []() { emxlogd("m_pubAsync connected\n"); };
        //注册一个断开连接回调，当与MsgTopicServer的连接断开时触发
        cfg.OnDisConnected = []() { emxlogd("m_pubAsync disconnected\n"); };
        //创建
        m_pubAsync.CreateMsgTopicPublisherAsync(cfg);

        //创建一个定时器，每100ms触发一次
        m_timer.Create(m_loop);
        m_timer.Start(100, 100, [this] { OnTimer(); });
        //启动loop，timer和m_pubAsync开始运行
        m_loop.Start();
    }

    void Stop() {
        //退出loop
        m_loop.StopAndDeInit();
    }

private:
    //当定时器超时时触发此回调
    void OnTimer() {
        char content[256];
        strcpy(content, "data from Pub");
        //发送一条Topic0消息，内容是content
        m_pubAsync.Publish(Topic0, content, strlen(content) + 1);
    }

private:
    EuvLoop m_loop;
    EuvTimer m_timer;
    MsgTopicPublisherAsync m_pubAsync;
};

int main(int argc, char *argv[]) {
    MsgTopicSub sub;
    MsgTopicPub pub;

    sub.Start();//开启订阅客户端
    pub.Start();//开启发布客户端
    sleep(5);//跑个5s
    pub.Stop();//然后停止退出
    sub.Stop();
}
``