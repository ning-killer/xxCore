#include "GBSdp.h"
#include <sstream>
#include <string.h>

std::vector<std::string> spilit_string(const std::string& str, const std::string& delim)
{
	std::vector<std::string> ret;
	size_t pre_pos = 0;
	std::string tmp;
	size_t pos = 0;
	do {
		pos = str.find(delim, pre_pos);
		tmp = str.substr(pre_pos, pos - pre_pos);
		ret.push_back(tmp);
		pre_pos = pos + delim.size();
	} while (pos != std::string::npos);

	return ret;
}


MediaPayloadType::MediaPayloadType(int payloadType) :_payloadType(payloadType)
{}
MediaPayloadType::~MediaPayloadType()
{}

void MediaPayloadType::encode(std::ostringstream& os)
{
	os << "a=rtpmap:" << _payloadType << " " << _encodingName << "/" << _samplingRate;
	os << "\r\n";
}

MediaDesc::MediaDesc(const std::string& type) :_type(type)
{}

MediaDesc::~MediaDesc()
{}

MediaPayloadType* MediaDesc::find_media_payload(int t)
{
	auto it = _payloadTypes.begin();
	while (it != _payloadTypes.end())
	{
		if (it->_payloadType == t)
			return &(*it);

		++it;
	}
	return NULL;
}

void MediaDesc::parse_rtpmap(const std::string& cnt)
{
	std::istringstream is(cnt);
	int payload;
	is >> payload;
	std::string tail;
	is >> tail;

	auto v = spilit_string(tail, "/");
	if (v.size() < 2)
		return;

	auto it = find_media_payload(payload);

	it->_encodingName = v[0];
	it->_samplingRate = atoi(v[1].c_str());
}

void MediaDesc::parse_fmtp(const std::string& cnt)
{
	std::istringstream is(cnt);
	int i;
	is >> i;
	std::string v;
	is >> v;
	auto it = find_media_payload(i);
	if (it)
		it->fmtp = v;
}

void MediaDesc::encode(std::ostringstream& os)
{
	os << "m=" << _type << " " << _port << " " << _protocol;
	for (auto& v : _payloadTypes)
	{
		os << " " << v._payloadType;
	}
	os << "\r\n";
	if (!mode.empty())
		os << "a=stream:" << mode << "\r\n";
	if (sendonly)
		os << "a=sendonly\r\n";
	if (recvonly)
		os << "a=recvonly\r\n";
	if (setup == 0 || setup == 1)
	{
		os << "a=setup:" << (setup == 0 ? "passive" : "active") << "\r\n";
		os << "a=connection:new\r\n";
	}
	for (auto& v : _payloadTypes)
	{
		v.encode(os);
	}
	if (downloadspeed > 0)
		os << "a=downloadspeed:" << downloadspeed << "\r\n";

	if (!exty.empty())
		os << "y=" << exty << "\r\n";
}

int GBSdp::parse(const std::string& msg)
{
	medias.clear();
	std::istringstream is(msg);
	std::string line;

	while (std::getline(is, line))
	{
		if (line.size() < 2)//malform
		{
			printf("sdp has malform line %s\n", line.c_str());
			continue;
		}
		std::string content = line.substr(2);
		if (content.back() == '\r')
			content.pop_back();

		switch (line[0])
		{
		case 'o':
			parse_origin(content);
			break;
		case 'v':
			break;
		case 's':
			parse_session_name(content);
			break;
		case 't':
			parse_t_line(content);
			break;
		case 'm':
			parse_media_line(content);
			break;
		case 'a':
			parse_attr_line(content);
			break;
		case 'c':
			break;
		case 'y':
			medias.back().exty = content;
			break;
		default:
			break;
		}
	}
	return 0;
}

std::string GBSdp::encode()
{
	std::ostringstream os;
	os << "v=0\r\n";
	os << "o=" << username_ << " " << session_id_ << " " << session_version_ << " " << nettype_ << " " << addrtype_ << " " << unicast_address_ << "\r\n";
	os << "s=" << session_name_ << "\r\n";
	if (!uri_.empty())
		os << "u=" << uri_ << "\r\n";
	os << "c=" << nettype_ << " " << addrtype_ << " " << unicast_address_ << "\r\n";
	os << "t=" << start_time_ << " " << end_time_ << "\r\n";

	for (auto& v : medias)
	{
		v.encode(os);
	}

	return os.str();
}

int GBSdp::parse_origin(const std::string& cnt)
{
	std::istringstream is(cnt);
	is >> username_;
	is >> session_id_;
	is >> session_version_;
	is >> nettype_;
	is >> addrtype_;
	is >> unicast_address_;
	return 0;
}

int GBSdp::parse_session_name(const std::string& cnt)
{
	session_name_ = cnt;
	return 0;
}

int GBSdp::parse_media_line(const std::string& cnt)
{
	std::istringstream is(cnt);
	std::string type;
	is >> type;

	int port;
	is >> port;

	std::string protocol;
	is >> protocol;

	medias.push_back(MediaDesc(type));
	medias.back()._port = port;
	medias.back()._protocol = protocol;

	int fmt;
	while (is >> fmt)
		medias.back()._payloadTypes.push_back(MediaPayloadType(fmt));

	return 0;
}

int GBSdp::parse_attr_line(const std::string& cnt)
{
	auto pos = cnt.find(':');

	auto tag = cnt.substr(0, pos);
	auto value = cnt.substr(pos + 1);

	if (tag == "rtpmap")
		medias.back().parse_rtpmap(value);
	else if (tag == "fmtp")
		medias.back().parse_fmtp(value);
	else if (tag == "recvonly")
		medias.back().recvonly = true;
	else if (tag == "sendonly")
		medias.back().sendonly = true;
	else if (tag == "setup")
	{
		if (value == "passive")
			medias.back().setup = 0;
		else if (value == "active")
			medias.back().setup = 1;
	}
	else if (tag == "stream")
		medias.back().mode = value;
	else if (tag == "downloadspeed")
		medias.back().downloadspeed = atoi(value.c_str());

	return 0;
}

int GBSdp::parse_t_line(const std::string& cnt)
{
	std::istringstream is(cnt);
	is >> start_time_;
	is >> end_time_;
	return 0;
}

void PlaybackControlInfo::Unserialize(std::string str)
{
	std::stringstream ss(str);
	std::string line;
	bool firstLine = true;
	while (std::getline(ss, line))
	{
		if (firstLine)
		{
			std::istringstream is(line);
			is >> Command;

			firstLine = false;
			continue;
		}
		if (line.back() == '\r')
			line.pop_back();
		auto pos = line.find(":");
		if (pos == line.npos)
			continue;
		auto h = line.substr(0, pos);
		auto v = line.substr(pos + 2); 
		if (strcasecmp(h.c_str(), "cseq") == 0)
			CSeq = std::stoi(v);
		else if (strcasecmp(h.c_str(), "range") == 0)
		{
			if (v == "npt=now-")
				Range = 0.f;
			else
				sscanf(v.c_str(), "npt=%f-", &Range);
		}
	}
}