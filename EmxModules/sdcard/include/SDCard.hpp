//
// Created by xiong on 2021/6/1.
//

#ifndef EMX_SDCard_HPP
#define EMX_SDCard_HPP

#include "EmxCore.hpp"

namespace Emx {

    //!@brief 提供操作设备SD卡相关接口
    class SDCard {
    public:
        //!@brief SD卡状态
        enum class StatE : uint8_t {
            Rejected,//!<未插入
            Inserted,//!<已插入，未挂载
            Normal,//!<正常挂载运行
            Formatting,//!<格式化中
        };
        //!@brief 文件系统类型
        enum class FileSystemE : uint8_t {
            None,//!<未知
            Fat32,//!<FAT32
            Fat64,//!<FAT64/exfat
            Ntfs,//!<NTFS
        };
        //!@brief SD卡信息
        struct Info {
            static const int MaxDevSize = 32;//!<设备名称最大长度
            StatE stat;//!<SD卡状态
            FileSystemE fileSystem;//!<文件系统类型
            uint32_t total;//!<总大小MB
            uint32_t free;//!<空闲大小MB
            uint32_t used;//!<已使用大小MB
            char dev[MaxDevSize];//!<设备名称
            char mountPoint[EMX_MAX_PATH_SIZE];//!<设备挂载路径
        };

        SDCard(int chn) : m_chn(chn) {}

        /**
         * @brief 挂载SD卡
         * @return ErrCodeE
         */
        virtual ErrCodeE Mount() = 0;

        /**
         * @brief 卸载SD卡
         * @return ErrCodeE
         */
        virtual ErrCodeE UnMount() = 0;

        /**
         * @brief 格式化SD卡
         * @return ErrCodeE
         */
        virtual ErrCodeE Format() = 0;

        /**
         * @brief 获取SD卡信息
         * @param info [out] SD卡信息
         * @return ErrCodeE
         */
        virtual ErrCodeE GetInfo(Info &info) = 0;

        /**
         * @brief 检查SD卡是否读写正常
         * @return ErrCodeE
         */
        virtual ErrCodeE Check() = 0;

        /**
         * @brief 获取当前SD卡所属通道
         * @return 通道号
         */
        int GetChn() { return m_chn; }

    protected:
        int m_chn;
    };

}

#endif //EMX_SDCard_HPP
