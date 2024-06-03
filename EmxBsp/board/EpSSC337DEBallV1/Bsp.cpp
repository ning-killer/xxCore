//
// Created by xiong on 2022/6/27.
//
#include "BspSoundCheck.hpp"
#include "BspAdcCvitek.hpp"
#include "BspIrCut2Wire.hpp"
#include "BspIrLight.hpp"
#include "BspKeyReset.hpp"
#include "BspSpk.hpp"
#include "BspWhiteLightCvitek.hpp"
#include "BspUpdateIndicator.hpp"
#include "BspLed.hpp"

using namespace Emx;

Bsp::Bsp() : BspBase() {
    adc = std::make_shared<BspAdcCvitek>();
    irCut = std::make_shared<BspIrCut2Wire>();
    irLight = std::make_shared<BspIrLight>();
    keyReset = std::make_shared<BspKeyReset>();
    SoundCheck = std::make_shared<BspSoundCheck>();
    spk = std::make_shared<BspSpk>();
    whiteLight = std::make_shared<BspWhiteLightCvitek>();
    updateIndicator = std::make_shared<BspUpdateIndicator>();
    led = std::make_shared<BspLed>();
    emxlogi("Inited\n");
}

Bsp::~Bsp() {}
