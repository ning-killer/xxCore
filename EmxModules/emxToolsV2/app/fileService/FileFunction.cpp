/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileFunction.hpp"
#include "FileFunctionWrite.hpp"
#include "FileFunctionRead.hpp"
#include "FileFunctionLogUpload.hpp"
#include "FileFunctionUpdate.hpp"

using namespace Emx;

FileFunction *FileFunctionFactory::Make(FileHeader::CmdE cmd, FileClient *c) {
    switch (cmd) {
        case FileHeader::CmdE::Update:
            return new FileFunctionUpdate(c);
        case FileHeader::CmdE::FileWrite:
            return new FileFunctionWrite(c);
        case FileHeader::CmdE::FileRead:
            return new FileFunctionRead(c);
        case FileHeader::CmdE::LogUpload:
            return new FileFunctionLogUpload(c);
        default:
            return nullptr;
    }
}
