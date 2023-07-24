/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileFunction_HPP
#define EMX_FileFunction_HPP

#include "FileHeader.hpp"

namespace Emx {
    class FileClient;

    class FileFunction {
    public:
        FileFunction(FileClient *c) : m_c(c) {}

        virtual ~FileFunction() {}

        virtual void OnGetData(char *data, int size) = 0;

        virtual void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {};

    protected:
        FileClient *m_c;
    };

    class FileFunctionFactory {
    public:
        static FileFunction *Make(FileHeader::CmdE cmd, FileClient *c);
    };

}

#endif //EMX_FileFunction_HPP
