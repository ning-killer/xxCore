#pragma once
#include <string>
#include <vector>

class PTZControl
{
public:
	PTZControl(const std::string& channelID);
	~PTZControl();

	int ptzGO(const std::string& strCMD);

	std::string QueryPreset(const std::string& strSN);
private:
	std::vector<int> ParseControlStr(const std::string& strCMD);
private:
	std::string m_channelID;
};

