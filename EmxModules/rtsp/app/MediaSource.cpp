#include "MediaSource.h"
//#include <thread>
#include "sockutil.h"
#include "Base64.h"
#include "common.h"
#include "EmxCore.hpp"

const uint8_t* h265_nalu_find(const uint8_t* p, const uint8_t* end)
{
	for (p += 2; p + 1 < end; p++)
	{
		if (0x01 == *p && 0x00 == *(p - 1) && 0x00 == *(p - 2))
			return p + 1;
	}
	return end;
}

//H264File::H264File(std::string& _fileName)
//{
//	m_curPos = 0;
//	m_fileSize = 0;
//
//	pf = fopen(_fileName.c_str(), "rb");
//	//fopen_s(&pf, _fileName.c_str(), "rb");
//	fseek(pf, 0, SEEK_END);
//	m_fileSize = ftell(pf);
//	rewind(pf);
//	buf = new unsigned char[m_fileSize];
//	fread(buf, 1, m_fileSize, pf);
//	fclose(pf);
//	pf = NULL;
//}
//
//H264File::~H264File()
//{
//	if (pf)
//		fclose(pf);
//	if (buf)
//		delete buf;
//}
//
//int H264File::GetOneFrame(std::string& fr, bool& _IFrame)
//{
//	const uint8_t *p1, *p2, *pend;
//	pend = buf + m_fileSize;
//	bool endRead = false;
//	for (p1 = h265_nalu_find(buf + m_curPos, pend); p1 < pend;)
//	{
//		auto type = *p1 & 0x1f;
//		if (type == 5 || type == 1)
//		{
//			_IFrame = (type == 5);
//			endRead = true;
//		}
//
//		p1 = h265_nalu_find(p1 + 1, pend);
//		p2 = p1;
//		if (p1 == pend)
//			endRead = true;
//		else
//		{
//			p2 -= 2;
//			while (0 == *p2)p2--;
//			p2++;
//		}
//
//		if (endRead)
//		{
//			auto sz = p2 - (buf + m_curPos);
//			fr.append((char*)(buf + m_curPos), sz);
//			m_curPos += sz;
//			if (m_curPos >= m_fileSize)
//				m_curPos = 0;
//			break;
//		}
//	}
//	return 0;
//}
//
//H265File::H265File(std::string& _fileName)
//{
//	m_curPos = 0;
//	m_fileSize = 0;
//
//	pf = fopen(_fileName.c_str(), "rb");
//	//fopen_s(&pf, _fileName.c_str(), "rb");
//	fseek(pf, 0, SEEK_END);
//	m_fileSize = ftell(pf);
//	rewind(pf);
//	buf = new unsigned char[m_fileSize];
//	fread(buf, 1, m_fileSize, pf);
//	fclose(pf);
//	pf = NULL;
//}
//
//H265File::~H265File()
//{
//	if (pf)
//		fclose(pf);
//	if (buf)
//		delete buf;
//}
//
//int H265File::GetOneFrame(std::string& fr, bool& _IFrame)
//{
//	const uint8_t *p1, *p2, *pend;
//	pend = buf + m_fileSize;
//	bool endRead = false;
//	for (p1 = h265_nalu_find(buf + m_curPos, pend); p1 < pend;)
//	{
//		auto type = (*p1 & 0x7E) >> 1;
//		if ((type > 0 && type < 10) || (type > 15 && type < 22))
//		{
//			_IFrame = (type > 15 && type < 22);
//			endRead = true;
//		}
//
//		p1 = h265_nalu_find(p1 + 1, pend);
//		p2 = p1;
//		if (p1 == pend)
//			endRead = true;
//		else
//		{
//			p2 -= 2;
//			while (0 == *p2)p2--;
//			p2++;
//		}
//
//		if (endRead)
//		{
//			auto sz = p2 - (buf + m_curPos);
//			fr.append((char*)(buf + m_curPos), sz);
//			m_curPos += sz;
//			if (m_curPos >= m_fileSize)
//				m_curPos = 0;
//			break;
//		}
//	}
//	return 0;
//}

//copy from live555
unsigned removeH264or5EmulationBytes(uint8_t* to, unsigned toMaxSize, uint8_t const* from, unsigned fromSize)
{
	unsigned toSize = 0;
	unsigned i = 0;
	while (i < fromSize && toSize + 1 < toMaxSize)
	{
		if (i + 2 < fromSize && from[i] == 0 && from[i + 1] == 0 && from[i + 2] == 3)
		{
			to[toSize] = to[toSize + 1] = 0;
			toSize += 2;
			i += 3;
		}
		else
		{
			to[toSize] = from[i];
			toSize += 1;
			i += 1;
		}
	}

	return toSize;
}

MediaSource::MediaSource(const std::string& videoCodec, std::string& audioCodec, uint32_t audioSamplingRate)
	:_videoCodec(videoCodec), _audioCodec(audioCodec), _audioSamplingRate(audioSamplingRate)
{
	_videoSSRC = randInt();
	_audioSSRC = randInt();
	rtpPool = std::make_shared<ResourcePool<BufferRaw>>();
	rtpPool->setSize(256);

	_gopPoll = std::make_shared<ResourcePool<RTPFrame>>();
	_gopPoll->setSize(2);//gop һ��100֡����

	if (_audioCodec == "PCMA")
		_audioPT = 8;
	else if (_audioCodec == "PCMU")
		_audioPT = 0;
    emxlogd("construct MediaSource:%p, _videoCodec:%s, _audioCodec:%s\n", this, _videoCodec.c_str(), _audioCodec.c_str());
}

MediaSource::~MediaSource()
{
	exitFlag = true;
	if (_th)
		_th->join();
    _dispatcher.clear();
	delete[] _sps;
	delete[] _pps;
	if (_vps)
		delete[] _vps;
	emxlogd("deconstruct MediaSource:%p\n", this);
}

int MediaSource::inputFrame(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame, unsigned char type)
{
	if (type == 0 && _videoCodec == "H264")
        AVCBuildRTP(payload, payload_size, pts, keyFrame);
	else if (type == 0 && _videoCodec == "H265")
	    HEVCBuildRTP(payload, payload_size, pts, keyFrame);
	else if (type == 1 && (_audioCodec == "PCMU" || _audioCodec == "PCMA"))
		G711BuildRTP(payload, payload_size, pts);
	else if (type == 1 && _audioCodec == "AAC")
		ADTSBuildRTP(payload, payload_size, pts);
	return 0;
}

void MediaSource::run()
{
	_th = std::make_shared<std::thread>(&MediaSource::GetFrameFunc, this);
}

unsigned MediaSource::GetFrameFunc()
{
	//std::string fileName = "test.h264";
	//H264File hf(fileName);
	////std::string fileName = "2.h265";
	////H265File hf(fileName);
	//std::string frame;
	//bool iFrame;
	//uint32_t pts = 0;
	//while (!exitFlag)
	//{
	//	hf.GetOneFrame(frame, iFrame);


	//	inputFrame((unsigned char*)frame.c_str(), frame.size(), pts * 90, iFrame, 0);
	//	pts += 40;
	//	frame.clear();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(35));
	//}
	return 0;
}

static unsigned const samplingFrequencyFromIndex[16] = {
  96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
  16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

std::string MediaSource::getSDP()
{
	if (!_sdp.empty())
		return _sdp;

	_sdp += "v=0\r\n";
	_sdp += "o=- 0 0 IN IP4 0.0.0.0\r\n";
	_sdp += "s=Pusher\r\n";
	_sdp += "c=IN IP4 0.0.0.0\r\n";
	_sdp += "t=0 0\r\n";
	_sdp += "m=video 0 RTP/AVP 96\r\n";
	if (!_videoCodec.empty())
	{
		char tmpBuf[64];
		snprintf(tmpBuf,sizeof(tmpBuf),"a=rtpmap:%d %s/90000\r\n", _videoPT, _videoCodec.c_str());
		_sdp += tmpBuf;

		if (_vps && _sps && _pps)/*H265*/
		{
			uint8_t* vpsWEB = new uint8_t[vpsSize]; // "WEB" means "Without Emulation Bytes"
			unsigned vpsWEBSize = removeH264or5EmulationBytes(vpsWEB, vpsSize, (uint8_t*)_vps, vpsSize);
			if (vpsWEBSize < 6/*'profile_tier_level' offset*/ + 12/*num 'profile_tier_level' bytes*/) {
				// Bad VPS size => assume our source isn't ready
				delete[] vpsWEB;
				return NULL;
			}
			uint8_t const* profileTierLevelHeaderBytes = &vpsWEB[6];
			unsigned profileSpace = profileTierLevelHeaderBytes[0] >> 6; // general_profile_space
			unsigned profileId = profileTierLevelHeaderBytes[0] & 0x1F; // general_profile_idc
			unsigned tierFlag = (profileTierLevelHeaderBytes[0] >> 5) & 0x1; // general_tier_flag
			unsigned levelId = profileTierLevelHeaderBytes[11]; // general_level_idc
			uint8_t const* interop_constraints = &profileTierLevelHeaderBytes[5];
			char interopConstraintsStr[100];
			snprintf(interopConstraintsStr, 100, "%02X%02X%02X%02X%02X%02X",
				interop_constraints[0], interop_constraints[1], interop_constraints[2],
				interop_constraints[3], interop_constraints[4], interop_constraints[5]);
			delete[] vpsWEB;
			Base64 base64;
			char* sprop_vps = base64.base64Encode((char*)_vps, vpsSize);
			char* sprop_sps = base64.base64Encode((char*)_sps, spsSize);
			char* sprop_pps = base64.base64Encode((char*)_pps, ppsSize);

			char const* fmtpFmt =
				"a=fmtp:%d profile-space=%u"
				";profile-id=%u"
				";tier-flag=%u"
				";level-id=%u"
				";interop-constraints=%s"
				";sprop-vps=%s"
				";sprop-sps=%s"
				";sprop-pps=%s\r\n";

			char* fmtp = new char[256];
			snprintf(fmtp, 256, fmtpFmt,
				96, profileSpace,
				profileId,
				tierFlag,
				levelId,
				interopConstraintsStr,
				sprop_vps,
				sprop_sps,
				sprop_pps);
			_sdp += fmtp;
			delete[] sprop_vps;
			delete[] sprop_sps;
			delete[] sprop_pps;
			delete[] fmtp;
		}
		else if (_sps && _pps)/*H264*/
		{
			Base64 base64;
			auto sps_base64 = base64.base64Encode(_sps, spsSize);
			auto pps_base64 = base64.base64Encode(_pps, ppsSize);

			uint8_t* spsWEB = new uint8_t[spsSize]; // "WEB" means "Without Emulation Bytes"
			unsigned spsWEBSize = removeH264or5EmulationBytes(spsWEB, spsSize, (uint8_t*)_sps, spsSize);
			if (spsWEBSize < 4) { // Bad SPS size => assume our source isn't ready
				delete[] spsWEB;
				return "";
			}
			uint32_t profileLevelId = (spsWEB[1] << 16) | (spsWEB[2] << 8) | spsWEB[3];
			char const* fmtpFmt =
				"a=fmtp:%d packetization-mode=1"
				";sprop-parameter-sets=%s,%s"
				";profile-level-id=%06X\r\n";
			char* fmtp = new char[256];//256 maybe enough
			snprintf(fmtp, 256, fmtpFmt, 96, sps_base64, pps_base64, profileLevelId);
			_sdp += fmtp;
			delete[] sps_base64;
			delete[] pps_base64;
			delete[] spsWEB;
			delete[] fmtp;
		}
		_sdp += "a=control:" + _videoControlPath + "\r\n";
	}
	else
		return "";

	if (!_audioCodec.empty())
	{
		char tmpBuf[64];
		if (_audioCodec == "PCMU")
		{
			_sdp += "m=audio 0 RTP/AVP 0\r\n";
			snprintf(tmpBuf, sizeof(tmpBuf),"a=rtpmap:0 PCMU/%d\r\n", _audioSamplingRate);
			 _sdp += tmpBuf;

		}
		else if (_audioCodec == "PCMA")
		{
			_sdp += "m=audio 0 RTP/AVP 8\r\n";
			snprintf(tmpBuf, sizeof(tmpBuf),"a=rtpmap:8 PCMA/%d\r\n", _audioSamplingRate);
			_sdp += tmpBuf;

		}
		else if (_audioCodec == "AAC")
		{
			_sdp += "m=audio 0 RTP/AVP 97\r\n";
		    snprintf(tmpBuf, sizeof(tmpBuf),"a=rtpmap:97 mpeg4-generic/%d/1\r\n", _audioSamplingRate);
			_sdp += tmpBuf;
            uint8_t channelConfiguration = 1;// single channel
            int samplingFrequencyIndex = 0;
            for (size_t i = 0; i < 16; i++)
            {
                if (samplingFrequencyFromIndex[i] == _audioSamplingRate)
                {
                    samplingFrequencyIndex = i;
                    break;
                }
            }
            /*
            0 main profile
            1 low complexity profile(LC)
            2 scalable sampling rate profile (SSR)
            3 reserved
            */
            unsigned char profile = 1;
            uint8_t audioConfig[2] = { 0 };
            uint8_t const aacObjectType = profile + 1;
            audioConfig[0] = (aacObjectType << 3) | (samplingFrequencyIndex >> 1);
            audioConfig[1] = (samplingFrequencyIndex << 7) | (channelConfiguration << 3);
            //for aac
            char const* fmtpFmt =
                    "a=fmtp:%d "
                    "streamtype=%d;profile-level-id=1;"
                    "mode=%s;sizelength=13;indexlength=3;indexdeltalength=3;"
                    "config=%02X%02x\r\n";
            char* fmtp = new char[256];//256 maybe enough
            snprintf(fmtp, 256, fmtpFmt, 97, 5, "AAC-hbr", audioConfig[0], audioConfig[1]);
            _sdp += fmtp;
            delete[] fmtp;
		}

		_sdp += "a=control:" + _audioControlPath + "\r\n";
	}
	return _sdp;
}

int MediaSource::AVCBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame)
{
	auto mp = SplitAVCNalu(payload, payload_size);
	auto sz = mp.size();
	if (sz > 0)
	{
		auto frame = _gopPoll->obtain();
		frame->reset();
		frame->_interval = 0;
		frame->_isKey = keyFrame;

		unsigned offset = 0;
		int i = 0;
		while (offset < payload_size && i < (int)sz)
		{
			offset = mp[i].first;
			auto naluSize = mp[i].second;
			if (naluSize <= MTU)
			{
				auto pkt = rtpPool->obtain();
				auto rtp = pkt->data() + 4;
				packHeader(rtp, (offset + naluSize == payload_size), pts, 0);
				memcpy(rtp + 12, payload + offset, naluSize);
				auto rtpSize = 12 + naluSize;
				rtp[-4] = 0x24;
				rtp[-3] = 0;
				rtp[-2] = (rtpSize >> 8) & 0xFF;
				rtp[-1] = rtpSize & 0xFF;
				pkt->setSize(4 + rtpSize);

				frame->_pktCache.emplace_back(std::move(pkt));
			}
			else
			{
				unsigned char type = (payload + offset)[0];
				//	--	FU indicator	--
				//	--	+---------------+	--
				//	--	|0|1|2|3|4|5|6|7|	--
				//	--	+-+-+-+-+-+-+-+-+	--
				//	--	|F|NRI|  Type(FUA)  |	--
				//	--	+---------------+	--
				unsigned char fu_indicator = ((0xE0 & type) | NALU_TYPE_FU_A);

				auto pkt = rtpPool->obtain();
				auto rtp = pkt->data() + 4;
				packHeader(rtp, 0, pts, 0);

				rtp[12] = fu_indicator;
				//	--	FU header	--
				//	--	+---------------+	--
				//	--	|0|1|2|3|4|5|6|7|	--
				//	--	+-+-+-+-+-+-+-+-+	--
				//	--	|S|E|R|  Type(Frame)|	--
				//	--	+---------------+	--
				rtp[13] = 0x80 | (0x1F & type);
				offset += 1;
				naluSize -= 1;
				while (naluSize > MTU - 2)
				{
					memcpy(rtp + 14, payload + offset, MTU - 2);
					auto rtpSize = 12 + MTU;
					rtp[-4] = 0x24;
					rtp[-3] = 0;
					rtp[-2] = (rtpSize >> 8) & 0xFF;
					rtp[-1] = rtpSize & 0xFF;
					pkt->setSize(4 + rtpSize);
					frame->_pktCache.emplace_back(std::move(pkt));

					offset += MTU - 2;
					naluSize -= MTU - 2;

					pkt = rtpPool->obtain();
					rtp = pkt->data() + 4;
					if (naluSize <= (MTU - 2))
						packHeader(rtp, true, pts, 0);
					else
						packHeader(rtp, false, pts, 0);
					rtp[12] = fu_indicator;
					rtp[13] = 0x00 | (0x1F & type);
				}

				rtp[13] = 0x40 | (0x1F & type);
				memcpy(rtp + 14, payload + offset, naluSize);

				auto rtpSize = 12 + 2 + naluSize;
				rtp[-4] = 0x24;
				rtp[-3] = 0;
				rtp[-2] = (rtpSize >> 8) & 0xFF;
				rtp[-1] = rtpSize & 0xFF;
				pkt->setSize(4 + rtpSize);

				frame->_pktCache.emplace_back(std::move(pkt));

				offset += naluSize;
			}
			i++;
		}//while

		std::lock_guard<std::mutex> lck(_testLock);
		if (frame->_isKey)
			_gopCache.clear();

		//_gopCache.emplace_back(frame);
		
		for (auto& ptr : _dispatcher)
		{
			auto& second = ptr.second;
			ptr.first->async([second,frame]()
			{
				second->write(frame);
			});
		}
	}//
	return 0;
}

int MediaSource::HEVCBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts, bool keyFrame)
{
	auto mp = SplitHEVCNalu(payload, payload_size);
	auto sz = mp.size();
	if (sz > 0)
	{
		auto frame = _gopPoll->obtain();
		frame->reset();
		frame->_interval = 0;
		frame->_isKey = keyFrame;

		unsigned offset = 0;
		int i = 0;
		while (offset < payload_size && i < (int)sz)
		{
			offset = mp[i].first;
			auto naluSize = mp[i].second;
			if (naluSize <= MTU)
			{
				auto pkt = rtpPool->obtain();
				auto rtp = pkt->data() + 4;
				packHeader(rtp, (offset + naluSize == payload_size), pts, 0);
				memcpy(rtp + 12, payload + offset, naluSize);
				auto rtpSize = 12 + mp[i].second;
				rtp[-4] = 0x24;
				rtp[-3] = 0;
				rtp[-2] = (rtpSize >> 8) & 0xFF;
				rtp[-1] = rtpSize & 0xFF;
				pkt->setSize(4 + rtpSize);

				frame->_pktCache.emplace_back(std::move(pkt));

				offset += naluSize;
			}
			else
			{
				unsigned char a = payload[offset];
				unsigned char b = payload[offset + 1];
				unsigned char type = (a & 0x7E) >> 1;

				auto pkt = rtpPool->obtain();
				auto rtp = pkt->data() + 4;
				packHeader(rtp, false, pts, 0);

				rtp[12] = (a & 0x81) | (49 << 1);
				rtp[13] = b;
				rtp[14] = 0x80 | type;
				offset += 2;
				naluSize -= 2;
				while (naluSize > MTU - 3)
				{
					memcpy(rtp + 15, payload + offset, MTU - 3);
					auto rtpSize = 12 + MTU;
					rtp[-4] = 0x24;
					rtp[-3] = 0;
					rtp[-2] = (rtpSize >> 8) & 0xFF;
					rtp[-1] = rtpSize & 0xFF;
					pkt->setSize(4 + rtpSize);

					frame->_pktCache.emplace_back(std::move(pkt));
					//next
					naluSize -= MTU - 3;
					offset += MTU - 3;

					pkt = rtpPool->obtain();
					rtp = pkt->data() + 4;
					if (naluSize <= (MTU - 3))
						packHeader(rtp, true, pts, 0);
					else
						packHeader(rtp, false, pts, 0);

					rtp[12] = (a & 0x81) | (49 << 1);
					rtp[13] = b;
					rtp[14] = 0x00 | type;
				}

				rtp[13] = b;
				rtp[14] = 0x40 | type;
				memcpy(rtp + 15, payload + offset, naluSize);

				auto rtpSize = 12 + 3 + naluSize;
				rtp[-4] = 0x24;
				rtp[-3] = 0;
				rtp[-2] = (rtpSize >> 8) & 0xFF;
				rtp[-1] = rtpSize & 0xFF;
				pkt->setSize(4 + rtpSize);

				frame->_pktCache.emplace_back(std::move(pkt));

				offset += naluSize;
			}
			i++;
		}

		std::lock_guard<std::mutex> lck(_testLock);
		if (frame->_isKey)
			_gopCache.clear();

		_gopCache.emplace_back(frame);

		for (auto& ptr : _dispatcher)
		{
			auto& second = ptr.second;
			ptr.first->async([second, frame]()
			{
				second->write(frame);
			});
		}
	}
	return 0;
}

int MediaSource::G711BuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts)
{
	auto frame = _gopPoll->obtain();
	frame->reset();
	frame->_interval = 2;
	frame->_isKey = false;

	auto pkt = rtpPool->obtain();
	auto rtp = pkt->data() + 4;
	packHeader(rtp, true, pts, 1);
	memcpy(rtp + 12, payload, payload_size);

	auto rtpSize = 12 + payload_size;
	rtp[-4] = 0x24;
	rtp[-3] = 2;
	rtp[-2] = (rtpSize >> 8) & 0xFF;
	rtp[-1] = rtpSize & 0xFF;
	pkt->setSize(4 + rtpSize);
	frame->_pktCache.emplace_back(std::move(pkt));

	std::lock_guard<std::mutex> lck(_testLock);
	if (frame->_isKey)
		_gopCache.clear();

	//_gopCache.emplace_back(frame);

	for (auto& ptr : _dispatcher)
	{
		auto& second = ptr.second;
		ptr.first->async([second, frame]()
		{
			second->write(frame);
		});
	}

	return 0;
}

int MediaSource::ADTSBuildRTP(const unsigned char* payload, size_t payload_size, unsigned int pts)
{
	auto frame = _gopPoll->obtain();
	frame->reset();
	frame->_interval = 2;
	frame->_isKey = false;

	auto pkt = rtpPool->obtain();
	auto rtp = pkt->data() + 4;
	packHeader(rtp, true, pts, 1);

	unsigned short aacLen = payload_size - 7;

	rtp[12] = 0x00;
	rtp[13] = 0x10;

	rtp[14] = (aacLen & 0x1FE0) >> 5;
	rtp[15] = (aacLen & 0x1F) << 3;

	memcpy(rtp + 16, payload + 7, aacLen);

	auto rtpSize = 12 + 4 + aacLen;
	rtp[-4] = 0x24;
	rtp[-3] = 2;
	rtp[-2] = (rtpSize >> 8) & 0xFF;
	rtp[-1] = rtpSize & 0xFF;
	pkt->setSize(4 + rtpSize);
	frame->_pktCache.emplace_back(std::move(pkt));

	std::lock_guard<std::mutex> lck(_testLock);
	if (frame->_isKey)
		_gopCache.clear();

	// _gopCache.emplace_back(frame);

	for (auto& ptr : _dispatcher)
	{
		auto& second = ptr.second;
		ptr.first->async([second, frame]()
		{
			second->write(frame);
		});
	}
	return 0;
}

std::vector<std::pair<int, int>> MediaSource::SplitAVCNalu(const unsigned char* payload, size_t payload_size)
{
	result.clear();

	const uint8_t *p1, *p2, *pend;
	pend = payload + payload_size;
	size_t nalu_size;
	for (p1 = h265_nalu_find(payload, pend); p1 < pend;p1=p2)
	{
		auto type = *p1 & 0x1f;
		if (type == 5 || type == 1)
		{
			result.push_back({ p1 - payload, pend - p1 });
			break;
		}

		p2 = h265_nalu_find(p1 + 1, pend);
		nalu_size = p2 - p1;

		// filter suffix '00' bytes
		if (p2 != pend) --nalu_size;
		int i = 0;
		while (0 == p1[nalu_size - 1] && i < 3)
		{
			--nalu_size;
			i++;
		}

		if (!_sps && type == 7)
		{
			spsSize = nalu_size;
			_sps = new char[spsSize];
			memcpy(_sps, p1, spsSize);
		}
		else if (!_pps && type == 8)
		{
			ppsSize = nalu_size;
			_pps = new char[ppsSize];
			memcpy(_pps, p1, ppsSize);
		}
		result.push_back({ p1 - payload, nalu_size });
	}
	return result;
}

std::vector<std::pair<int, int>> MediaSource::SplitHEVCNalu(const unsigned char* payload, size_t payload_size)
{
	result.clear();

	const uint8_t* p1, * p2, * pend;
	pend = payload + payload_size;
	size_t nalu_size;
	for (p1 = h265_nalu_find(payload, pend); p1 < pend; p1 = p2)
	{
		auto type = (*p1 & 0x7E) >> 1;
		if ((type > 0 && type < 10) || (type > 15 && type < 22))
		{
			result.push_back({ p1 - payload, pend - p1 });
			break;
		}

		p2 = h265_nalu_find(p1 + 1, pend);
		nalu_size = p2 - p1;

		// filter suffix '00' bytes
		if (p2 != pend) --nalu_size;
		int i = 0;
		while (0 == p1[nalu_size - 1] && i < 3)
		{
			--nalu_size;
			i++;
		}

		if (!_sps && type == 33)
		{
			spsSize = nalu_size;
			_sps = new char[spsSize];
			memcpy(_sps, p1, spsSize);
		}
		else if (!_pps && type == 34)
		{
			ppsSize = nalu_size;
			_pps = new char[ppsSize];
			memcpy(_pps, p1, ppsSize);
		}
		else if (!_vps && type == 32)
		{
			vpsSize = nalu_size;
			_vps = new char[vpsSize];
			memcpy(_vps, p1, vpsSize);
		}
		result.push_back({ p1 - payload, nalu_size });
	}
	return result;
}

void MediaSource::packHeader(char* in, bool mark, uint32_t timeStamp, unsigned char type)
{
	in[0] = 0x80;//V_P_X
	in[1] = mark ? (1 << 7) : 0;
	if (type == 0)
	{
		in[1] |= _videoPT;/*M_PT*/
		in[2] = _videoSeq >> 8;
		in[3] = (_videoSeq & 0xff);/*seq*/
		_videoSeq++;

		in[8] = _videoSSRC >> 24;
		in[9] = _videoSSRC >> 16;
		in[10] = _videoSSRC >> 8;
		in[11] = (_videoSSRC & 0xff);
	}
	else
	{
		in[1] |= _audioPT;/*M_PT*/
		in[2] = _audioSeq >> 8;
		in[3] = (_audioSeq & 0xff);/*seq*/
		_audioSeq++;

		in[8] = _audioSSRC >> 24;
		in[9] = _audioSSRC >> 16;
		in[10] = _audioSSRC >> 8;
		in[11] = (_audioSSRC & 0xff);
	}
	in[4] = timeStamp >> 24;
	in[5] = timeStamp >> 16;
	in[6] = timeStamp >> 8;
	in[7] = (timeStamp & 0xff);
}

void MediaSource::bbc(EventPoller::Ptr& poller, std::function<int(const RTPFrame::Ptr&)>&& cb)
{
	RtspDispatcher::Ptr dp;

	std::lock_guard<std::mutex> lck(_testLock);
	auto& ref = _dispatcher[poller];
	if (!ref)
		ref = std::make_shared<RtspDispatcher>(_gopCache);
	dp = ref;
	dp->addReader(std::move(const_cast<std::function<int(const RTPFrame::Ptr&)>&>(cb)));
}
