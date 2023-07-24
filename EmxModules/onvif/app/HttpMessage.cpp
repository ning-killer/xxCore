#include "HttpMessage.h"
#include <sstream>
#include <string.h>

std::string http_status_text(int status)
{
	static std::map<int, std::string> status_map;
	if (status_map.empty())
	{
		status_map[100] = "Continue";
		status_map[101] = "Switching Protocols";
		status_map[200] = "OK";
		status_map[302] = "Found";
		status_map[304] = "Not Modified";
		status_map[400] = "Bad Request";
		status_map[401] = "Unauthorized";
		status_map[403] = "Forbidden";
		status_map[404] = "Not Found";
		status_map[405] = "Method Not Allowed";
		status_map[415] = "Unsupported Media Type";
		status_map[486] = "Device Busy";
		status_map[501] = "Not Implemented";
		status_map[503] = "Service Unavailable";
	}
	std::string text;
	auto it = status_map.find(status);
	if (it == status_map.end())
		text = "Status Unknown";
	else
		text = it->second;

	return text;
}

std::string http_createTime(time_t tt)
{
	char buf[256];
	strftime(buf, sizeof buf, "%a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
	return std::string(buf);
}

void parse_usr_pwd(std::string& body, std::string& u, std::string& p)
{
	std::stringstream ss(body);
	std::string line;
	while (std::getline(ss, line, '&'))
	{
		auto pos = line.find("=");
		if (pos == line.npos)
			return;
	
		auto k = line.substr(0, pos);
		auto v = line.substr(pos + 1);
		if (k == "username")
			u = std::move(v);
		else if (k == "password")
			p = std::move(v);
	}
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
			if (strncmp("HTTP", _protocol.c_str(), 4))
				return 1;

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

HttpResponse::HttpResponse()
{}

HttpResponse::~HttpResponse()
{}

void HttpResponse::setStatus(int status)
{
	_status = status;
}

void HttpResponse::setContentLength(size_t length)
{
	char cntLength[22];
	snprintf(cntLength, 22, "%lu", (unsigned long )length);
	set("Content-Length", cntLength);
}

void HttpResponse::setDate(std::string d, time_t tt)
{
	_headers[d] = http_createTime(tt);
}

void HttpResponse::set(std::string key, std::string value)
{
	_headers[key] = value;
}

std::string HttpResponse::toStr()
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << _status << " " << http_status_text(_status) << "\r\n";
	auto it = _headers.begin();
	while (it != _headers.end())
	{
		ss << it->first << ": " << it->second << "\r\n";
		++it;
	}
	ss << "\r\n";
	auto gg = ss.str();
	return ss.str();
}