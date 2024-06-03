/*
 * @Author: xiong
 * @Date: 2022/8/26
*/

#ifndef EMX_UpdateServer_HPP
#define EMX_UpdateServer_HPP

#include "FlashInf.hpp"
#include "UpdateInner.hpp"
#include "UpdateClient.hpp"
#include "EmxBsp.hpp"

namespace Emx {

    class UpdateServer {
    public:
        UpdateServer();
        
        void Create(bool newThread);

        void Destroy();

    private:

        void ResetContext();

        ErrCodeE ParseConfig();

        ErrCodeE GetBootEnvInfo();

        void OnRecv(ErrCodeE e, const char *data, int32_t size, const struct sockaddr *addr);

        void SendAck(const struct sockaddr *addr, ErrCodeE e);

        void UpdateTimer();

        ErrCodeE WriteScript(const char *script, const char *data, int size);

        void PublishEvent();

    private://升级步骤

        ErrCodeE Download(const char *data, int32_t size, const struct sockaddr *addr);

        ErrCodeE Verify();

        void Decrypt();

        ErrCodeE PrepareBurn(const std::string &str);

        ErrCodeE Burn();

        ErrCodeE StreamingUpdate(const char *data, int32_t size, const struct sockaddr *addr);

        ErrCodeE StreamingDownload(const char *data, int32_t size, const struct sockaddr *addr);

        ErrCodeE HandleDecrypt(const char *data, int32_t size, const struct sockaddr *addr);

        ErrCodeE PackBlock(const char *data, int32_t size, const struct sockaddr *addr);
        
        ErrCodeE StreamingBurn();

        void InitDecrypt();

        void Decrypt(char* data, int size, bool isNeedUnPad);

        void OffsetStr(std::string &str, int offset, int size);

        void TestDecrypt();
        void TestOffsetStr();

    private:
        static const uint32_t m_sdcardUpdateSession = 11534941;//SDCardUpdate定义的升级session，用于判断设备升级完成后，不要重启，卡刷由用户自己重启
        FlashInf *m_flash;//flash的基类，指向具体的nor/nand/emmc
        enum class ModeE : uint8_t {
            DoubleSystem = 0,
            Spare,
        } m_mode;
        std::string m_dev;//存储flash分区的前缀路径(例如/dev/mtd)
        int m_firstPart;//分区的起始序号(例如mtd类的是0开始，mmc是1开始)
        bool m_rebootAfterComplete;
        int m_rebootDelayS;
        uint8_t m_buffer[EMX_UPDATE_UDP_BUFFER_SIZE];//用于接收数据
        EuvLoop m_loop;
        EuvUdp m_udp;//使用UDP来接收数据
        EuvTimer m_timer;//用于判断接收超时
        bool m_updating;//是否正在升级
        uint32_t m_session;//存储触发升级的客户端session
        uint32_t m_waitSeq;//期待的包序号，序号不连续判定无效包
        bool m_waitHeader;//文件头信息是否已经全部接收完毕
        std::string m_header;//保存file header
        struct FileHeader {
            uint32_t size;//除FileHeader外，rom应当总大小
            char md5[33];//除FileHeader外，rom应当md5
        } m_fileHeader;//文件头信息
        std::string m_data;//保存json size + json + data
        struct RomInfo {
            int preScriptSize;//是否包含升级前处理脚本
            int postScriptSize;//是否包含升级后处理脚本
            struct Img {
                bool isOpen; //设备是否open文件
                std::string name;//img名称(boot/kernel/media...)
                const uint8_t *data;//用于烧录img的数据指针
                int32_t size;//img的实际大小
                int32_t imgOff;  //烧写偏移量
                int32_t partOff; //擦除块偏移量
                int32_t partSize;//img烧录目标分区的大小
                std::string dev;//img目标分区的设备路径
            };
            std::vector<Img> img;//希望烧录的img数组
        } m_romInfo;//存储结合rom json和partition table一起解析出的用于最终烧录的rom信息
        struct Partition {//uboot env中保存的分区信息
            char name[16];//分区名称(boot/kernel/rootfs/media/app.A/app.B/data...)
            int32_t size;//分区大小
        };
        std::vector<Partition> m_partition;//分区信息数组
        enum class SystemSelectE {
            A,
            B
        } m_systemSelect;//当前使用的是A还是B系统，用于控制rom往另一个分区中烧录
        enum class SystemSpareE {
            Normal,
            Spare
        } m_systemSpare;//当前使用的是正常系统还是Spare系统
        UpdateEvent::Event m_event;//存储当前的升级状态
        int m_publishSize;//用于降低publish的频率，累计到一定程度再pub
        Bsp m_bsp;
        UpdateWayE m_updateWay;     //升级方式
        struct RomJson {
            bool isRecved = false;
            bool isRecvedScript = false;
            bool isRunpreScript = true;
            uint32_t size = 0;
        } m_romJson;
        struct CipherBlock {
            AES_CBC aes;             
            bool isReachEnd = false;
            std::string data = "";   
        } m_cipherBlock;
        int m_deBlockSize;      //解密块大小
        int m_burnBlockSize;    //烧写块大小
        std::string m_partitionEnvName; //partition env环境命令名称
        bool m_eraseallFlash;    //全量擦除flash
    };
};

#endif //EMX_UpdateServer_HPP
