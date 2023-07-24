/*
 * @Author: xiong
 * @Date: 2022/9/18
*/

#include "Mp4Buffer.h"
#include "stdio.h"

int mov_file_read(void *fp, void *data, uint64_t bytes) {
    if (bytes == fread(data, 1, bytes, (FILE *) fp))
        return 0;
    return 0 != ferror((FILE *) fp) ? ferror((FILE *) fp) : -1 /*EOF*/;
}

int mov_file_write(void *fp, const void *data, uint64_t bytes) {
    return bytes == fwrite(data, 1, bytes, (FILE *) fp) ? 0 : ferror((FILE *) fp);
}

int mov_file_seek(void *fp, int64_t offset) {
    return fseek((FILE *) fp, (long int) offset, offset >= 0 ? SEEK_SET : SEEK_END);
}

int64_t mov_file_tell(void *fp) {
    return ftell((FILE *) fp);
}

static struct mov_buffer_t s_io = {
        mov_file_read,
        mov_file_write,
        mov_file_seek,
        mov_file_tell,
};

struct mov_buffer_t *GetBuffer() { return &s_io; }