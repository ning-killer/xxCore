//
// Created by xiong on 2022/6/9.
//

#include "core/utils/File.hpp"
#include "core/EmxLog.hpp"
#include <sys/stat.h>

using namespace Emx;


ErrCodeE File::Copy(const char *src, const char *dst) {
    emxlogt("copy %s to %s\n", src, dst);
    FILE *fps = fopen(src, "rb");
    if (!fps) {
        emxloge("cannot open %s\n", src);
        return ErrCodeE::ResNotExist;
    }

    FILE *fpd = fopen(dst, "rb+");
    if (!fpd) {
        fpd = fopen(dst, "wb+");
        if (!fpd) {
            fclose(fps);
            emxloge("cannot open %s\n", dst);
            return ErrCodeE::ResNotExist;
        }
    }
//    lockf(fileno(fps), F_LOCK, 0);
//    if (lockf(fileno(fpd), F_TLOCK, 0)) {
//        usleep(500 * 1000);
//        if (lockf(fileno(fpd), F_TLOCK, 0)) {
//            lockf(fileno(fps), F_ULOCK, 0);
//            fclose(fps);
//            emxloge("%s in using\n", src);
//            return ErrCodeE::RES_IN_USING;
//        }
//    }
    int n;
    char buffer[1024];
    while ((n = (int) fread(buffer, 1, 1024, fps)) > 0) {
        fwrite(buffer, 1, n, fpd);
        fflush(fpd);
        fsync(fileno(fpd));
    }
//    lockf(fileno(fpd), F_ULOCK, 0);
//    lockf(fileno(fps), F_ULOCK, 0);
    fclose(fps);
    fclose(fpd);
    return ErrCodeE::Success;
}

bool File::Exist(const char *path) {
    return access(path, 0) == 0;
}

void File::Create(const char *path) {
    fclose(fopen(path, "wb+"));
}

bool File::IsDir(const char *path) {
    struct stat st = {};
    stat(path, &st);
    return (bool) S_ISDIR(st.st_mode);
}

bool File::IsFile(const char *path) {
    struct stat st = {};
    stat(path, &st);
    return (bool) S_ISREG(st.st_mode);
}

int32_t File::Size(const char *path) {
    struct stat st = {};
    stat(path, &st);
    return st.st_size;
}

void File::Remove(const char *path) {
    if (Exist(path)) {
        remove(path);
    }
}

ErrCodeE File::Read(const char *path, std::string &buffer) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return ErrCodeE::OpenFailed;
    }
    char tmp[1024];
    while (true) {
        int n = (int) fread(tmp, 1, 1024, fp);
        if (n > 0)
            buffer.append(tmp, n);
        else
            break;
    }
    fclose(fp);
    return ErrCodeE::Success;
}

ErrCodeE File::Write(const char *path, const char *buffer, int size) {
    if (!buffer || !size)
        return ErrCodeE::Success;
    FILE *fp = fopen(path, "wb+");
    if (!fp) {
        emxloge("cannot open file %s\n", path);
        return ErrCodeE::OpenFailed;
    }
    int totalSize = size;
    int off = 0;
    while (totalSize) {
        int n = (int) fwrite(buffer + off, 1, totalSize, fp);
        if (n <= 0) {
            break;
        }
        totalSize -= n;
        off += n;
    }
    fclose(fp);
    return totalSize ? ErrCodeE::Failure : ErrCodeE::Success;
}
