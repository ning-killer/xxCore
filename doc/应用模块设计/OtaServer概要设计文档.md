---
html:
    toc: true
    number_sections: true
    toc_depth: 2
    toc_float: true
    smooth_scroll: true
    collapsed: true
    embed_local_images: true
---

# 0.版本信息

| 版本 | <div style="width: 80pt">发布日期 | <div style="width: 80pt">作者 | 修改说明 |
| ---- | --------------------------------- | ----------------------------- | -------- |
| v1.0 | 2024年5月31日                     | hening                     | 初版发布     |
| v1.1 | 2024年6月3日                      | hening                     | 支持日志上报 |


# 1.目的
&emsp;&emsp;本概要设计文档，主要说明OtaServer模块业务流程、编码设计说明。为维护模块迭代流程优化或新增需求协议功能的研发人员提供基础参考文档。

# 2.业务流程说明
![alt text](image-1.png)
&emsp;&emsp;OtaServer业务流程如上图所示：
&emsp;&emsp;1. OtaServer服务Create入口进行服务创建->全量加载配置参数->注册update升级模块的状态回调->判断时间同步（未同步保持定时1s重试）->判断http注册请求host信息等（未成功保持定时30~60s重试）->判断socket连接绑定并监听接收数据（未成功保持定时30~60s重试）->socket发送设备信息->定时维护socket心跳发送服务端。注意：该流程全量绑定依附于main loop中，不会涉及到线程安全的问题。
&emsp;&emsp;其中流程涉及到模块服务管理的全量默认配置开放如下：
```json
{
  "host": {
    "serverUrl": "https://slb.eapil.com", // 目标https请求地址
    "userName": "EAPIL",                  // 目标https请求携带用户名信息
    "userPwd": "eqVARZfmp5sZqJLD",        // 目标https请求携带密码信息
    "heart": 120                          // socket服务维护心跳间隔
  },
  "socket comment": "socket常规配置信息",   
  "socket": {
    "bufferSizeKB": 64                    // socket客户端监听接收数据buffer大小
  },
  "devmsg comment": "信息上报预留拓展项,常规设备可不用配置",
  "devmsg": {
    "templ": "",                          // 全景球
    "iccid": "",                          // 4G
    "chassis_h_ver": "",                  // 硬盘版本
    "gpt_ai_enable": ""                   // gpt ai
  },
  "download comment": "rom下载配置,配置支持文件断点续传时，需要配置flash储存路径fileName",
  "download": {
    "resumable": 0,                        // 0 非断点续传； 1 断点续传
    "fileName": "",                        // 断点续传,存储flash的目标目录文件
    "timeout": 120000                      // 文件下载https连接超时时间 ms
  },
  "aesKey": "G0VB3cMF2QNG0kh8"             // AES ECB PKCS5Padding 加解密key
}
```
&emsp;&emsp;2. 当前socket开启监听接收数据后（绑定在main loop中）->数据接收解包分类处理：1. 接收token失效指令后，通过原子量中断当前在other thread线程的https下载动作，并销毁socketClient，重跑流程1的start动作； 2. 接收到升级指令后，中断当前在other thread线程的https下载动作，解析下载字段信息，根据配置构建断点文件续传或是内存分段处理升级的数据结构，在other thread去进行下载动作并执行对应的升级操作（该下载流程是异步操作，接收后会立刻响应socket应答， 当update模块开始进行升级后，会动态广播下载和升级进度，otaServer模块接收后动态socket发送升级状态）；3. 接收到拓展业务指令后->再次解析进行分类：1. 文件日志上传->根据token等信息get请求文件上传的目标url等信息->打包日志文件压缩包，并向目标的url地址put上传日志文件->socket再上报当前结果。2. GPT AI(暂未支持)。 3. 其他（未新增需求）
&emsp;&emsp;其中流程涉及socket接收和发送的定义如下：
```c++
//!@brief Ota指令
enum OtaSocketCmd {
    SendDevMsg = 0x1a,        //!< 发送设备信息    
    RecvTokenExpired = 0x1b,  //!< 接收token失效指令
    SendHeart = 0x2a,         //!< 发送心跳
    SendUpdateSta = 0x3a,     //!< 发送升级状态：（指令响应|下载进度|升级进度|升级失败）
    RecvUpdateCmd = 0x3b,     //!< 接受升级指令
    SendExpand = 0x4a,        //!< 发送拓展业务信息
    RecvExpand = 0x4b,        //!< 接收拓展业务指令
};

//!@brief 拓展协议类型
enum ExPandType {
    UploadLogFile = 1,        //!< 文件上传
    GPTAI = 2,                //!< GPT AI
};
```

# 3.编码设计说明
![alt text](image-2.png)
&emsp;&emsp;OtaServer编码设计UML类图如上所示：
OtaServerLib类：主要提供OtaServer作为外部模块子线程或者独立进程的启动入口类。依赖OtaServer类，提供服务创建和销毁接口。
```c++
class OtaServerLib {
    public:
        OtaServerLib();
        virtual ~OtaServerLib();

        /**
         * @brief OtaSocket服务创建
         * @param newThread [in] 作为子线程或者依附绑定主线loop独立进程
         */
        void Create(bool newThread);

        /**
         * @brief 释放退出服务
         */
        void Destroy();

    private:
        OtaServer *m_server;
    };
```

OtaServer类：整个服务模块数据交互传递和业务处理调度的中心管控类。依赖一个HttpMgr类来实现对http协议通道的注册、romfile下载、日志文件上传、中断romfile下载等业务操作管控；依赖一个SocketClient类来实现对socket协议通道的创建监听、销毁socket客户端、socket异步发送等业务操作管控，同时创建时对该类注册socket连接、socket断线、数据接收、发送结果回调，以实现对socket状态以及结束数据的整体管控；依赖OtaServerUtil类，主要使用一些静态的公共方法。
```c++
class OtaServer {
    public:
        OtaServer();
        ~OtaServer();

        /**
         * @brief OtaSocket服务创建
         * @param newThread [in] 作为子线程或者依附绑定主线loop独立进程
         */
        void Create(bool newThread);

        /**
         * @brief 释放退出服务
         */
        void Destroy();

    private:
        ErrCodeE ParseConfig();
        void Start();
        void OnSocketClientConnected();
        void OnSocketClientDisConnected();
        void OnSocketRecvResp(const SocketClient::SocketHeader header, const std::string msg);
        void SendCmdMsg(SocketClient::OtaSocketCmd cmd, uint16_t req = 0, std::string data = "");
        void PackDevInfo(std::string &msg, SocketClient::SocketHeader &header);
        void PackHeart(std::string &msg, SocketClient::SocketHeader &header);
        void PackUpdateSta(std::string &msg, SocketClient::SocketHeader &header);
        void PackExpand(std::string &msg, SocketClient::SocketHeader &header, std::string &data);
        bool UnpackUpdateMsg(const std::string &msg, DownLoadRomMsg &loadMsg);
        bool HandleExPandCmd(const std::string &msg, int req);
        void OnUpdateEvent(UpdateEvent::Event &e);
        void HdlUploadLogFile(const std::string &msg, int req);

    private:
        EuvLoop m_loop;
        EuvTimer m_timer;
        OtaHost m_otaHost;
        OtaDeviceInfo m_oatDevInfo;
        OtaSocketMsg m_otaSocketMsg;
        SocketRes m_socketRes;
        HttpMgr m_httpMgr;
        SocketClient m_socketClient;
        SocketClient::Config m_socketCfg;
        DevInfo m_devInfo;
        Sn m_sn;
        Mac m_mac;
        std::string m_aesKey;
        DownLoadRomMsg m_loadMsg;
        struct UpdateMsg {
            UpdateEvent updateEvent;
            char *buffer;
            int bufferSize = 64*1024;
            UpdateEvent::Event event = { };
            OtaUpdateType type = Ack;
        } m_updateMsg;
    };
```

HttpMgr类: 管理https协议通道业务操作。依赖HttpSyncClient类，提供Post(), DownLoadFile()等同步接口实现对注册、romfile下载、日志文件上报的业务操作实现。
```c++
class HttpMgr {
    public:
        static const int UpdateSession = 2230009;

    public:
        HttpMgr();
        ~HttpMgr();

    public:
        /**
         * @brief 注册
         * @param devInfo [in] 服务器注册信息
         * @param host [in] 服务器注册信息
         * @param socketMsg [out] 服务器注册信息
         * @return true 成功; false 失败
         */
        bool Register(const OtaDeviceInfo &devInfo, const OtaHost &host, OtaSocketMsg &socketMsg);

        /**
         * @brief 下载rom文件
         * @param msg [out] 下载rom信息
         * @return true 成功; false 失败
         */
        bool DownloadRomFile(DownLoadRomMsg &msg);

        /**
         * @brief 上传日志文件
         * @param json [in] msg 上传日志信息
         * @return true 成功; false 失败
         */
        bool UploadLogFile(UploadLogMsg &msg);

        /**
         * @brief 中断下载操作
         * @note std::atomic<bool>原子变量可保证线程安全
         */
        void BreakDownload() {
            m_shouldAbort.store(false);
            usleep(100 * 1000);
        }

    private:
        std::string PackRegisterAuthHeader(const OtaDeviceInfo &devInfo, const OtaHost &host);

        static size_t OnRegisterWriteHeader(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnRegisterWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);
        
        static size_t OnDownloadRomWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnDownloadRomWriteFile(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnUploadLogFileWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnUploadLogFileReadData(void* buffer
            , size_t size, size_t nmemb, void* user);
    
    private:
        bool m_created = false;
        std::atomic<bool> m_shouldAbort;
        struct memUserHdl {
            UpdateClient updateClient;
            std::atomic<bool> *shouldAbort;
        } m_memUserHdl;
        struct FileUserHdl {
            std::ofstream *ofs;
            std::atomic<bool> *shouldAbort;
        } m_fileUserHdl;

        std::string m_token;
    };
```

SocketClient类：管理设备端socket客户端的业务。继承于EuvPipeClientBase类&EuvStmClientBase类，封装实现socket连接监听操作、socket数据发送、接收数据并初步解析数据回调响应给OtaServer类等。
```c++
class SocketClient : public EuvPipeClientBase {
    public:
        //!@brief Ota指令
        enum OtaSocketCmd {
            SendDevMsg = 0x1a,        //!< 发送设备信息    
            RecvTokenExpired = 0x1b,  //!< 接收token失效指令
            SendHeart = 0x2a,         //!< 发送心跳
            SendUpdateSta = 0x3a,     //!< 发送升级状态：（指令响应|下载进度|升级进度|升级失败）
            RecvUpdateCmd = 0x3b,     //!< 接受升级指令
            SendExpand = 0x4a,        //!< 发送拓展业务信息
            RecvExpand = 0x4b,        //!< 接收拓展业务指令
        };

        //!@brief 拓展协议类型
        enum ExPandType {
            UploadLogFile = 1,        //!< 文件上传
            GPTAI = 2,                //!< GPT AI
        };

        //!@brief Socket数据包头
        struct SocketHeader {
            char magic[2];  //!< 固定开头 '0','1'
            char ver[6];    //!< 版本号
            uint16_t seq;   //!< 序号
            uint8_t cmd;    //!< 命令
            uint16_t len;   //!< 消息内容长度
        };

        //!@brief 连接状态回调
        using Callback = std::function<void()>;

        /*!
         * @brief 接收到响应数据回调
         * @param cmd [in] 接收到ota指令
         * @param msg [in] 接收到信息
         */
        using RecvRespCallback = std::function<void(const SocketHeader header, const std::string msg)>;

        //!@brief 创建socketclient信息
        struct Config {
            OtaSocketMsg *msg;           //!< socket目标服务器信息
            SocketRes *res;              //!< socket资源
            Callback OnConnected;        //!< 注册一个当与Server成功建立连接后的回调，连接成功后才能发送请求
            Callback OnDisConnected;     //!< 注册一个当与Server断开连接后的回调
            RecvRespCallback OnRecvResp; //!< 注册一个用于接收响应消息的回调
            std::string aesKey;          //!< aeskey
        };

        SocketClient() : EuvPipeClientBase()
            , m_waitHeader(false) {
            memset(&m_header, 0, sizeof(SocketHeader));
            m_waitSize = sizeof(SocketHeader);
            m_data.clear();
            m_data = "";
        }
        virtual ~SocketClient() { }

    public:
        /**
         * @brief 创建目标socket服务链接
         * @param cfg [in] 创建链接socketclient资源信息
         * @return 错误码
         */
        ErrCodeE CreateSocketClient(Config &cfg);

        /**
         * @brief 释放SocketClient
         */
        void DestroySocketClient();

        /**
         * @brief 发起异步发送
         * @param header [in] 发送数据包头
         * @param msg [in] 发送数据
         * @return ErrCodeE 请求端/响应端/请求转发端返回的错误码
         */
        ErrCodeE AsyncSend(const SocketHeader &header, const std::string msg);
        
    protected:
        void OnConnect() override;
        void OnDisConnect() override; 
        void OnRecv(const char *data, int32_t size) override;
        void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override;
    
    private:
        bool m_waitHeader;
        uint32_t m_waitSize;
        SocketHeader m_header;
        std::string m_data;
        Config m_cfg;
    };
```

HttpSyncClient&HttpAsyncClient类：继承于HttpClient类，基于libcurl，实现对https&http协议Post&get&文件上传&文件下载等异步和同步操作。
```c++
class HttpClient {
      public:
        typedef void (*CbWriteData) (void* buffer, size_t size, size_t nmemb, void* user);
        typedef void (*CbWriteHeader) (void* buffer, size_t size, size_t nmemb, void* user);
        typedef void (*CbWriteDebug) (CURL *p, curl_infotype itype, char *data, size_t size, void *user);
        typedef void (*CbReadData) (void* buffer, size_t size, size_t nmemb, void* user);

        //!@brief Http请求方法
        enum HttpPostWay : uint8_t {
            GET = 0, //!< get方式请求
            POST,    //!< post方式请求
        };

        //!@brief Curl代理配置
        //!@note 存在string数据，需要避免memset()操作
        struct CurlProxy {
            bool enable = false;    //!< 代理开关
            std::string addr = "";  //!< 代理addr
        };
        
        //!@brief Post数据信息结构
        //!@note 存在string数据，需要避免memset()操作
        struct PostData {
            std::string url = "";                   //!< 请求url
            std::string param = "";                 //!< 请求参数
            std::string response = "";              //!< 应答数据
            std::string responseHead = "";          //!< 应答header数据
            bool isdebug = false;                   //!< curl日志开关
            HttpPostWay way = HttpPostWay::POST;    //!< 请求方式
            int timeout = 3000;                     //!< 超时时间
            CURLcode code = CURLE_OK;               //!< 请求结果错误码
            CurlProxy proxy;                        //!< 代理信息
            struct curl_slist *header = nullptr;    //!< 请求头信息
            CbWriteData *cbWriteData = nullptr;     //!< 写应答数据回调
            CbWriteHeader *cbWriteHeader = nullptr; //!< 写应答数据头回调
            CbWriteDebug *cbWriteDebug = nullptr;   //!< 写curl日志的回调
            UserOperation *userOperation = nullptr; //!< 用户操作类指针
        };

        //!@brief 文件下载数据信息结构
        //!@note 存在string数据，需要避免memset()操作
        struct DownLoadFileData {
            bool isResumable  = false;              //!< 是否支持断点续传
            std::string fileUrl = "";               //!< 文件下载url
            std::string fileName = "";              //!< 文件名
            void *memUser = nullptr;                //!< 用户内存非断点处理写句柄
            void *fileUser = nullptr;               //!< 用户文件断点处理句柄
            int timeout = 60000;                    //!< 超时时间 ms
            CURLcode code = CURLE_OK;               //!< 请求结果错误码
            CbWriteData *cbWriteData = nullptr;     //!< 写应答数据回调
        };

        //!@brief Put文件信息结构
        //!@note 存在string数据，需要避免memset()操作
        struct PutFileData {
            std::string url = "";                   //!< 请求url
            std::string file = "";                  //!< 目标上传文件
            std::string response = "";              //!< 应答数据
            int timeout = 60000;                    //!< 超时时间
            CURLcode code = CURLE_OK;               //!< 请求结果错误码
            struct curl_slist *header = nullptr;    //!< 请求头信息
            CbWriteData *cbWriteData = nullptr;     //!< 写数据回调
            CbReadData *cbReadData = nullptr;       //!< 读数据回调
        };

      public:
        HttpClient();
        virtual ~HttpClient();
      
      public:
        /**
         * @brief 数据投递上传(适用于常规的POST|GET请求)
         * @param data [in] PostData结构化数据
         * @return true 成功; false 失败
         * @note  调用者可使用FreePostData接口进行释放手动资源
         */
        virtual bool Post(const PostData *data) = 0;

        /**
         * @brief 释放PostData资源
         * @param data [in] PostData结构化数据
         */
        void FreePostData(PostData *data);

        /**
         * @brief 文件下载
         * @param data [in] DownLoadFileData结构化数据
         * @return true 成功; false 失败
         * @note 断点续传和非断点都可配置支持;断点设计文件句柄接收，非断点设计用户句柄接收
         */
        virtual bool DownLoadFile(DownLoadFileData *data) { 
            emxlogi("no support DownLoadFile!\n");
            return false;
        }

        /**
         * @brief 上传文件
         * @param data [in] PutFileData结构化数据
         */
        virtual bool PutFile(PutFileData *data) { 
            emxlogi("no support PutFile!\n");
            return false;
        }

      protected:
        void Send(PostData *data, bool is_free_header = true);
    };

    //!@brief Http同步发送客户端
    class HttpSyncClient : public HttpClient {
      public:
        HttpSyncClient();
        ~HttpSyncClient();

        bool Post(const PostData *data) override;

        bool DownLoadFile(DownLoadFileData *data) override;

        bool PutFile(PutFileData *data) override;

      private:
        std::streampos GetDownloadedFileSize(const std::string& filename);
    };

    //!@brief Http异步发送客户端
    class HttpAsyncClient : public HttpClient {
      public:
        HttpAsyncClient(int max_size = 3);
        ~HttpAsyncClient();

      public:
        /**
         * @brief 创建客户端
         * @param loop [in] curl客户端工作线程池绑定loop
         * @return bool
         */
        bool Create(EuvLoop *loop);

        /**
         * @brief 启动客户端
         * @return bool
         */
        bool Start();

        /**
         * @brief 停下客户端
         * @note 只停下send动作(当次post的动作暂未设计中断)，数据链表未清空
         * @return bool
         */
        bool Stop();

        bool Post(const PostData *data) override;
    
      private:
        void PostMsg(void *arg);
        void PostMsgDone(Emx::ErrCodeE e, void *arg);
        void RunPostWork();
        bool AddMsg(const PostData *data);
        PostData* GetMsg();
        PostData* Clone(const PostData *data);
        int GetTimerInterval();
        void OnMsgSync();
        
      private:
        EuvLoop *m_loop;
        EuvWork m_postWork;
        EuvTimer m_postTimer;
        EuvAsync m_msgSyncInfo;
        int m_maxSize;
        bool m_isStarted;
        bool m_isCreated;
        std::list<PostData*> m_msgList;
        std::mutex m_msgListMtx;
        int m_failedCount;
    };
```
EuvStmClientBase类： 基于libuv和socket实现对socket客户端stream数据流操作的封装类，SocketClient类的封装同理参考Rpc封装实现。

# 4.其他说明
文件目录结构介绍如下：
```shell
├── app               // OtaServer
│   ├── OtaServer.cpp
│   ├── OtaServer.hpp
│   ├── OtaServerLib.cpp
│   ├── OtaServerUtil.cpp
│   ├── OtaServerUtil.hpp
│   └── protocolChn
│       ├── HttpMgr.cpp
│       ├── HttpMgr.hpp
│       ├── SocketClient.cpp
│       └── SocketClient.hpp
├── CMakeLists.txt
├── configs         // 默认配置文件
│   └── OtaServer.json
├── doc             // 默认文档资源
│   ├── OtaServer概要设计文档.md
│   └── OTA 平台协议 V2 0 07e89cd3bd4d4ff5841c84efd958913c.pdf
├── include         // 模块对外头文件
│   └── OtaServerLib.hpp   
├── inner           // 模块内部公共头文件
│   └── ComType.hpp
├── lib             // OtaClient提供外部rpc通信交互实现源码（咱不支持）
└── main.cpp        // main
```

