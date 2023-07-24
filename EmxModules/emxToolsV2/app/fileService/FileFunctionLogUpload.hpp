/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileFunctionLogUpload_HPP
#define EMX_FileFunctionLogUpload_HPP

#include "FileFunction.hpp"

namespace Emx {

    class FileFunctionLogUpload : public FileFunction {
    public:
        FileFunctionLogUpload(FileClient *c);

        virtual ~FileFunctionLogUpload();

        void OnGetData(char *data, int size) override;

        void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override;

    private:
        enum class CodeE : uint32_t {
            Success = 0,
            OpenFailed,
            ReadFailed,
        };


        struct OutHeader {
            static const int Size = 4 + 4;
            uint32_t fileSize;
            uint32_t code;
        };
        FILE *m_fp;
    };

}

#endif //EMX_FileFunctionLogUpload_HPP
