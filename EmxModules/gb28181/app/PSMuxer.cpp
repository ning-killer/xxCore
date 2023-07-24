#include "PSMuxer.h"
#include "bits.h"

#define MAX_PES_SIZE 60000

PSMuxer::PSMuxer(uint8_t vType, uint8_t aType) :_videoStreamType(vType), _audioStreamType(aType)
{
    _cache.reset(new BufferRaw(60 * 1024));
}

PSMuxer::~PSMuxer() {}

int PSMuxer::PacketMediaData(char* frameData, uint32_t frameSize, uint32_t pts, bool iFrame, uint8_t t, char** out, uint32_t& outSize) {
    if (t == 0) {
        PacketVideo(frameData, frameSize, pts, iFrame);
    } else {
        PacketAudio(frameData, frameSize, pts);
    }
    *out = _cache->data();
    outSize = _cache->size();
    return 0;
}

int PSMuxer::PacketVideo(char* frameData, uint32_t frameSize, uint32_t pts, bool iFrame) {
    _cache->clear();
    int writeSize = 0;
    writeSize = MakePSHeader(_commonHeader, pts);
    _cache->append(_commonHeader, writeSize);
    if (iFrame) {
        writeSize = MakeSysMapHeader(_commonHeader);
        _cache->append(_commonHeader, writeSize);
    }

    uint32_t offset = 0;
    while (frameSize > MAX_PES_SIZE)
    {
        writeSize = MakePesHeader(_commonHeader, MAX_PES_SIZE, pts, 0);
        _cache->append(_commonHeader, writeSize);
        _cache->append(frameData + offset, MAX_PES_SIZE);

        offset += MAX_PES_SIZE;
        frameSize -= MAX_PES_SIZE;
    }

    writeSize = MakePesHeader(_commonHeader, frameSize, pts, 0);
    _cache->append(_commonHeader, writeSize);
    _cache->append(frameData + offset, frameSize);

    return 0;
}

int PSMuxer::PacketAudio(char* frameData, uint32_t frameSize, uint32_t pts) {
    _cache->clear();

    auto sz = MakePesHeader(_commonHeader,frameSize, pts,1);
    _cache->append((char*)_commonHeader,sz);
    _cache->append((char*)frameData, frameSize);
    return 0;
}

int PSMuxer::MakePSHeader(char* in, uint32_t pts)
{
	bits_buffer_t bits;
	bits_initwrite(&bits, 14, in);
	bits_write(&bits, 32, 0x000001BA); //4 bytes pack_start_code
	bits_write(&bits, 2, 0x01);

	bits_write(&bits, 3, (pts >> 30) & 0x07);
	bits_write(&bits, 1, 1); // marker
	bits_write(&bits, 15, (pts >> 15) & 0x7fff);
	bits_write(&bits, 1, 1); // marker
	bits_write(&bits, 15, pts & 0x7fff);
	bits_write(&bits, 1, 1); // marker

	bits_write(&bits, 9, 0); // src extension
	bits_write(&bits, 1, 1);     // marker

	bits_write(&bits, 22, 2000); //max rate
	bits_write(&bits, 1, 1);     // marker

	bits_write(&bits, 1, 1);     // marker
	bits_write(&bits, 5, 0x1f);  // reserved
	bits_write(&bits, 3, 0);     // stuffing bytes
	return 14;
}

int PSMuxer::MakeSysMapHeader(char* in)
{
	bits_buffer_t bits;
	bits_initwrite(&bits, 18 + 24, in);
	bits_write(&bits, 32, 0x01bb);
	bits_write(&bits, 16, 12);
	bits_write(&bits, 1, 1); // marker bit
	bits_write(&bits, 22, 2000);
	bits_write(&bits, 1, 1); // marker bit

	bits_write(&bits, 6, 1);
	bits_write(&bits, 1, 0); // fixed flag
	bits_write(&bits, 1, 0); // CSPS flag
	bits_write(&bits, 1, 1); // system audio lock flag
	bits_write(&bits, 1, 1); // system video lock flag

	bits_write(&bits, 1, 1); // marker bit

	bits_write(&bits, 5, 1);
	bits_write(&bits, 1, 0); // packet rate restriction flag (1 for mpeg1)
	bits_write(&bits, 7, 0xff); // reserved bits
	//video
	bits_write(&bits, 8, 0xe0);
	bits_write(&bits, 2, 0x03); /* reserved */
	bits_write(&bits, 1, 1);
	bits_write(&bits, 13, 256);
	//audio
	bits_write(&bits, 8, 0xc0);
	bits_write(&bits, 2, 0x03); /* reserved */
	bits_write(&bits, 1, 0);
	bits_write(&bits, 13, 16);
	/*system-header*/
	// 18 length;

	bits_write(&bits, 32, 0x01bc);
	bits_write(&bits, 16, 24 - 6);
	bits_write(&bits, 1, 1); /* current_next_indicator */
	bits_write(&bits, 2, 0xF); /* reserved */
	bits_write(&bits, 5, 1);
	bits_write(&bits, 7, 0xFF); /* reserved */
	bits_write(&bits, 1, 1); /* marker */

	bits_write(&bits, 16, 0); /* program_stream_info_length */
	/* empty */

	bits_write(&bits, 16, 8); /* elementary_stream_map_length */
	//12
	bits_write(&bits, 8, _videoStreamType); /* stream_type */
	bits_write(&bits, 8, 0xe0); /* elementary_stream_id video*/
	bits_write(&bits, 16, 0); /* elementary_stream_info_length */

	bits_write(&bits, 8, _audioStreamType); /* stream_type */
	bits_write(&bits, 8, 0xc0); /* elementary_stream_id audio*/
	bits_write(&bits, 16, 0); /* elementary_stream_info_length */
	bits_write(&bits, 32, 0x00000000);
	return 18 + 24;
}

int PSMuxer::MakePesHeader(char* in, uint32_t sz, uint32_t pts, uint8_t t) {
    bits_buffer_t bpes;
    bits_initwrite(&bpes, 14, in);
    bits_write(&bpes, 24, 0x01);
    if (t == 1)
        bits_write(&bpes, 8, 0xc0);
    else
        bits_write(&bpes, 8, 0xe0);

    bits_write(&bpes, 16, 8 + sz);

    bits_write(&bpes, 2, 0x02); // mpeg2 id
    bits_write(&bpes, 2, 0x00); // pes scrambling control
    bits_write(&bpes, 1, 0x00); // pes priority
    bits_write(&bpes, 1, 0x01); // data alignement indicator
    bits_write(&bpes, 1, 0x00); // copyright
    bits_write(&bpes, 1, 0x00); // original or copy

    bits_write(&bpes, 2, 0x02); // pts_dts flags
    bits_write(&bpes, 1, 0x00); // escr flags
    bits_write(&bpes, 1, 0x00); // es rate flag
    bits_write(&bpes, 1, 0x00); // dsm trick mode flag
    bits_write(&bpes, 1, 0x00); // additional copy info flag
    bits_write(&bpes, 1, 0x00); // pes crc flag
    bits_write(&bpes, 1, 0x00); // pes extension flag
    bits_write(&bpes, 8, 0x05); //pes header data length
    //9
    //pts dts flags
    bits_write(&bpes, 4, 0x02); // '0010' or '0011'
    bits_write(&bpes, 3, pts >> 30);
    bits_write(&bpes, 1, 0x01); // marker
    bits_write(&bpes, 15, pts >> 15);
    bits_write(&bpes, 1, 0x01); // marker
    bits_write(&bpes, 15, pts);
    bits_write(&bpes, 1, 0x01); // marker
    return 14;
}