#pragma once
#include <memory>
#include <functional>
#include <map>
#include <string>

class SoapDispatcher :public std::enable_shared_from_this<SoapDispatcher>
{
public:
	~SoapDispatcher();

	void Init();

	static SoapDispatcher& Instance();

	bool QueryHandler(const std::string& suffixUri, const std::string& tag, std::function<int(void*, std::string&)>& fn);
private:
	SoapDispatcher();
private:
	std::map<std::string, std::map<std::string, std::function<int(void*, std::string&)>>> mapFunc;
};

