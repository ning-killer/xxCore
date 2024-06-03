# EMX2.0介绍

&emsp;&emsp;EMX2.0是第二代IPC固件端工程编程框架
## 1. 架构

&emsp;&emsp;EMB1.0的架构比较老旧，总结主要有如下的几点问题

- media层的代码复用率低
- 各个模块充斥着大量不太合适的宏定义
- 存在反向依赖的问题
- 单进程的模式，某个模块的崩溃会导致整个系统需要重启(由于芯片media SDK的回收机制不完善)
- 各个模块的启停响应速度比较慢，需要等待sleep或select延时
- 某些模块接口定义不合适 导致移植的改动比较大
- 以平台为工程仓库的基本单位将导致仓库充斥大量与平台无关的修改
<div drawio-diagram="575" align=center><img src="https://docs.eapil.com/uploads/images/drawio/2022-12/zTJ4Ir4CVoEc6s6V-drawing-8-1672045002.png"></div>
<div align=center>EMB1.0架构图</div>

&emsp;&emsp;针对如上的这些问题，提出了EMX2.0架构，2.0架构主要特点是

- 全面使用C++实现，提高代码复用率，优化接口实现
- 基于libuv库的IO多路复用，采用事件驱动架构，大量的采用事件回调的方式实现功能，提高模块响应速度与系统的稳定性
- 采用多进程方式代替单进程，减少各个模块之间的耦合，各个进程间通过EmxCoreServer通信
- 各个模块大量使用配置文件，减少模块的重复编译，很多模块以app和lib.so方式提供
- 单独提取了board模块，每套硬件有自己的ID，配置文件和代码，方便移植
- 单独提取了media模块，以二进制的server和lib.so形式提供，供其他模块和应用程序调用
- 对工程结构做了优化

<div drawio-diagram="576" align=center><img src="https://docs.eapil.com/uploads/images/drawio/2022-12/cdDU6axeWd8WrFdW-drawing-8-1672045073.png"></div>
<div align=center>EMX2.0架构图</div>
&emsp;&emsp;上图为EMX2.0架构，箭头代表依赖关系，如图所示：

- **Hardware层：** 整个架构的最底层，是各个芯片厂商提供的芯片例如Hi3518EV300/T31X/CV1821等
- **CoreMedia层：** 也就是核心&媒体层，包含两部分，其一是core模块，提供了EmxCoreServer服务程序+libEmxCore.so动态库+EmxCore.hpp头文件，实现了通用的基础功能，如Euv组件，日志，参数存储，消息转发和一些小工具等，整个架构几乎所有的模块都需要依赖此模块，其二是media模块，用于屏蔽各个厂商的各种芯片的差异，实现媒体功能，向其他模块提供统一的媒体接口，提供了EmxMediaServer服务程序+libEmxMedia.so动态库+EmxMedia.hpp头文件
- **Modules层：** Modules提供了很多通用的基础模块，这些基础模块有些会提供服务程序和对应的动态库供其他模块调用，有些会仅提供动态库或静态库，这些模块不存在对具体软件平台或硬件平台的特异性依赖，Modules模块内部可存在互相依赖，之后新的模块加入也需要遵守这种依赖关系。
- **Platform层：** 对接流媒体平台，实现平台的所有功能
- **Product层：** 是各个具体的产品，依托于Platform层并实现自身的特殊性部分

## 2. EMB1.0vsEMB2.0编程

&emsp;&emsp;基于2.0的编程思路相较于1.0有很大的差别，举个简单的例子，我们想将一路视频保存到tf卡中并按照60s进行文件分割，当前的做法是这样的。

```c++
//伪代码
bool time_out = false;//60s超时标志

void rec_save_thread(){
    media_stream_open();
    while(!quit){
    	//获取视频帧，超时时间2s
    	media_stream_get(2000);
        //超时后创建新的文件来存储视频
        if(time_out){
            time_out = false;
            recreate_file();
        }
        save_frame_to_file();
        media_stream_release();
    }
    media_stream_close();
}

//每60s将time_out赋值为true
void rec_ctrl_thread(){
	while(!quit){
    	sleep(1);
        time(&time_cur);
        if(time_cur-time_rec > 60){
            time_rec = time_cur;
            time_out = true;
        }        
    }
}

int main(){
    //启动两个线程一个用于获取并存储视频，另一个负责控制每1分钟进行文件的切片
    thread_start(&save, rec_save_thread);
    thread_start(&ctrl, rec_ctrl_thread);
    ...
    //停止两个线程
    thread_stop(&save);
    thread_stop(&ctrl);
}
```
&emsp;&emsp;2.0的平台下应该这样实现。
```c++
//伪代码
class Record {
public:
	Create(){
    	m_loop.Init();//初始化一个loop(线程)
        m_timer.Create(m_loop);//初始化一个定时器，绑定到loop上
        m_timer.Start(0,60000,OnTimer());//启动定时器，初次超时时间为0s，然后每60s超时一次，超时后执行OnTimer回调
        m_venc.Create(m_loop);//初始化一个视频编码接收对象，绑定到loop上
        m_venc.Start(OnGetFrame(Frame));//启动监听视频帧的到来，当有视频帧到来时执行OnGetFrame回调
        m_loop.Start()//启动loop，注意前面的timer.Start和venc.Start都是在这个函数执行完后才真正开始生效
    }
    Destroy(){
    	//结束loop
    	m_loop.DeInit();
    }
    //当定时器触发时，此回调被执行，创建一个新文件
    OnTimer(){
    	ReCreateNewFile();
    }
    //接收到一个视频帧的时候触发此回调，将frame写入文件
    OnGetFrame(Frame frame){
    	SaveFrameToFile(frame)
    }
private:
    EuvLoop m_loop；//定义一个loop对象
    EuvTimer m_timer；//定义一个定时器对象
    MediaVenc m_venc；//定义一个视频编码接收对象
}

int main(){
    //创建一个Record对象rec
    Record rec;
    rec.Create();
    ...
    rec.Destroy();
}
```
&emsp;&emsp;可以看到最明显的一个区别就是1.0使用C语言，2.0使用C++，之所以使用C++主要是为了提高代码的可复用性和可扩展性，想想1.0中media模块中大量重复的代码，很多都是可以提取出来直接继承的。C++提供的继承和多态特性是重构代码的利器，用C来实现是很费劲很丑陋的，当然使用C++也会对编程能力的要求有所提高(这里包含对语法的熟悉以及编程思维的转换)，不过2.0中使用的只是C++的一些基本特性，很少量的C++11特性。对于C++不是很熟练的瞳学，建议看看焦哥的枕边书《C++ Primer》。

&emsp;&emsp;还有一个比较明显的区别就是命名规则不同0_0，1.0的代码采用下划线命名法，即每个单词之间使用下划线连接，而2.0的代码函数使用帕斯卡命名法(所有单词首字母大写)，变量使用骆驼命名法(除第一个单词外首字母大写),类的私有成员变量使用骆驼命名法，但是前面要加"m_"。关于编码规范，参考EMX2.0的《固件端开发规范》中的代码编写部分

&emsp;&emsp;关于响应速度，注意1.0代码中的两个延时，一个是ctrl中的1s的sleep一个是media_stream_get时2s的timeout，这意味着最坏情况下结束这个录像功能需要>3s的时间，当然如果把sleep和timeout的延时都降低成10ms，那么可以在20ms左右推出，不过众所周知这会极大的浪费系统调度资源，线程会频繁的唤醒和休眠/挂起。相比之下2.0的代码会在一个线程调度周期内(ms量级)就可以顺利的退出，并且几乎不会浪费系统调度资源。其实现方式是监听了一个退出的信号，当loop接收到这个信号之后会立刻触发一个退出事件回调，在回调中结束所有监听的事件然后顺利退出

&emsp;&emsp;关于线程，1.0创建了2个线程，而2.0只创建了1个线程(m_loop.Start())，当然1.0也可以把ctrl线程和save线程合并入同一个thread，但是当需要合并的线程越来越多，线程的代码会变得非常复杂。2.0提供了IO多路复用的框架，以及很多实用的组件，可以让之前需要多线程处理的工作很方便的变为单线程处理，从而节省大量的系统资源。节省系统资源是一方面，更令人欣喜的是，当多线程变为单线程之后，完全不需要考虑线程间的同步以及临界区的问题了，我们可以很确定的知道，当前正在码的这个函数如果在运行，那么其他所有在这个loop中的函数都处于未执行的状态，只有这个函数完整的执行完毕，才有可能运行另一个函数，所有函数都可以放心的读写运行在这个loop中的同一个buffer或者同一个变量(撒花)。

&emsp;&emsp;关于编程模型，1.0的代码中相信很多人都会使用某种轮询的机制，例如上面例子中的ctrl线程，相当于每隔1s轮询一下有没有超时，又如我们在配网的时候会不停的轮询网络的连接状态，这种线程+轮询的模型无疑是低效且迟钝的，例如我们在轮询网络状态的时候如果用户按了reset键打算重新配网，这个时候我们会等待线程退出后才能再次进入配网状态(等待的时间取决于我们轮询的频率)。而2.0会大量的使用事件驱动的模型，loop的退出是一个事件，超时是一个事件，网络状态的改变是一个事件，reset按键的按下是一个事件，我们的loop一直处于挂起的状态(不消耗系统资源)等待这些事件，而不是一遍遍的去确认这些事件是否发生。编程模型的改变看似是调用流程的改变，其实是编程思维的改变，从面向过程跟着函数一步一步的走，到等着事件的到来一个一个的处理，从关注着数据如何安全高效的互斥访问，到关注自身的loop如何不被阻塞，从咔咔的疯狂创建线程，到咔咔的疯狂添加事件，这一系列的改变在使用2.0框架的时候都会遇到

## 4. 单进程vs多进程
&emsp;&emsp;例如web页面/配置工具/平台SDK都需要获取设备当前的运行状态，可能是一次性获取也可能是需要实时监控
<div drawio-diagram="577" align=center><img src="https://docs.eapil.com/uploads/images/drawio/2022-12/nF3cO5uum5AeY9g7-drawing-8-1672045452.png"></div>
&emsp;&emsp;上图的左侧是1.0的情况，web页面/配置工具/平台sdk都是在一个进程里面，大家通过调用类似NetGetStatus函数获取网络的状态。2.0的web/配置工具/平台SDK都是单独的进程，并且Net模块也是单独的进程，各个进程通过libNet.so这个动态库与NetServer进程进行通信。1.0的网络状态由epmain管理，2.0则由NetServer自行管理。

>为何采用多进程，而不是单进程？
1. **稳定性：** 1.0采用单进程模式，例如media/sdcard/web/eptool/app/env/rtsp等等模块，均是集成在epmain中，这导致一个问题，随着需要的功能模块的增加，epmain会变得越来越庞大，越来越不稳定，任何模块的异常都会导致整个系统的全面崩溃。2.0使用多进程模式，类似web/eptool/app/rtsp功能，其实是各自独立的，并没有什么需要耦合的功能，因此将他们拆分为多个单独的进程，每个进程的崩溃不会影响到其他进程。
2. **快速恢复：** 1.0的单进程模式下，如果某个模块例如rtsp出现某些异常，那么整个epmian会崩溃，进而带着media模块一起崩溃，从而需要重启整个系统才能恢复(media的崩溃可能无法单纯重启epmain来恢复)，这可能需要花费几十秒的时间。2.0的多进程模式下，如果rtsp崩溃了，那么只有此功能会暂时不可用，其他功能正常使用，并且仅需要重启rtsp进程即可恢复它的功能，花费不到1s，并且影响范围很小。另外，1.0的所有参数都是由epmain管理，epmain负责启动的时候将所有参数读取出来，并配置给各个模块，而2.0的参数由各个模块自行管理，例如rtsp模块，RtspServer启动的时候会自己从flash中读取参数配置，media模块的MediaServer启动的时候也会从flash中读取参数配置，这样利于模块进程崩溃后重启恢复。不仅仅是启动的时候加载参数，参数更改后的存储也由模块各自管理(通过core模块中的Param子功能)。

## 5. 进程间通信
&emsp;&emsp;既然2.0采用的是多进程方式，那么不可避免的需要进程间的通信，2.0将通信方式抽象为两种，一种是RPC跨进程函数调用，另一种是消息的发布订阅，分别对应着2.0提供的MsgRpc和MsgTopic，详细的机制和用法参考Core模块中的Msg部分

## 6. 关于编程框架
&emsp;&emsp;这种编程方式或者说是设计模式在2.0中普遍存在
### 6.1 何谓编程框架？

&emsp;&emsp;编程框架就是我知道做这件事的大体流程是什么样的，流程我帮你写好，甚至流程里面某些我认为固定的操作也帮你写好，但是流程里面某些操作可能每个人实现方式都不一样，我就把它空着，等着使用者来填充，也就是C++设计模式中的template方法。编程框架的实现可以用虚函数继承或者注册回调函数实现，例如各个流媒体平台的SDK，这些SDK里面实现了整体流程和部分功能，但是某些具体的操作，例如获取设备的一张缩略图，各个厂商的实现方式都不一样，只能通过回调函数实现。

### 6.2. 为什么要使用编程框架？

&emsp;&emsp;编程框架存在的意义是让使用者可以方便的实现一个功能，统一的框架可以让实现的功能变得更加可靠稳定(取决于框架编写者的能力),减少大量重复的代码，稍微限制了一下使用者天马行空的想象力0_0

### 6.3. 举个例子
&emsp;&emsp;假设我们想实现一个TCP服务器，一般来讲是要像下面这样写
```c++
//伪代码
int main(){
    int sock = socket();//创建socket
    //配置地址信息
    addr.ip = "192.168.1.1";
    addr.port = 1234;
    bind(sock,addr);//绑定地址
    listen(sock);//监听客户端的连接
    while(true){
    	int newClient = accept(sock);
        //新的客户端到来，开一个线程去处理这个连接
        createThread(ProcClient,newClient);
    }
}

//接收数据，处理数据，发送数据的线程
void ProcClient(newClient){
    while(true){
      data[256];
      recv(data);
      ...
      send(data);
    }
    close(newClient)
}
```
此时假设有一个Server框架，那么可以这样写
```c++
//伪代码
int main(){
    //定义一个带有框架的对象
    Server server;
    //创建
    //"192.168.1.1"，1234：需要绑定的ip和端口
    //OnNewClient：当新客户端到来时需要执行的函数
    //OnRecv：当某个客户端接收到了数据之后需要执行的函数
    server.Create("192.168.1.1",1234,OnNewClient,OnRecv);
}

//处理新客户端的到来
void OnNewClient(newClient){
	...
}

//处理某个客户端接收到的数据
void OnRecv(newClient,data){
  ...
  send(data);
}
```
&emsp;&emsp;可以看到，使用编程框架的时候，我们不需要去按照标准流程的去创建绑定监听接收，因为这套流程是很常规的操作，框架会帮你实现好。这里的核心问题是，流程的把控者是谁？如果仅仅是封装了一些函数供使用者调用，那么并不是框架，实际的调用流程依然是使用者来控制。只有使用者不需要也不能够控制流程，只能够实现流程中的某些步骤，类似做填空题一样才是框架