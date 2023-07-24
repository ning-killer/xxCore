//
// Created by xiong on 23-4-11.
//
// 支持最多MOTOR_GPIO_MAX_CHN_NUM路步进电机控制
// 每路电机独立调速
// 支持最多MOTOR_GPIO_ORDER_MAX_NUM个步进序列配置
// 支持最多MOTOR_GPIO_MAX_LOCATION_NUM个定位GPIO，可配置每个定位信号GPIO对应的位置
// 支持MOTOR_GPIO_PIN_NUM个电机相位控制GPIO
// 支持所有电机分时复用模式
// 支持无限位360度转动方式
// 支持每路电机配置一个使能信号引脚
// 支持阻塞/非阻塞方式配置步进量与步进速度


#ifndef MOTOR_MOTOR_H
#define MOTOR_MOTOR_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <linux/ioctl.h>

#define MOTOR_GPIO_PIN_NUM 4 //!< 控制电机的GPIO数量
#define MOTOR_GPIO_ORDER_MAX_NUM 8 //!< 电机的步进序列数量
#define MOTOR_GPIO_MAX_CHN_NUM 8 //!< 最大支持的电机通道数量
#define MOTOR_GPIO_MAX_LOCATION_NUM 3 //!< 最大支持的定位信号数量

//!定位信号参数
typedef struct {
    int pinNum; //!< 管脚号
    int step; //!< 信号触发对应的位置
    unsigned char active; //!< 信号的有效电平
} LocationSignal;

//!电机参数
typedef struct {
    int stepMax; //!< 最大步进位置
    int stepDef; //!< 默认步进位置
    int speedDef; //!< 默认速度（定时器频率，单位Hz）
    int pinNum[MOTOR_GPIO_PIN_NUM]; //!< 电机管脚号,-1代表忽略此管脚
    int pinEnaNum; //!< 电机使能管脚号，-1代表无需使能
    unsigned char pinEnaActive; //!< 电机使能有效电平
    unsigned char unlimited; //!< 是否旋转无限位
    unsigned char stepOrderNum; //!< 序列实际数量
    unsigned char stepOrder[MOTOR_GPIO_ORDER_MAX_NUM][MOTOR_GPIO_PIN_NUM]; //!< 步进序列
    LocationSignal location[MOTOR_GPIO_MAX_LOCATION_NUM]; //!< 定位信号参数
} MotorArg;

//!初始化参数
typedef struct MotorArgInit {
    int chnNum; //!<有效通道数量
    int timeDivision; //!<是否所有通道进行分时运行
    MotorArg arg[MOTOR_GPIO_MAX_CHN_NUM]; //!<通道参数
} MotorArgInit;

//!通道配置参数
typedef struct MotorArgSet {
    int chn; //!< 需要配置的通道
    int offset; //!< 需要步进的步数（+-）
    int speed; //!< 步进过程使用的速度
    int block; //!< 是否以阻塞的方式进行配置，阻塞方式下，配置的步数走完函数才会返回
} MotorArgSet;

//!通道获取参数
typedef struct MotorArgGet {
    int chn; //!< 需要获取的通道
    int block; //!< 是否以阻塞的方式进行配置获取
    int step; //!< 通道当前位置
    int speed; //!< 通道当前速度
    int busy; //!< 通道是否正在步进中
} MotorArgGet;

//!驱动控制指令
typedef enum {
    MOTOR_GPIO_IOCTL_NR_INIT = 0, //!< 初始化驱动，初始化完成会自动调用RESET，0:成功，-1:失败
    MOTOR_GPIO_IOCTL_NR_RESET, //!< 复位所有通道到默认位置，会长时间阻塞，0:成功，-1:失败
    MOTOR_GPIO_IOCTL_NR_CHN_SET, //!< 配置通道参数，可选阻塞，0:成功，-1:失败
    MOTOR_GPIO_IOCTL_NR_CHN_GET, //!< 获取通道参数，可选阻塞，0:成功，-1:失败
    MOTOR_GPIO_IOCTL_NR_CHN_IS_BUSY, //!< 获取通道是否正在运行，非阻塞，0:没有运行，1:正在运行
} MOTOR_GPIO_IOCTL_E;

#define MOTOR_GPIO_IOCTL_BASE 'M'
#define MOTOR_GPIO_IOCTL_INIT _IOW(MOTOR_GPIO_IOCTL_BASE ,MOTOR_GPIO_IOCTL_NR_INIT, struct MotorArgInit)
#define MOTOR_GPIO_IOCTL_RESET _IO(MOTOR_GPIO_IOCTL_BASE ,MOTOR_GPIO_IOCTL_NR_RESET)
#define MOTOR_GPIO_IOCTL_CHN_SET _IOW(MOTOR_GPIO_IOCTL_BASE ,MOTOR_GPIO_IOCTL_NR_CHN_SET, struct MotorArgSet)
#define MOTOR_GPIO_IOCTL_CHN_GET _IOR(MOTOR_GPIO_IOCTL_BASE ,MOTOR_GPIO_IOCTL_NR_CHN_GET, struct MotorArgGet)
#define MOTOR_GPIO_IOCTL_CHN_IS_BUSY _IOR(MOTOR_GPIO_IOCTL_BASE ,MOTOR_GPIO_IOCTL_NR_CHN_IS_BUSY, int)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif //MOTOR_MOTOR_H
