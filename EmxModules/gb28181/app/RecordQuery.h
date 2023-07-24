#pragma once
#include <string>
#include <vector>
#include <memory>

class RecordQuery
{
public:
	RecordQuery();
	~RecordQuery();

	struct RecordItem
	{
		std::string DeviceID;
		std::string Name;
		std::string FilePath;
		std::string Address;
		std::string StartTime;
		std::string EndTime;
		int Secrecy;
		std::string Type;// 录像产生类型(可选) time或alarm或manual
		std::string RecorderID;
	};

	int query(const std::string& channelID, 
		const std::string& st, 
		const std::string& et, 
		int recLocation,
		std::vector<std::shared_ptr<RecordItem>>& ls);
private:
	time_t gbTimeToClock(const std::string& strTime);
private:
};

