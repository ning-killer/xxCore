//
// Created by xiong on 2021/7/19.
//

#ifndef EMX_EmxLog_HPP
#define EMX_EmxLog_HPP

#include <cstdio>
#include <netinet/in.h>
#include <sys/un.h>
#include "core/EmxTypeDef.hpp"

//单条日志最大size
#define EMX_LOG_MAX_SIZE (1024*4)

namespace Emx {

    //!@brief日志系统客户端
    class Log {
    public:

        //!@brief 日志等级
        enum class LevelE : uint8_t {
            Crt = 0, //!< 紧急
            Err, //!< 错误
            Warn, //!< 告警
            Info, //!< 通知
            Debug, //!< 调试
            Trace, //!< 跟踪
        };

        //!@brief 配置Log系统
        struct Config {

            //!@brief 各个进程包括LogServer共享的内存的数据
            struct Shared {
                LevelE level; //!< 日志等级
                bool async; //!< 是否是异步打印，异步打印由LogServer统一收集并打印，同步打印在各自进程的调用处打印
            } shared;

            //!@brief LogServer存储通道的配置信息
            struct Channel {
                int32_t maxSizeKB;//-1:means infinity
                char path[EMX_MAX_PATH_SIZE];
            } normal, flash;
        };

        Log();

        ~Log();

        /*!
         * @brief 打印普通日志
         * @param level [in] 日志等级
         * @param file [in] 所属文件
         * @param func [in] 所属函数
         * @param line [in] 所在行数
         * @param fmt [in] 格式化参数
         * @param ... [in] 可变参数
         */
        void Print(LevelE level, const char *file, const char *func,
                   int32_t line, const char *fmt, ...);

        /*!
         * @brief 打印不带额外格式化信息的普通日志
         * @param fmt [in] 格式化参数
         * @param ... [in] 可变参数
         */
        void Print(const char *fmt, ...);

        /*!
         * @brief 打印普通日志,同步本地阻塞打印,无打印等级控制
         * @param file [in] 所属文件
         * @param func [in] 所属函数
         * @param line [in] 所在行数
         * @param fmt [in] 格式化参数
         * @param ... [in] 可变参数
         */
        void Print(const char *file, const char *func,
                   int32_t line, const char *fmt, ...);

        /*!
         * @brief 保存日志到flash
         * @param fmt [in] 格式化参数
         * @param ... [in] 可变参数
         */
        void Save(const char *fmt, ...);

        /*!
         * @brief 配置全局日志等级
         * @param level [in]日志等级
         */
        void SetLevel(LevelE level);

        /*!
         * @brief 获取全局日志等级
         * @return LevelE
         */
        LevelE GetLevel();

        /*!
         * @brief 设置日志打印异步模式
         * @param ena [in] true:使能异步打印；false:同步打印
         */
        void SetAsync(bool ena);

        /*!
         * @brief 获取日志打印异步模式
         * @return bool true:异步打印；false:同步打印
         */
        bool GetAsync();

        /*!
         * @brief 配置日志系统
         * @param cfg [in] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE SetConfig(Config &cfg);

        /*!
         * @brief 获取日志系统配置
         * @param cfg [out] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE GetConfig(Config &cfg);

        /*!
         * @brief 暂停日志记录
         * @return ErrCodeE
         */
        ErrCodeE Pause();

        /*!
         * @brief 恢复日志记录
         * @return ErrCodeE
         */
        ErrCodeE Resume();

    private:
        void CreateChannel();

        void DestroyChannel();

        struct Channel {
            int32_t sock;
            sockaddr_un address;
        };
        Channel m_ctrl;
        Channel m_normal;
        Channel m_flash;
        Config::Shared *m_shared;
        int m_shmId;
        void *m_shm;
    };
}  // namespace Emx
//!每个进程的全局日志系统客户端对象
extern Emx::Log emxLog;

#define xlogpt(level, fmt, ...) \
  emxLog.Print(level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

//!打印紧急日志
#define emxlogc(fmt, ...) xlogpt(Emx::Log::LevelE::Crt, fmt, ##__VA_ARGS__)
//!打印错误日志
#define emxloge(fmt, ...) xlogpt(Emx::Log::LevelE::Err, fmt, ##__VA_ARGS__)
//!打印警告日志
#define emxlogw(fmt, ...) xlogpt(Emx::Log::LevelE::Warn, fmt, ##__VA_ARGS__)
//!打印通知日志
#define emxlogi(fmt, ...) xlogpt(Emx::Log::LevelE::Info, fmt, ##__VA_ARGS__)
//!打印调试日志
#define emxlogd(fmt, ...) xlogpt(Emx::Log::LevelE::Debug, fmt, ##__VA_ARGS__)
//!打印跟踪日志
#define emxlogt(fmt, ...) xlogpt(Emx::Log::LevelE::Trace, fmt, ##__VA_ARGS__)
//!打印无额外格式化信息日志(仅包含时间)
#define emxlogn(fmt, ...) emxLog.Print(fmt, ##__VA_ARGS__)
//!打印普通日志,同步本地阻塞打印,无打印等级控制
#define emxlogx(fmt, ...) emxLog.Print(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif //EMX_EmxLog_HPP

