/*
 * @Author: xiong
 * @Date: 2022/9/5
*/

#include "FlashFactory.hpp"
#include "SpiNorFlash.hpp"
#include "SpiNandFlash.hpp"
#include "Emmc.hpp"

using namespace Emx;

FlashInf *FlashFactory::Make(const char *type) {
    if (strcmp(type, "spiNor") == 0) {
        return new SpiNorFlash;
    } else if (strcmp(type, "spiNand") == 0) {
        return new SpiNandFlash;
    } else if (strcmp(type, "emmc") == 0) {
        return new Emmc;
    } else {
        emxloge("not supported flash type %s\n", type);
        return nullptr;
    }
}
