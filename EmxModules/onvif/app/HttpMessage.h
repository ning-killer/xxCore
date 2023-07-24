#pragma once
#include <string>
#include <map>

std::string http_status_text(int status);

std::string http_createTime(time_t tt);

class WebHandler;

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
private:
	std::map<std::string, std::string> _headers;
	friend WebHandler;
	std::string _method;
	std::string _url;
	std::string _protocol;


	std::map<std::string, std::string> _params;
};

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();

	void setStatus(int status);

	void setContentLength(size_t length);

	void setDate(std::string d, time_t tt);

	void set(std::string key, std::string value);

	std::string toStr();
private:
	int _status = 200;
	std::map<std::string, std::string> _headers;
};