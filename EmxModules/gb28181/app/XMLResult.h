#include <string>
#include <map>
//#include <vector>

class XMLResult
{
public:
	std::string RootName;//Control Query Notify Response
	std::string CmdType;
	int SN;
	std::string DeviceID;
	std::string Result;
	std::map<std::string, std::string> KV;

	//int SumNum = 0;
	//int Num = 0;
	//std::vector<CatalogItem> items;
	//std::vector<RecordItem> records;
	//std::vector<PresetItem> presets;

	//AlarmInfo Info;

	int ParseXml(const std::string& xmlMsg);
};
