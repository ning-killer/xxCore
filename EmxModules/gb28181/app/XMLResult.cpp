#include "XMLResult.h"
#include "tinyxml2.h"

int XMLResult::ParseXml(const std::string& xmlMsg)
{
	tinyxml2::XMLDocument doc;
	auto err = doc.Parse(xmlMsg.c_str());
	if (err != tinyxml2::XML_SUCCESS)
		return 1;
	auto root = doc.RootElement();
	RootName = root->Name();

	for (auto i = root->FirstChildElement(); i; i = i->NextSiblingElement())
	{
		auto nodeName = i->Name();
		if (strstr(nodeName, "CmdType"))
			CmdType = i->GetText();
		else if (strstr(nodeName, "SN"))
			SN = i->IntText();
		else if (strstr(nodeName, "DeviceID"))
			DeviceID = i->GetText();
		else if (strstr(nodeName, "Result"))
			Result = i->GetText();
		else if (strstr(nodeName, "DeviceList"))
		{
			/*Num = atoi(i->Attribute("Num"));
			for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
			{
				if (strstr(j->Name(), "Item"))
				{
					CatalogItem item;
					parse_catalog_xml(j, item);
					items.push_back(item);
				}
			}*/
		}
		else if (strstr(nodeName, "RecordList"))
		{
			/*Num = atoi(i->Attribute("Num"));
			for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
			{
				if (strstr(j->Name(), "Item"))
				{
					RecordItem item;
					parse_record_xml(j, item);
					records.push_back(item);
				}
			}*/
		}
		else if (strcmp(nodeName, "PresetList") == 0)
		{
			/*i->QueryAttribute("Num", &Num);
			for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
			{
				if (strstr(j->Name(), "Item"))
				{
					PresetItem item;
					parse_preset_xml(j, item);
					presets.push_back(item);
				}
			}*/
		}
		else if (strstr(nodeName, "SumNum"))
		{
			//SumNum = atoi(i->GetText());
		}
		else if (strcmp(nodeName, "Info") == 0)
		{
			/*for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
			{
				if (strcmp(j->Name(), "AlarmType") == 0)
					Info.AlarmType = j->GetText();
				else if (strcmp(j->Name(), "AlarmTypeParam") == 0)
				{
					auto etNode = j->FirstChildElement("EventType");
					if (etNode)
						Info.EventType = etNode->GetText();
				}
			}*/
		}
		else if (strcmp(nodeName, "AlarmPriority") == 0)
		{
			//Info.AlarmPriority = i->GetText();
		}
		else if (strcmp(nodeName, "AlarmMethod") == 0)
		{
			//Info.AlarmMethod = i->GetText();
		}
		else if (strcmp(nodeName, "AlarmTime") == 0)
		{
			//Info.AlarmTime = i->GetText();
		}
		else
		{
			//if(i->QueryIntText)
			KV[nodeName] = i->GetText();
		}

	}
	return 0;
}