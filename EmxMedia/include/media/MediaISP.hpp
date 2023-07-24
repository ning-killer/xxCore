//
// Created by xiong on 2021/4/23.
//

#ifndef EMX_MediaISP_HPP
#define EMX_MediaISP_HPP

#include <cstring>
#include "core/EmxTypeDef.hpp"
#include "json/json.h"

namespace Emx {
    class MediaISP {
    public:

        //!@brief ISP运行模式
        enum class RunModeE : uint8_t {
            Day, //!<白天模式
            Night, //!<夜晚模式
            NightWithLight, //!<白光夜视模式
            UnDef //!<未定义
        };

        //!@brief 操作模式
        enum class OptModeE : uint8_t {
            Auto, //!<自动模式
            Manual //!<手动模式
        };

        //!@brief 曝光增益
        enum class ExpGainE : uint8_t {
            X1 = 1,
            X2 = 2,
            X4 = 4,
            X8 = 8,
            X16 = 16,
            X32 = 32,
            X64 = 64,
        };

        //!@brief ISP持久化参数
        struct Param {
            OptModeE expMode; //!<曝光模式
            ExpGainE autoMaxExpGain; //!<最大自动曝光增益
            uint8_t compensation; //!<曝光补偿
            ExpGainE manualExpGain; //!<手动曝光增益
            int32_t expTime; //!<曝光时间
            uint8_t brightness; //!<亮度
            uint8_t sharpness; //!<锐度
            uint8_t hue; //!<色度
            uint8_t contrast; //!<对比度
            uint8_t saturation; //!<饱和度
            uint8_t deNoise; //!<降噪程度
            OptModeE wbMode; //!<白平衡模式
            uint8_t redGain; //!<手动白平衡下的红色增益
            uint8_t blueGain; //!<手动白平衡下的蓝色增益
            uint8_t antiFlickerHz; //!<抗闪烁频率
            bool forceAntiFlickerEna; //!<强制抗闪烁使能
        };

        //!@brief ISP信息
        struct Info {
            int chn; //!<曝光模式
            int iso; //!<感光度
            int redGain; //!<红色增益
            int blueGain; //!<蓝色增益
            int luma; //!<亮度
        };

        explicit MediaISP(int32_t chn) : m_chn(chn) {};

        virtual ~MediaISP() {};

        /**
         * @brief 设置参数
         * @param param [in] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE SetParam(const Param &param) = 0;

        /**
         * @brief 获取参数
         * @param param [out] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(Param &param) = 0;

        /**
         * @brief 设置运行模式
         * @param mode [in] 运行模式
         * @return ErrCodeE
         */
        virtual ErrCodeE SetRunMode(RunModeE mode) = 0;

        /**
         * @brief 获取运行模式
         * @param mode [out] 运行模式
         * @return ErrCodeE
         */
        virtual ErrCodeE GetRunMode(RunModeE &mode) = 0;

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParamRange(Json::Value &range) = 0;

        /**
         * @brief 判断参数是否改变
         * @param paramA [in] 参数A
         * @param paramB [in] 参数B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool ParamChanged(const Param &paramA, const Param &paramB) {
            return memcmp(&paramA, &paramB, sizeof(Param)) != 0;
        }

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };
}
#endif //EMX_MediaISP_HPP

