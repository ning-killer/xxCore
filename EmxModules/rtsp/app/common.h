#pragma once
#include <string>
#include <time.h>
#include <map>

int randInt();

char const* dateHeader(const char* h, time_t tt);

std::string createNonce();

class HttpRequest
{
public:
	HttpRequest();
	~HttpRequest();

	int Parse(const std::string& data);
	/*
	* @return
	* 0 get 1 post 2 options
	*/
	uint8_t method();

	int ContentLength();

	bool hasOrigin();

	std::string get(const std::string& key);

	std::string Url()
	{
		return _url;
	}

	std::string QueryParam(const std::string& key);

	bool parseTransport();
public:
	//for tcp
	std::string _streamMode;
	unsigned short iRtp = 0;
	unsigned short iRtcp = 1;
	//for udp
	char destAddr[32];
	unsigned short rtpPort;
	unsigned short rtcpPort;
public:
	std::string _method;
private:
	std::map<std::string, std::string> _headers;
	
	std::string _url;
	std::string _protocol;


	std::map<std::string, std::string> _params;
};