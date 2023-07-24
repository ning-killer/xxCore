#include "RecordQuery.h"
#include <time.h>

RecordQuery::RecordQuery()
{
}

RecordQuery::~RecordQuery()
{
}

int RecordQuery::query(const std::string& channelID, const std::string& st, const std::string& et, int recLocation, std::vector<std::shared_ptr<RecordItem>>& ls)
{
	auto beginTime = gbTimeToClock(st);
	auto endTime = gbTimeToClock(et);
	if (beginTime > endTime)
		return 1;

	auto totalTime = endTime - beginTime;

	auto perFilePeriod = 30 * 60;//�ļ���30����һ��
	auto sl = totalTime % perFilePeriod;
	auto sz = totalTime / perFilePeriod + (sl == 0 ? 0 : 1);

	char cTime[64];
	for (unsigned int i = 0; i < (unsigned int)sz; i++)
	{
		std::shared_ptr<RecordItem> item(new RecordItem);
		item->DeviceID = channelID;
		item->Name = "rec";
		item->FilePath = "file_path";
		item->Address = "Address 1";
		item->Secrecy = 0;
		item->Type = "time";
		item->RecorderID = "";

		if (i + 1 == (unsigned int) sz)
		{
			auto a = i * perFilePeriod + beginTime;
#ifdef _WIN32
			tm localeTM;
			localtime_s(&localeTM, &a);
			localeTM.tm_year += 1900;
			localeTM.tm_mon += 1;
			sprintf_s(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", localeTM.tm_year, localeTM.tm_mon, localeTM.tm_mday, localeTM.tm_hour, localeTM.tm_min, localeTM.tm_sec);
			item->StartTime = cTime;
#else
			auto ptm = localtime((time_t*)&a);
			ptm->tm_year += 1900;
			ptm->tm_mon += 1;
			sprintf(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			item->StartTime = cTime;
#endif 	
			item->EndTime = et;
		}
		else
		{
			auto a = i * perFilePeriod + beginTime;
			auto b = (i + 1) * perFilePeriod + beginTime;
#ifdef _WIN32
			tm localeTM;
			localtime_s(&localeTM, &a);
			localeTM.tm_year += 1900;
			localeTM.tm_mon += 1;
			sprintf_s(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", localeTM.tm_year, localeTM.tm_mon, localeTM.tm_mday, localeTM.tm_hour, localeTM.tm_min, localeTM.tm_sec);
			item->StartTime = cTime;

			localtime_s(&localeTM, &b);
			localeTM.tm_year += 1900;
			localeTM.tm_mon += 1;
			sprintf_s(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", localeTM.tm_year, localeTM.tm_mon, localeTM.tm_mday, localeTM.tm_hour, localeTM.tm_min, localeTM.tm_sec);
			item->EndTime = cTime;
#else
			auto localeTM = localtime((time_t*)&a);
			localeTM->tm_year += 1900;
			localeTM->tm_mon += 1;
			sprintf(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", localeTM->tm_year, localeTM->tm_mon, localeTM->tm_mday, localeTM->tm_hour, localeTM->tm_min, localeTM->tm_sec);
			item->StartTime = cTime;

			auto localeTM1 = localtime((time_t*)&b);
			localeTM1->tm_year += 1900;
			localeTM1->tm_mon += 1;
			sprintf(cTime, "%04d-%02d-%02dT%02d:%02d:%02d", localeTM1->tm_year, localeTM1->tm_mon, localeTM1->tm_mday, localeTM1->tm_hour, localeTM1->tm_min, localeTM1->tm_sec);
			item->EndTime = cTime;
#endif 
		}
		ls.push_back(item);
	}
	return 0;
}

time_t RecordQuery::gbTimeToClock(const std::string& strTime)
{
	//2013-09-10T14:00:00
	tm t;
#ifdef _WIN32
	sscanf_s(strTime.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
#else
	sscanf(strTime.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
#endif 
	t.tm_year -= 1900;
	t.tm_mon -= 1;

	return mktime(&t);
}
