/*
* @Author: xiong
* @Date: 2023/4/17
*/

#ifndef EMX_MotorBase_HPP
#define EMX_MotorBase_HPP

#include "Ptz.hpp"

namespace Emx {

    class MotorBase {
    public:
        MotorBase() {}

        virtual ~MotorBase() {}

        /**
         * @brief 初始化
         * @retval ErrCodeE
         */
        virtual ErrCodeE Init() = 0;

        /**
         * @brief 反初始化
         */
        virtual void DeInit() = 0;

        /**
         * @brief 根据指定的类型和方向，按照指定的速度移动指定步长
         * @param type [in] 操作类型
         * @param block [in] 是否以阻塞的方式步进，阻塞方式下配置的步数走完函数才会返回
         * @param step [in] 步长
         * @param speed [in] 速度 : 0 代表使用默认速度
         * @retval ErrCodeE
         */
        virtual ErrCodeE MoveStep(Ptz::TypeE type, bool block, int step, int speed) = 0;

        /**
         * @brief 复位所有PTZ通道
         * @retval ErrCodeE
         */
        virtual ErrCodeE Reset() = 0;

        /**
         * @brief 获取指定类型的当前是否正在运行，当仅需要判断是否正在运行的时候
         * 推荐使用此接口，而非GetStat，效率较高，不会引起阻塞
         * @param type [in] 操作类型
         * @retval false:没有运行，true:正在运行
         */
        virtual bool IsBusy(Ptz::TypeE type) = 0;

        /**
         * @brief 获取指定类型的当前状态
         * @param type [in] 操作类型
         * @param block [in] 是否以阻塞的方式获取状态
         * @param status [out] 状态信息
         * @retval ErrCodeE
         */
        virtual ErrCodeE GetStat(Ptz::TypeE type, bool block, Ptz::Status &status) = 0;
    };

}
#endif //EMX_MotorBase_HPP
