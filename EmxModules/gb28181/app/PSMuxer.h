#pragma once
#include <stdint.h>
#include "Buffer.h"

class PSMuxer
{
public:
	PSMuxer(uint8_t vType, uint8_t aType = 0x90);
	~PSMuxer();
	/*
	* @param frameData 媒体数据
	* @param frameSize 
	* @param pts 时间戳 毫秒*90
	* @param iFrame 是否是I帧
	* @param t 媒体数据类型 0-视频 1-音频
	* @param out ps数据包
	* @param outSize ps数据包长度
	*/
	int PacketMediaData(char* frameData, uint32_t frameSize, uint32_t pts, bool iFrame, uint8_t t, char** out, uint32_t& outSize);
private:
    int PacketVideo(char* frameData, uint32_t frameSize, uint32_t pts, bool iFrame);

    int PacketAudio(char* frameData, uint32_t frameSize, uint32_t pts);

	int MakePSHeader(char* in, uint32_t pts);

	int MakeSysMapHeader(char* in);

	int MakePesHeader(char* in, uint32_t sz, uint32_t pts, uint8_t t);
private:
    uint8_t _videoStreamType = 0x1b;/*0x1b h264,0x24 h265*/
	uint8_t _audioStreamType = 0x90;
	char _commonHeader[14 + 18 + 24] = { 0 };
    BufferRaw::Ptr _cache;

	uint8_t _audioSpecificConfig[2] = { 0 };
};