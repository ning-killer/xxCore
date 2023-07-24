//
// Created by xiong on 2022/4/13.
//

#include "core/EmxGpio.hpp"
#include "core/EmxLog.hpp"
#include "core/EmxUtils.hpp"

using namespace Emx;

Gpio::Gpio(int num, Gpio::DirE dir, Gpio::ActiveE active, Gpio::ValueE def) :
        m_num(num), m_dir(dir), m_active(active), m_default(def), m_value(def) {
    char path[64];
    sprintf(path, "/sys/class/gpio/gpio%d", m_num);
    if (!File::Exist(path)) {
        FILE *fp = fopen("/sys/class/gpio/export", "w");
        if (fp) {
            fprintf(fp, "%d", m_num);
            fclose(fp);
            Gpio::SetDir(dir);
        } else {
            emxloge("open gpio num %d failed\n", m_num);
        }
    }
}

Gpio::~Gpio() {
//    FILE *fp = fopen("/sys/class/gpio/unexport", "w");
//    if (fp) {
//        fprintf(fp, "%d", m_num);
//        fclose(fp);
//    }
}

ErrCodeE Gpio::SetValue(Gpio::ValueE value) {
    FILE *fp;
    char path[64];
    unsigned char buf[8] = {};

    sprintf(path, "/sys/class/gpio/gpio%d/value", m_num);
    fp = fopen(path, "rb+");
    if (fp == nullptr) {
        emxloge("Cannot open %s.\n", path);
        return ErrCodeE::OpenFailed;
    }

    buf[0] = (int) ((int) value == (int) m_active) + 48;
    buf[1] = 0;
    fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);
    fclose(fp);
    m_value = value;
    return ErrCodeE::Success;
}

Gpio::ValueE Gpio::GetValue() {
    FILE *fp;
    char path[64];
    unsigned char buf[8];

    sprintf(path, "/sys/class/gpio/gpio%d/value", m_num);
    fp = fopen(path, "rb+");
    if (fp == nullptr) {
        emxloge("Cannot open %s.\n", path);
        return Gpio::ValueE::Err;
    }
    buf[0] = 0;
    buf[1] = 0;
    fread(buf, sizeof(char), sizeof(buf) - 1, fp);
    fclose(fp);
    return (ValueE)((buf[0] - 48) == (int) m_active);
}

ErrCodeE Gpio::SetDir(Gpio::DirE dir) {
    FILE *fp;
    char path[64];

    sprintf(path, "/sys/class/gpio/gpio%d/direction", m_num);
    fp = fopen(path, "rb+");
    if (fp == nullptr) {
        emxloge("Cannot open %s.\n", path);
        return ErrCodeE::OpenFailed;
    }
    fprintf(fp, "%s", m_dir == DirE::In ? "in" : "out");
    fclose(fp);
    m_dir = dir;
    if (m_dir == DirE::Out) {
        m_value = m_default;
        return SetValue(m_value);
    }
    return ErrCodeE::Success;
}
