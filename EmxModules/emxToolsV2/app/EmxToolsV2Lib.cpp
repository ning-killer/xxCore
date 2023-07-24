/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include <utility>
#include "EmxToolsV2.hpp"
#include "EmxToolsV2Lib.hpp"

using namespace Emx;

EmxToolsV2Lib::EmxToolsV2Lib() {
    m_tools = new EmxToolsV2;
}

EmxToolsV2Lib::~EmxToolsV2Lib() {
    delete m_tools;
}

void EmxToolsV2Lib::Create(bool newThread) {
    m_tools->Create(newThread);
}

void EmxToolsV2Lib::Destroy() {
    m_tools->Destroy();
}

