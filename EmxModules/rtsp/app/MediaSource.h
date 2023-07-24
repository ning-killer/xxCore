#pragma once
#include <memory>
#include <string>
#include "Buffer.h"
#include "ResourcePool.h"
#include <deque>
#include <unordered_map>
#include "EventPoller.h"
#include "RtspDispatcher.h"

#define MTU 1440
#define NALU_TYPE_FU_A 0x1C

enum HEVC_TYPE
{
	NAL_BLA_W_LP = 16,
	NAL_VPS = 32,
	NAL_SPS = 33,
	NAL_PPS = 34,
	NAL_SEI_PREFIX = 39
};

//class H265File
//{
//public:
//	H265File(std::string& _fileName);
//	~H265File();
//
//	int GetOneFrame(std::string& fr, bool& _IFrame);
//
//private:
//	FILE* pf;
//	unsigned int m_curPos;
//	unsigned int m_fileSize;
//	unsigned char* buf;
//};
//
//class H264File
//{
//public:
//	H264File(std::string& _fileName);
//	~H264File();
//
//	int GetOneFrame(std::string& fr, bool& _IFrame);
//
//private:
//	FILE* pf;
//	unsigned int m_curPos;
//	unsigned int m_fileSize;
//	unsigned char* buf;
//};

class MediaSource :public std::enable_shared_from_this<MediaSource>
{
public:
	MediaSource(const std::string& videoCodec, std::string& audioCodec, uint32_t audioSamplingRate = 16000);
	~MediaSource();

	std::string getSDP();

	void run();
	/*  
	 * @param
	 * type 0 video 1 auido
	*/
	int inputFrame(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame, unsigned char type);

	void bbc(EventPoller::Ptr& poller, std::function<int(const RTPFrame::Ptr&)>&& cb);

	std::string getVideoCodec()
	{
		return _videoCodec;
	}
private:
	unsigned GetFrameFunc();

private:
	int AVCBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame);

	int HEVCBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame);

	int G711BuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts);

	int ADTSBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts);

	std::vector<std::pair<int, int>> SplitAVCNalu(const unsigned char* payload, size_t payload_size);

	std::vector<std::pair<int, int>> SplitHEVCNalu(const unsigned char* payload, size_t payload_size);

	void packHeader(char* in, bool mark, uint32_t timeStamp, unsigned char type);
private:
	bool exitFlag = false;
	std::string _videoCodec = "H264";
	std::string _audioCodec = "PCMU";

	std::string _videoControlPath = "track=0";
	std::string _audioControlPath = "track=1";

	std::shared_ptr<ResourcePool<BufferRaw>> rtpPool;

	unsigned short _videoSeq = 1;
	unsigned short _audioSeq = 1;
	uint32_t _videoSSRC;
	uint32_t _audioSSRC;
	unsigned char _videoPT = 96;
	unsigned char _audioPT = 97;
	unsigned int _audioSamplingRate = 16000;
	std::string _sdp;

	std::mutex _readerLock;
	std::list<std::function<int(std::list<BufferRaw::Ptr>&)>> _readers;
	
	std::mutex _testLock;
	std::deque<RTPFrame::Ptr> _gopCache;
	std::shared_ptr<ResourcePool<RTPFrame>> _gopPoll;

	std::unordered_map<EventPoller::Ptr, RtspDispatcher::Ptr> _dispatcher;

	std::shared_ptr<std::thread> _th;
	std::vector<std::pair<int, int>> result;
	char* _sps = nullptr;
	unsigned short spsSize = 0;
	char* _pps = nullptr;
	unsigned short ppsSize = 0;
	char* _vps = nullptr;
	unsigned short vpsSize = 0;
};

