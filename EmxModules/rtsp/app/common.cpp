#include "common.h"
#include <sstream>
#include <memory.h>

char const* dateHeader(const char* h, time_t tt)
{
    static char fmt[256];
    char buf[200];
#if !defined(_WIN32_WCE)
    strftime(buf, sizeof buf, "%a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
    snprintf(fmt, sizeof fmt, "%s: %s", h, buf);
#else
    // WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
    // so generate the "Date:" header a different, WinCE-specific way.
    // (Thanks to Pierre l'Hussiez for this code)
    // RSF: But where is the "Date: " string?  This code doesn't look quite right...
    SYSTEMTIME SystemTime;
    GetSystemTime(&SystemTime);
    WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
    WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
    WCHAR inBuf[200];
    DWORD locale = LOCALE_NEUTRAL;

    int ret = GetDateFormat(locale, 0, &SystemTime,
        (LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof inBuf);
    inBuf[ret - 1] = ' ';
    ret = GetTimeFormat(locale, 0, &SystemTime,
        (LPTSTR)timeFormat,
        (LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
    wcstombs(buf, inBuf, wcslen(inBuf));
#endif
    return fmt;
}

#define pj_hex_digits	"0123456789abcdef"

void pj_val_to_hex_digit(unsigned value, char* p)
{
    *p++ = pj_hex_digits[(value & 0xF0) >> 4];
    *p = pj_hex_digits[(value & 0x0F)];
}

bool init_seed = false;

int randInt()
{
    if (!init_seed)
    {
        init_seed = true;
        srand(time(NULL));
    }
    return rand();
}

std::string createNonce()
{
    char nonce_buf[16] = { 0 };
    size_t len = 16;
    char* p = nonce_buf;
    size_t i;

    for (i = 0; i < len / 8; ++i) {
        uint32_t val = randInt();
        pj_val_to_hex_digit((val & 0xFF000000) >> 24, p + 0);
        pj_val_to_hex_digit((val & 0x00FF0000) >> 16, p + 2);
        pj_val_to_hex_digit((val & 0x0000FF00) >> 8, p + 4);
        pj_val_to_hex_digit((val & 0x000000FF) >> 0, p + 6);
        p += 8;
    }
    for (i = i * 8; i < len; ++i) {
        *p++ = pj_hex_digits[randInt() & 0x0F];
    }
    return std::string(nonce_buf, 16);
}

HttpRequest::HttpRequest()
{
}
HttpRequest::~HttpRequest()
{
}

int HttpRequest::Parse(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;
	bool firstLine = true;
	while (std::getline(ss, line))
	{
		if (firstLine)
		{
			std::istringstream is(line);
			is >> _method;

			is >> _url;

			is >> _protocol;
			//if (strncmp("HTTP", _protocol.c_str(), 4))
			//	return 1;

			firstLine = false;
			continue;
		}
		line.pop_back();//ȥ��\r
		auto pos = line.find(":");
		if (pos != line.npos)
			_headers.insert({ line.substr(0, pos) ,line.substr(pos + 2) });
	}
	auto pos = _url.find("?");
	if (pos != _url.npos)
	{
		std::istringstream os(_url.substr(pos + 1));
		while (std::getline(os, line, '&'))
		{
			pos = line.find("=");
			if (pos == line.npos)
				continue;
			_params.insert({ line.substr(0,pos),line.substr(pos + 1) });
		}
	}
	return 0;
}

uint8_t HttpRequest::method()
{
	if (_method == "GET")
		return 0;
	else if (_method == "POST")
		return 1;
	else if (_method == "OPTIONS")
		return 2;
	return 3;
}

int HttpRequest::ContentLength()
{
	auto it = _headers.find("Content-Length");
	if (it != _headers.end())
		return atoi(it->second.c_str());

	return 0;
}

bool HttpRequest::hasOrigin()
{
	if (!_headers["Origin"].empty())
		return true;
	return false;
}

std::string HttpRequest::get(const std::string& key)
{
	auto it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return "";
}

std::string HttpRequest::QueryParam(const std::string& key)
{
	auto it = _params.find(key);
	if (it != _params.end())
		return it->second;
	return "";
}

bool HttpRequest::parseTransport()
{
	auto transport = _headers["Transport"];
	std::stringstream ss(transport);
	std::string item;
	while (std::getline(ss, item, ';'))
	{
		if (strcmp(item.c_str(), "RTP/AVP/TCP") == 0)
		{
			_streamMode = "TCP";
		}
		else if (strcmp(item.c_str(), "RAW/RAW/UDP") == 0 || strcmp(item.c_str(), "MP2T/H2221/UDP") == 0)
		{
			_streamMode = "RAW_UDP";
		}
		else if (strncasecmp(item.c_str(), "destination=", 12) == 0)
		{
			strcpy(destAddr, item.c_str() + 12);
		}
		//else if (sscanf(field, "ttl%u", &ttl) == 1)
		//{
		//	destinationTTL = (unsigned char)ttl;
		//}
		else if (sscanf(item.c_str(), "client_port=%hu-%hu", &rtpPort, &rtcpPort) == 2)
		{
			//clientRTPPortNum = p1;
			//clientRTCPPortNum = streamingMode == RAW_UDP ? 0 : p2; // ignore the second port number if the client asked for raw UDP
		}
		else if (sscanf(item.c_str(), "client_port=%hu", &rtpPort) == 1)
		{
			rtpPort = (_streamMode == "RAW_UDP") ? 0 : rtpPort + 1;
		}
		else if (sscanf(item.c_str(), "interleaved=%hu-%hu", &iRtp, &iRtcp) == 2)
		{

		}
	}

	return true;
}