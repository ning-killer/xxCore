/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileFunctionWrite_HPP
#define EMX_FileFunctionWrite_HPP

#include "FileFunction.hpp"

namespace Emx {

    class FileFunctionWrite : public FileFunction {
    public:
        FileFunctionWrite(FileClient *c);

        virtual ~FileFunctionWrite();

        void OnGetData(char *data, int size) override;

    private:
        enum class CodeE : uint32_t {
            Success = 0,
            Writing,
            Md5MissMatch,
            OpenPathFailed,
            WriteFailed,
        };

        struct InHeader {
            static const int Size = 32 + EMX_MAX_PATH_SIZE;
            char md5[32];
            char path[EMX_MAX_PATH_SIZE];
        };

        struct OutHeader {
            static const int Size = 4 + 4 + 4;
            uint32_t totalSize;
            uint32_t receivedSize;
            uint32_t code;
        };
        InHeader m_inHeader;
        uint32_t m_currentSize;
        bool m_waitHeader;
        std::string m_data;
        FILE *m_fp;
        MD5 m_md5;
    };

}

#endif //EMX_FileFunctionWrite_HPP
