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
    keyReset = std::make_shared<BspKeyReset>();
    spk = std::make_shared<BspSpk>();
    led = std::make_shared<BspLed>();
    emxlogi("BspBase Inited\n");
}

Bsp::~Bsp() {}
