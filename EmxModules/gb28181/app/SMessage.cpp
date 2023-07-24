#include "SMessage.h"
#include "SipUAC.h"
#include "EmxCore.hpp"
#include <sstream>
#include "PTZControl.h"
#include "CatalogQuery.h"
#include "RecordQuery.h"
#include "XMLResult.h"

int SMessage::OnRequest(pjsip_rx_data* rdata) {
    std::string body;
    if (rdata->msg_info.msg->body)
        body.assign((char*)rdata->msg_info.msg->body->data, rdata->msg_info.msg->body->len);

    int ret = 200;
    XMLResult xmlRes;
   
    if (xmlRes.ParseXml(body))
    {
        emxloge("parse failed %s\n", body.c_str());
        return SipUAC::Instance().SendStatelessResponse(rdata, 400);
    }

    SipUAC::Instance().SendStatelessResponse(rdata, ret);

    if (xmlRes.RootName == "Query")
    {
        if (xmlRes.CmdType == "Catalog")
        {
            OnCatalog(xmlRes);
        }
        else if (xmlRes.CmdType == "RecordInfo")
        {
            OnRecord(xmlRes);
        }
        else if (xmlRes.CmdType == "DeviceStatus")
        {
            //OnDeviceStatus(xmlRes);
        }
        else if (xmlRes.CmdType == "DeviceInfo")
        {
            //OnDeviceStatus(xmlRes);
        }
        else if (xmlRes.CmdType == "PresetQuery")
        {
            auto strSN = xmlRes.KV["SN"];
            PTZControl ptz(xmlRes.DeviceID);
            auto xml = ptz.QueryPreset(strSN);
            //SipUAC::Instance().SendMessageRequest(xml.c_str());
        }
    }
    else if (xmlRes.RootName == "Control")
    {
        if (xmlRes.CmdType == "DeviceControl")
        {
            auto tc = xmlRes.KV["TeleBoot"];
            if (tc.empty())
            {

            }
            std::string strPTZ = xmlRes.KV["PTZCmd"];
            if (strPTZ.empty())
            {
                PTZControl ptz(xmlRes.DeviceID);
                ptz.ptzGO(strPTZ);
            }

            auto strGuard = xmlRes.KV["GuardCmd"];
            if (strGuard.empty())
            {
                if (strGuard == "SetGuard")
                {
                }
                else if (strGuard == "ReSetGuard")
                {
                }
            }
        }
        else
        {

        }
    }
    else if (xmlRes.RootName == "Notify")
    {
        if (xmlRes.CmdType == "Broadcast")
        {
            OnBroadcast(xmlRes);
        }
    }
    else if (xmlRes.RootName == "Response")
    {
        if (xmlRes.CmdType == "Alarm")
        {
            // std::cout << "alarm response" << std::endl;
        }
    }
    return 0;
}

int SMessage::OnCatalog(XMLResult& xmlRes){
    CatalogQuery ciq;

    auto v = ciq.GetCatalog();
    auto sz = v.size();
    if (sz > 0)
    {
        std::ostringstream os;
        os << "<?xml version=\"1.0\"?>\n";
        os << "<Response>\n<CmdType>Catalog</CmdType>\n";
        os << "<SN>" << xmlRes.SN << "</SN>\n";
        os << "<DeviceID>" << xmlRes.DeviceID << "</DeviceID>\n";
        os << "<SumNum>" << v.size() << "</SumNum>\n";

        auto a = sz % 3;
        auto b = sz / 3 + (a == 0 ? 0 : 1);
        for (size_t i = 0; i < b; ++i) {
            std::ostringstream resp;
            resp << os.str();
            int j = 0;
            if (i + 1 == b)
                j = a;
            else
                j = 3;

            resp << "<DeviceList Num=\"" << j << "\">\n";
            for (int k = 0; k < j; ++k) {
                resp << v.at(i * 3 + k)->ToString();
            }
            resp << "</DeviceList></Response>\n";

            SipUAC::Instance().SendMessageRequest(resp.str().c_str(), nullptr);
        }
    }
    else
    {
        std::ostringstream os;
        os << "<?xml version=\"1.0\"?>\n";
        os << "<Response>\n<CmdType>Catalog</CmdType>\n";
        os << "<SN>" << xmlRes.SN << "</SN>\n";
        os << "<DeviceID>" << xmlRes.DeviceID << "</DeviceID>\n";
        os << "<SumNum>0</SumNum>\n<DeviceList Num =\"0\">\n</DeviceList>\n</Response>\n";
        SipUAC::Instance().SendMessageRequest(os.str().c_str(), nullptr);
    }
    return 200;
}

int SMessage::OnRecord(XMLResult& xmlRes){
    std::string strStart = xmlRes.KV["StartTime"];
    std::string strEnd = xmlRes.KV["EndTime"];
    auto strSN = std::to_string(xmlRes.SN);
    auto strID = xmlRes.DeviceID;

    emxlogi("query record[%s-%s]\n", strStart.c_str(), strEnd.c_str());

    std::string strReqBody =
        "<?xml version=\"1.0\"?>\n"
        "<Response>\n"
        "<CmdType>RecordInfo</CmdType>\n"
        "<SN>" + strSN + "</SN>\n"
        "<DeviceID>" + strID + "</DeviceID>\n";

    RecordQuery recordQuery;
    std::vector<std::shared_ptr<RecordQuery::RecordItem>> v;

    recordQuery.query(strID, strStart, strEnd, 1, v);
    char tmp[64];
    sprintf(tmp, "<SumNum>%lu</SumNum>\n", (unsigned long )v.size());
    strReqBody.append(tmp);

    if (v.size() > 0)
    {
        auto sl = v.size() % 5;
        auto sz = v.size() / 5 + (sl == 0 ? 0 : 1);
        for (size_t i = 0; i < sz; i++)
        {
            if (i + 1 == sz)
            {
                std::string items;
                sprintf(tmp, "<RecordList Num=\"%lu\">\n", (unsigned long)(v.size() - i * 5));
                items.append(strReqBody);
                items.append(tmp);
                for (size_t j = 0; j < v.size() - i * 5; j++)
                {
                    items += "<Item>\n";
                    items += "<DeviceID>" + strID + "</DeviceID>\n";
                    items += "<Name>" + v.at(i * 5 + j)->Name + "</Name>\n";
                    items += "<Address/>\n";
                    items += "<StartTime>" + v.at(i * 5 + j)->StartTime + "</StartTime>\n";
                    items += "<EndTime>" + v.at(i * 5 + j)->EndTime + "</EndTime>\n";
                    items += "<Secrecy>0</Secrecy>\n";
                    items += "<Type>time</Type>\n";
                    items += "<RecorderID/>";
                    items += "</Item>\n";
                }
                items += "</RecordList>";
                items += "</Response>";

                SipUAC::Instance().SendMessageRequest(items.c_str(), nullptr);
            }
            else
            {
                std::string items;
                sprintf(tmp, "<RecordList Num=\"%u\">\n", 5);
                items.append(strReqBody);
                items.append(tmp);
                for (size_t j = 0; j < 5; j++)
                {
                    items += "<Item>\n";
                    items += "<DeviceID>" + strID + "</DeviceID>\n";
                    items += "<Name>" + v.at(i * 5 + j)->Name + "</Name>\n";
                    items += "<Address/>\n";
                    items += "<StartTime>" + v.at(i * 5 + j)->StartTime + "</StartTime>\n";
                    items += "<EndTime>" + v.at(i * 5 + j)->EndTime + "</EndTime>\n";
                    items += "<Secrecy>0</Secrecy>\n";
                    items += "<Type>time</Type>\n";
                    items += "<RecorderID/>";
                    items += "</Item>\n";
                }
                items += "</RecordList>";
                items += "</Response>";

                SipUAC::Instance().SendMessageRequest(items.c_str(), nullptr);
            }
        }
    }
    else
    {
        std::string items;
        items.append(strReqBody);
        items += "<RecordList Num=\"0\"></RecordList>\n</Response>\n";
        emxlogi("%s\n", items.c_str());

        SipUAC::Instance().SendMessageRequest(items.c_str(), nullptr);
    }
    return 200;
}

int SMessage::OnBroadcast(XMLResult& xmlRes)
{
    auto strSourceID = xmlRes.KV["SourceID"];
    auto strID = xmlRes.KV["TargetID"];

    std::ostringstream os;
    os << "<?xml version=\"1.0\"?>\n<Response>\n<CmdType>Broadcast</CmdType>\n";
    os << "<SN>" << xmlRes.SN << "</SN>\r\n";
    os << "<DeviceID>" << strID << "</DeviceID>\n";
    os << "<Result>OK</Result>\n</Response>\n";
    SipUAC::Instance().SendMessageRequest(os.str().c_str(), nullptr);

    /*std::string strTarget;
    auto strTo = fmt::format("<sip:{}@{}>", _channelId, ServerConfig::Instance().Domain);
    if (Protocol == "tcp")
        strTarget = fmt::format("sip:{}@{}:{};transport=tcp", _channelId, DevHost, DevPort);
    else
        strTarget = fmt::format("sip:{}@{}:{}", _channelId, DevHost, DevPort);*/
    return 0;
}