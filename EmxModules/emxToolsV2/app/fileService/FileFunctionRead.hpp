/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileFunctionRead_HPP
#define EMX_FileFunctionRead_HPP

#include "FileFunction.hpp"

namespace Emx {

    class FileFunctionRead : public FileFunction {
    public:
        FileFunctionRead(FileClient *c);

        virtual ~FileFunctionRead();

        void OnGetData(char *data, int size) override;

        void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) override;

    private:
        enum class CodeE : uint32_t {
            Success = 0,
            OpenFailed,
            ReadFailed,
        };

        struct InHeader {
            static const int Size = EMX_MAX_PATH_SIZE;
            char path[EMX_MAX_PATH_SIZE];
        };

        struct OutHeader {
            static const int Size = 4 + 4 + 32;
            uint32_t fileSize;
            char md5[32];
            uint32_t code;
        };
        InHeader m_inHeader;
        bool m_waitHeader;
        std::string m_data;
        FILE *m_fp;
        MD5 m_md5;
    };
}

#endif //EMX_FileFunctionRead_HPP
