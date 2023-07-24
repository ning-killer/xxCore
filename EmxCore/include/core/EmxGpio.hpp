//
// Created by xiong on 2022/4/13.
//

#ifndef EMX_EmxGpio_HPP
#define EMX_EmxGpio_HPP

#include "core/EmxTypeDef.hpp"

namespace Emx {
    //!@brief 关于GPIO的通用操作，奇葩的控制方式可通过继承此类后重载实现
    class Gpio {
    public:
        //!@brief GPIO有效时的极性
        enum class ActiveE : uint8_t {
            Low = 0,//!<低有效
            High,//!<高有效
        };

        //!@brief GPIO状态
        enum class ValueE : uint8_t {
            Disable = 0,//!<失能无效
            Enable,//!<使能有效
            Err//!<错误
        };

        //!@brief GPIO输入输出
        enum class DirE : uint8_t {
            In = 0,//!<输入
            Out,//!<输出
            Err//!<错误
        };

        /*!
         * @brief 构造并初始化GPIO,并设定默认值(如果此GPIO未被初始化)，set/get/def的值都是基于active极性的
         * @param num 通过bank和pin转换后的gpio号
         * @param dir 输入输出方向
         * @param active gpio有效值的极性
         * @param def gpio作为输出时的默认值
         */
        Gpio(int num, DirE dir, ActiveE active, ValueE def);

        /*!
         * @brief 析构并去初始化GPIO
         */
        virtual ~Gpio();

        /*!
         * @brief 获取GPIO号
         * @return 通过bank和pin转换后的gpio号
         */
        int GetNum() { return m_num; }

        /*!
         * @brief 输出的状态下配置GPIO的使能状态
         * @param value 使能/失能
         * @return ErrCodeE
         */
        virtual ErrCodeE SetValue(ValueE value);

//        /*!
//         * @brief 强制配置GPIO的使能状态
//         * @param value 使能/失能
//         * @return ErrCodeE
//         */
//        virtual ErrCodeE SetValueForce(ValueE value);

        /*!
         * @brief 获取GPIO实际的使能状态
         * @return Value
         */
        virtual ValueE GetValue();

        /*!
         * @brief 配置GPIO的输入输出方向
         * @param dir 输入/输出
         * @return ErrCodeE
         */
        virtual ErrCodeE SetDir(DirE dir);

        /*!
         * @brief 获取GPIO当前输入输出状态
         * @return Dir
         */
        DirE GetDir() { return m_dir; }

        /*!
         * @brief 配置GPIO有效极性
         * @param active 极性
         */
        void SetActive(ActiveE active) { m_active = active; }

        /*!
         * @brief 获取GPIO极性
         * @return Active
         */
        ActiveE GetActive() { return m_active; }

        /*!
         * @brief 获取GPIO默认值
         * @return Value
         */
        ValueE GetDefault() { return m_default; }

    protected:
        int m_num;
        DirE m_dir;
        ActiveE m_active;
        ValueE m_default;
        ValueE m_value;
    };
}

#endif //EMX_EmxGpio_HPP

