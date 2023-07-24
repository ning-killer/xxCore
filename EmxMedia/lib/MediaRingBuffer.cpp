//
// Created by xiong on 2021/7/30.
//

#include "media/MediaRingBuffer.hpp"
#include "core/EmxLog.hpp"

using namespace Emx;

#define RINGBUFFER_MAGIC_LEN 4

static inline void FillMagic(char *buf, uint32_t magicId) {
    buf[0] = (magicId >> 24) & 0xff;
    buf[1] = (magicId >> 16) & 0xff;
    buf[2] = (magicId >> 8) & 0xff;
    buf[3] = (magicId) & 0xff;
}

static inline uint32_t GetMagic(char *buf) {
    return ((uint32_t) buf[0] & 0xff) << 24 |
           ((uint32_t) buf[1] & 0xff) << 16 |
           ((uint32_t) buf[2] & 0xff) << 8 |
           ((uint32_t) buf[3] & 0xff);
}

ErrCodeE MediaRingBufferWriter::Create(int32_t key, int32_t size) {
    m_buffer = (char *) m_shm.Create(key, size);
    if (!m_buffer) {
        emxloge("failed\n");
        return ErrCodeE::MallocFailed;
    }
    m_bufferSize = size;
    m_off = 0;
    return ErrCodeE::Success;
}

int32_t MediaRingBufferWriter::Push(char *data, int32_t size, uint32_t magicId) {
    int32_t totalWriteSize = size + (RINGBUFFER_MAGIC_LEN * 2);
    if (totalWriteSize > m_bufferSize) {
        emxloge("data size %d large than buffer %d\n", size, m_bufferSize);
        return -1;
    }
    if (totalWriteSize > m_bufferSize - m_off) {
        //clear the remain buffer, in case someone read valid data
        memset(m_buffer + m_off, 0, m_bufferSize - m_off);
        m_off = 0;
    }
    int32_t off = m_off;
    FillMagic(m_buffer + m_off, magicId);
    m_off += RINGBUFFER_MAGIC_LEN;
    memcpy(m_buffer + m_off, data, size);
    m_off += size;
    FillMagic(m_buffer + m_off, magicId);
    m_off += RINGBUFFER_MAGIC_LEN;
    return off;
}

int32_t MediaRingBufferWriter::GetFreeBuffer(int32_t size, uint32_t magicId, char **data) {
    int32_t totalWriteSize = size + (RINGBUFFER_MAGIC_LEN * 2);
    if (totalWriteSize > m_bufferSize) {
        emxloge("data size %d large than buffer %d\n", size, m_bufferSize);
        return -1;
    }
    if (totalWriteSize > m_bufferSize - m_off) {
        //clear the remain buffer, in case someone read valid data
        memset(m_buffer + m_off, 0, m_bufferSize - m_off);
        m_off = 0;
    }
    int32_t off = m_off;
    FillMagic(m_buffer + m_off, magicId);
    m_off += RINGBUFFER_MAGIC_LEN;
    *data = m_buffer + m_off;
    m_off += size;
    FillMagic(m_buffer + m_off, magicId);
    m_off += RINGBUFFER_MAGIC_LEN;
    return off;
}

void MediaRingBufferReader::Create(int32_t key) {
    m_key = key;
    m_buffer = (char *) m_shm.Create(m_key);
    if (m_buffer) {
        m_bufferSize = m_shm.GetSize();
    }
}

ErrCodeE MediaRingBufferReader::Pop(int32_t off, int32_t size, uint32_t magicId, std::string &data) {
    if (!m_buffer) {
        m_buffer = (char *) m_shm.Create(m_key);
        if (m_buffer) {
            m_bufferSize = m_shm.GetSize();
        } else {
            return ErrCodeE::ResNotAvailable;
        }
    }
    if (off + size + (RINGBUFFER_MAGIC_LEN * 2) > m_bufferSize) {
        emxlogw("in off + size = %d, but buffer size = %d\n", off, size, m_bufferSize);
        return ErrCodeE::BufferOverflow;
    }

    uint32_t magicIdStart = GetMagic(m_buffer + off);
    uint32_t magicIdEnd = GetMagic(m_buffer + off + RINGBUFFER_MAGIC_LEN + size);
    if (magicId != magicIdStart || magicId != magicIdEnd) {
        emxlogt("buffer override 0\n");
        return ErrCodeE::BufferOverride;
    }
    try {
        data.assign(m_buffer + off + RINGBUFFER_MAGIC_LEN, size);
        magicIdStart = GetMagic(m_buffer + off);
        magicIdEnd = GetMagic(m_buffer + off + RINGBUFFER_MAGIC_LEN + size);
        if (magicId != magicIdStart || magicId != magicIdEnd) {
            emxlogt("buffer override 1\n");
            return ErrCodeE::BufferOverride;
        }
    } catch (std::exception &e) {
        emxloge("malloc failed\n");
        return ErrCodeE::MallocFailed;
    }
    return ErrCodeE::Success;
}
