/*
* @Author: xiong
* @Date: 2023/6/2
*/

#include "SDCardUpdateLib.hpp"
#include "SDCardUpdate.hpp"

using namespace Emx;

SDCardUpdateLib::SDCardUpdateLib() {
    m_update = new SDCardUpdate;
}

SDCardUpdateLib::~SDCardUpdateLib() {
    delete m_update;
}

void SDCardUpdateLib::Create(bool newThread) {
    m_update->Create(newThread);
}

void SDCardUpdateLib::Destroy() {
    m_update->Destroy();
}

