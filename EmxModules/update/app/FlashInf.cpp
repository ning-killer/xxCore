/*
 * @Author: xiong
 * @Date: 2022/8/26
*/

#include "FlashInf.hpp"
#include <sys/ioctl.h>

using namespace Emx;

FlashInf::~FlashInf() {
    if (m_fd > 0) {
        close(m_fd);
        m_fd = -1;
    }
}

ErrCodeE FlashInf::Open(const char *dev) {
    m_fd = open(dev, O_SYNC | O_RDWR);
    if (m_fd < 0) {
        emxloge("open %s failed %d:%s\n", dev, errno, strerror(errno));
        return ErrCodeE::OpenFailed;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::Close() {
    if (m_fd > 0) {
        close(m_fd);
        m_fd = -1;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::Erase(int64_t start, int64_t size) {
    erase_info_t erase;
    erase.start = start;
    erase.length = size;
    if (ioctl(m_fd, MEMERASE, &erase) < 0) {
        emxloge("erase flash failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::Write(const uint8_t *data, int64_t size) {
    if (m_fd < 0) {
        emxlogc("fd not opened\n");
        return ErrCodeE::ResNotAvailable;
    }
    int off = 0;
    while (size > 0) {
        int n = write(m_fd, data + off, size);
        if (n < 0) {
            emxlogc("write flash failed %d:%s\n", errno, strerror(errno));
            return ErrCodeE::Failure;
        }
        off += n;
        size -= n;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::Read(uint8_t *data, int64_t size) {
    if (m_fd < 0) {
        emxlogc("fd not opened\n");
        return ErrCodeE::ResNotAvailable;
    }
    int off = 0;
    while (size > 0) {
        int n = read(m_fd, data + off, size);
        if (n < 0) {
            emxlogc("read flash failed %d:%s\n", errno, strerror(errno));
            return ErrCodeE::Failure;
        } else if (n == 0) {
            return ErrCodeE::Success;
        }
        off += n;
        size -= n;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::Seek(off_t offset, int whence) {
    if (m_fd < 0) {
        emxlogc("fd not opened\n");
        return ErrCodeE::ResNotAvailable;
    }
    if (lseek(m_fd, offset, whence) == -1) {
        emxloge("lseek %d:%s\n", errno, strerror(errno));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE FlashInf::GetInfo(mtd_info_t &mtd) {
    if (m_fd < 0) {
        emxlogc("fd not opened\n");
        return ErrCodeE::ResNotAvailable;
    }
    if (ioctl(m_fd, MEMGETINFO, &mtd) < 0) {
        emxloge("get flash info failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

