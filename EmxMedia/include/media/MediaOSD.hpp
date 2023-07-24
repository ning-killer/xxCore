//
// Created by xiong on 2021/8/9.
//

#ifndef EMX_MediaOSD_HPP
#define EMX_MediaOSD_HPP

#include <cstring>
#include "core/EmxTypeDef.hpp"
#include "json/json.h"
#include <list>

namespace Emx {
    //!@brief OSD
    class MediaOSD {
    public:
        static const int MaxTextSize = 256; //!<字符类型最大长度
        static const int RateBase = 1000; //!<偏移量比率基准
        static const int MaxOSDNum = 8; //!<最大通道数

        //!@brief OSD类型
        enum class TypeE : uint8_t {
            Time = 0, //!<时间戳显示
            Text, //!<文字显示
            Image, //!<图片显示
        };

        //!@brief 水平对齐方式
        enum class HAlignE : uint8_t {
            Left, //!<左对齐
            Right, //!<右对齐
            Center, //!<居中
        };

        //!@brief 竖直对齐方式
        enum class VAlignE : uint8_t {
            Top, //!<顶部对齐
            Bottom, //!<底部对齐
            Center, //!<居中
        };

        //!@brief 描述位置偏移以及对齐方式
        struct Margin {
            HAlignE horizon; //!<水平对齐方式
            int rateX; //!<水平偏移值 1/RateBase
            VAlignE vertical; //!<竖直对齐方式
            int rateY; //!<竖直偏移值 1/RateBase
        };

        //!@brief 时间显示格式
        enum class TimeFormatE : uint8_t {
            Normal, //!<一般格式 eg.[2022-05-20 12:24:48]
            NormalWithZhWeek, //!<带有中文的星期 eg.[2022-05-20 星期五 12:24:48]
        };

        //!@brief 时间戳类型OSD持久化参数
        struct ParamTime {
            bool ena; //!<是否使能
            Margin margin; //!<对齐方式
            int size; //!<字体大小
            int thickness; //!<边框大小
            int layer; //!<显示层级
            char font[EMX_MAX_PATH_SIZE]; //!<字体文件名(字体文件均放置在configs/media/font目录下)
            TimeFormatE format; //!<时间显示格式
        };

        //!@brief 文字类型OSD持久化参数
        struct ParamText {
            bool ena; //!<是否使能
            Margin margin; //!<对齐方式
            int size; //!<字体大小
            int thickness; //!<边框大小
            int layer; //!<显示层级
            char font[EMX_MAX_PATH_SIZE]; //!<字体文件名(字体文件均放置在configs/media/font目录下
            char content[MaxTextSize]; //!<显示内容
        };

        //!@brief 图片类型OSD持久化参数
        struct ParamImage {
            bool ena; //!<是否使能
            Margin margin; //!<对齐方式
            int layer; //!<显示层级
            uint32_t alpha; //!<透明度
            int width; //!<图片宽
            int height; //!<图片高
            char path[EMX_MAX_PATH_SIZE]; //!<图片路径
        };

        //!@brief OSD持久化参数
        struct Param {
            TypeE type; //!<OSD类型
            union {
                ParamTime time; //!<时间戳类型OSD持久化参数
                ParamText text; //!<文字类型OSD持久化参数
                ParamImage image; //!<图片类型OSD持久化参数
            };
        };


        explicit MediaOSD(int chn) : m_chn(chn) {};

        virtual ~MediaOSD() {};

        /**
         * @brief 设置参数
         * @param param [in] 参数数据数组
         * @return ErrCodeE
         */
        virtual ErrCodeE SetParam(const std::vector<std::shared_ptr<Param>> &param) = 0;

        /**
         * @brief 获取参数
         * @param param [out] 参数数据数组
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(std::vector<std::shared_ptr<Param>> &param) = 0;

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParamRange(Json::Value &range) = 0;

        /**
         * @brief 判断参数是否改变
         * @param paramA [in] 参数数组A
         * @param paramB [in] 参数数组B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool ParamChanged(const std::vector<std::shared_ptr<Param>> &paramA,
                                 const std::vector<std::shared_ptr<Param>> &paramB) {
            if (paramA.size() != paramB.size())
                return true;
            for (int i = 0; i < (int) paramA.size(); i++) {
                if (memcmp(paramA[i].get(), paramB[i].get(), sizeof(Param)) != 0) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int GetChn() { return m_chn; }

    protected:
        int m_chn;
    };
}
#endif //EMX_MediaOSD_HPP

