//
// Created by xiong on 2021/7/19.
//

#ifndef EMX_EmxTypeDef_HPP
#define EMX_EmxTypeDef_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unistd.h>

#define EMX_MAX_PATH_SIZE 256

#define ph_1 std::placeholders::_1
#define ph_2 std::placeholders::_2
#define ph_3 std::placeholders::_3
#define ph_4 std::placeholders::_4
#define ph_5 std::placeholders::_5
#define ph_6 std::placeholders::_6
#define ph_7 std::placeholders::_7
#define ph_8 std::placeholders::_8

namespace Emx {
    //!@brief 错误码定义
    enum class ErrCodeE : int32_t {
        Success = 0, //!< 成功
        Failure = -1, //!< 失败
        IllegalParam = -2, //!< 非法参数
        MallocFailed = -3, //!< 内存分配失败
        NetworkError = -4, //!< 网络相关错误
        BufferOverflow = -5, //!< 缓存益处
        BufferOverride = -6, //!< 缓存数据被覆盖
        Timeout = -7, //!< 超时
        FrameError = -8, //!< 帧错误
        ResNotAvailable = -9, //!< 资源当前不可用
        ResInUsing = -10, //!< 资源被占用
        ResNotExist = -11, //!< 资源不存在
        OpenFailed = -12, //!< 打开失败
        ParseFailed = -13, //!< 解析失败
        OperationNotSupport = -14, //!< 操作不支持
        PskShorterThan8 = -15, //!< WiFi密码长度过短
        ReachTheEof = -16, //!< 达到文件尾
        Canceled = -17, //!< 被取消
        Destroyed = -18, //!< 被销毁
        VerifyFailed = -19, //!< 校验失败
    };
}
#endif //EMX_EmxTypeDef_HPP

