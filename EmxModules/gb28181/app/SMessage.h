#pragma once
#include "pjsip.h"
#include <string>

class XMLResult;

class SMessage {
public:
    SMessage() = default;
    ~SMessage(){}

    int OnRequest(pjsip_rx_data* rdata);

private:
    //int OnKeepAlive(const std::string& devId, const char* remoteIP, int port, const char* protocol);

    int OnCatalog(XMLResult& xmlRes);

    int OnRecord(XMLResult& xmlRes);

    int OnBroadcast(XMLResult& xmlRes);
    //int OnAlarm(XMLResult& x);

    //int OnPreset(XMLResult& x);
};
