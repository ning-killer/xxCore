/*
* @Author: xiong
* @Date: 2023/4/7
*/

#ifndef EMX_Ptz_HPP
#define EMX_Ptz_HPP

#include "EmxCore.hpp"

namespace Emx {
    class MotorBase;

    class Ptz {
    public:
        Ptz();

        virtual ~Ptz();

        //! 操作类型
        enum class TypeE : uint8_t {
            Horizon = 0, //!< 水平
            Vertical, //!< 垂直
            Zoom, //!< 焦距
            Iris, //!< 光圈
            Focus, //!< 对焦
        };

        //! 状态值
        struct StatusValue {
            int min; //!< 最小值
            int max; //!< 最大值
            int current; //!< 当前值
        };

        //! 状态
        struct Status {
            bool busy; //!< 当前类型是否处于忙碌状态(移动中)
            StatusValue step; //!< 步数状态
            StatusValue speed; //!< 速度状态
        };

    public:

        /**
         * @brief 初始化驱动，此处会对所有的电机进行复位，是阻塞操作，会引起长时间的阻塞
         * @retval ErrCodeE
         */
        ErrCodeE Init();

        /**
         * @brief 反初始化
         */
        void DeInit();

        /**
         * @brief 根据指定的类型和方向，按照指定的速度移动指定步长
         * @param type [in] 操作类型
         * @param block [in] 是否以阻塞的方式步进，阻塞方式下配置的步数走完函数才会返回
         * @param step [in] 步长
         * @param speed [in] 速度 : 0 代表使用默认速度
         * @retval ErrCodeE
         */
        ErrCodeE MoveStep(TypeE type, bool block, int step, int speed = 0);

        /**
         * @brief 复位所有PTZ通道，所有电机恢复默认位置，此操作会引起长时间的阻塞
         * @retval ErrCodeE
         */
        ErrCodeE Reset();

        /**
         * @brief 获取指定类型的当前是否正在运行，当仅需要判断是否正在运行的时候
         * 推荐使用此接口，而非GetStat，效率较高，不会引起阻塞
         * @param type [in] 操作类型
         * @retval false:没有运行，true:正在运行
         */
        bool IsBusy(Ptz::TypeE type);

        /**
         * @brief 获取指定类型的当前状态
         * @param type [in] 操作类型
         * @param block [in] 是否以阻塞的方式获取状态
         * @param status [out] 状态信息
         * @retval ErrCodeE
         */
        ErrCodeE GetStat(TypeE type, bool block, Status &status);

    private:
        MotorBase *m_motor;
    };
}
#endif //EMX_Ptz_HPP
