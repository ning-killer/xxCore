#pragma once
#include <string>
#include <vector>

class MediaPayloadType
{
public:
	MediaPayloadType(int payloadType);
	~MediaPayloadType();
public:
	int _payloadType;
	std::string _encodingName;
	int _samplingRate = 90000;
	std::string _encodingParam;

	std::string fmtp;

	void encode(std::ostringstream& os);
};

class MediaDesc
{
public:
	MediaDesc(const std::string& type);
	~MediaDesc();
public:
	void parse_rtpmap(const std::string& cnt);

	void parse_fmtp(const std::string& cnt);

	void encode(std::ostringstream& os);

	bool is_audio() const { return _type == "audio"; }
	bool is_video() const { return _type == "video"; }
private:
	MediaPayloadType* find_media_payload(int t);
public:
	std::string _type;
	int _port;
	std::string _protocol;

	bool sendonly = false;
	bool recvonly = false;

	uint8_t setup = 2;//0-passive,1-active,2-no
	std::string exty;
	int downloadspeed = 0;
	std::string mode;//main sub

	std::vector<MediaPayloadType> _payloadTypes;
};

class GBSdp
{
public:
	int parse(const std::string& msg);

	std::string encode();
private:
	int parse_origin(const std::string& cnt);

	int parse_session_name(const std::string& cnt);

	int parse_media_line(const std::string& cnt);

	int parse_attr_line(const std::string& cnt);

	int parse_t_line(const std::string& cnt);
public:
	std::vector<MediaDesc>  medias;

	// origin
	std::string username_;
	std::string session_id_ = "0";
	std::string session_version_ = "0";
	std::string nettype_ = "IN";
	std::string addrtype_ = "IP4";
	std::string unicast_address_;

	// session_name
	std::string session_name_;
	std::string uri_;
	// time
	time_t start_time_ = 0;
	time_t end_time_ = 0;
};

class PlaybackControlInfo
{
public:
	std::string Command;//PLAY PAUSE
	uint32_t CSeq;
	float Scale = 0.f;
	float Range = 0.f;//npt=now- npt=30-

	void Unserialize(std::string str);
};