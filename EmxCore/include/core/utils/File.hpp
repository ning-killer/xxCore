//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_File_HPP
#define EMX_File_HPP

#include "core/EmxTypeDef.hpp"

namespace Emx {

    //!@brief 文件操作相关函数
    class File {
    public:
        //!路径是否为文件夹
        static bool IsDir(const char *path);

        //!路径是否为文件
        static bool IsFile(const char *path);

        //!文件拷贝
        static ErrCodeE Copy(const char *src, const char *dst);

        //!文件是否存在
        static bool Exist(const char *path);

        //!创建一个文件
        static void Create(const char *path);

        //!文件大小
        static int32_t Size(const char *path);

        //!删除一个文件
        static void Remove(const char *path);

        //!从文件中读取数据
        static ErrCodeE Read(const char *path, std::string &buffer);

        //!向文件中写入数据
        static ErrCodeE Write(const char *path, const char *buffer, int size);
    };

}

#endif //EMX_File_HPP
