#include "CatalogQuery.h"
#include <sstream>
#include "DeviceConfig.h"

std::string CatalogItem::ToString() {
    std::ostringstream xml;
    xml << "<Item>\n";
    xml << "<DeviceID>" << DeviceID << "</DeviceID>\n";
    xml << "<Name>" << Name << "</Name>\n";
    xml << "<Manufacturer>" << Manufacturer << "</Manufacturer>\n";
    xml << "<Model>" << Model << "</Model>\n";
    xml << "<Owner>" << Owner << "</Owner>\n";
    xml << "<CivilCode>" << CivilCode << "</CivilCode>\n";
    xml << "<Address>" << Address << "</Address>\n";
    xml << "<Parental>" << Parental << "</Parental>\n";
    xml << "<ParentID>" << ParentID << "</ParentID>\n";
    xml << "<SafetyWay>" << 0 << "</SafetyWay>\n";
    xml << "<RegisterWay>" << RegisterWay << "</RegisterWay>\n";
    xml << "<Certifiable>" << Certifiable << "</Certifiable>\n";
    xml << "<ErrCode>" << ErrCode << "</ErrCode>\n";
    xml << "<Secrecy>" << Secrecy << "</Secrecy>\n";
    xml << "<Status>" << Status << "</Status>\n";
    xml << "</Item>\n";

    return xml.str();
}

std::vector<std::shared_ptr<CatalogItem>> CatalogQuery::GetCatalog()
{
    std::vector<std::shared_ptr<CatalogItem>> rs;
    auto it = DeviceConfig::Instance().Channels.begin();
    while (it != DeviceConfig::Instance().Channels.end())
    {
        std::shared_ptr<CatalogItem> item(new CatalogItem());
        item->DeviceID = *it;
        item->Name = "IPC";
        item->Manufacturer = "ZN";
        item->Model = "ZN-PANO-400";
        item->Owner = "ZN";
        item->CivilCode = "";
        item->Address = "";
        item->Parental = 0;
        item->ParentID = DeviceId;/*IPCΪ�豸ID*/
        item->RegisterWay = 1;
        //item->CertNum = "";
        item->Certifiable = 0;
        item->ErrCode = 400;
        //item->EndTime = "";
        item->Secrecy = 0;
        item->Status = "ON";
        //item->IPAddress = "127.0.0.1";
        //item->Port = 0;
        //item->Password = "12345";

        rs.push_back(item);
        ++it;
    }
    return rs;
}
