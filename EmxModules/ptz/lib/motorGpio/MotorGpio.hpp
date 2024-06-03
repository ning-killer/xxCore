/*
* @Author: xiong
* @Date: 2023/4/17
*/

#ifndef EMX_MotorGpio_HPP
#define EMX_MotorGpio_HPP

#include "MotorBase.hpp"
#include "MotorGpio.h"

namespace Emx {

    class MotorGpio : public MotorBase {
    public:
        MotorGpio() : MotorBase() { m_fd = -1; }

        virtual ~MotorGpio() {}

        ErrCodeE Init() override;

        void DeInit() override;

        ErrCodeE MoveStep(Ptz::TypeE type, bool block, int step, int speed) override;

        ErrCodeE Reset() override;

        bool IsBusy(Ptz::TypeE type) override;

        ErrCodeE GetStat(Ptz::TypeE type, bool block, Ptz::Status &status) override;

    private:
        struct Config {
            Ptz::TypeE type;
            int chn;
            struct Range {
                int min;
                int max;
            } position, speed;
        };
        std::map<Ptz::TypeE, Config> m_map;
        int m_fd;
        MotorArgInit m_arg;
        bool m_Vertical;
        bool m_Horizon;
    };

}

#endif //EMX_MotorGpio_HPP
