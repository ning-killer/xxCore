/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include <utility>
#include "EmxToolsV1.hpp"
#include "EmxToolsV1Lib.hpp"

using namespace Emx;

EmxToolsV1Lib::EmxToolsV1Lib() {
    m_tools = new EmxToolsV1;
}

EmxToolsV1Lib::~EmxToolsV1Lib() {
    delete m_tools;
}

void EmxToolsV1Lib::Create(bool newThread) {
    m_tools->Create(newThread);
}

void EmxToolsV1Lib::Destroy() {
    m_tools->Destroy();
}

