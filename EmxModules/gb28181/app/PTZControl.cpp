#include "PTZControl.h"
#include <iostream>

PTZControl::PTZControl(const std::string& channelID):m_channelID(channelID)
{

}

PTZControl::~PTZControl()
{

}

int PTZControl::ptzGO(const std::string& strCMD)
{
	auto v = ParseControlStr(strCMD);
	if (v[0] != 0xA5)
	{
		return 1;
	}
	switch (v[3])
	{
	case 0x08://up
	{
		std::cout << "ptz up{" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
//		auto speed = v[5];
	}break;
	case 0x04://down
	{
		std::cout << "ptz down{" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
//		auto speed = v[5];
	}break;
	case 0x02://left
	{
		std::cout << "ptz left" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
//		auto speed = v[4];
	}break;
	case 0x01://right
	{
		std::cout << "ptz right" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x09://right+up
	{
		std::cout << "ptz right+up" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x0A://left+up
	{
		std::cout << "ptz left+up" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x05://right+down
	{
		std::cout << "ptz right+down" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x06://left+down
	{
		std::cout << "ptz left+down" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x10://��ͷ�Ŵ�
	{
		std::cout << "��ͷ�Ŵ�" << std::endl;
	}break;
	case 0x20://��ͷ��С
	{
		std::cout << "��ͷ��С" << std::endl;
	}break;
	case 0x00:
	{
		std::cout << "ptz stop" << "\"horizon\":" << v[4] << ",\"vertical\":" << v[5] << "}" << std::endl;
	}break;
	case 0x81:/*set preset*/
	{
//		int num = v[5];
	}break;
	case 0x82:/*call preset*/
	{
//		int num = v[5];
	}break;
	case 0x83:/*delete preset*/
	{
//		int num = v[5];
	}break;
	default:
		break;
	}
	return 0;
}

std::vector<int> PTZControl::ParseControlStr(const std::string& strCMD)
{
	std::vector<int> vs;
	size_t idx;
	for (size_t i = 0; i < strCMD.size(); i += 2)
	{
		auto b = strCMD.substr(i, 2);
		vs.push_back(std::stoi(b, &idx, 16));
	}
	return vs;
}

std::string PTZControl::QueryPreset(const std::string& strSN)
{
	std::string xml = "<?xml version=\"1.0\"?>\n<Response>\r\n<CmdType>PresetQuery</CmdType>\r\n";
	xml += "<SN>" + strSN + "</SN>\r\n";
	xml += "<DeviceID>" + m_channelID + "</DeviceID>\r\n";
	xml += "<PresetList Num=\"1\">";
	xml += "<Item><PresetID>1</PresetID>\r\n<PresetName>1</PresetName></Item>\r\n";
	xml += "</PresetList>\r\n";
	xml += "</Response>\r\n";
	return xml;
}
