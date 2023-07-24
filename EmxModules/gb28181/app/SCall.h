#pragma once
#include <pjsip.h>
#include <string>
#include <map>
#include <mutex>

class SCall {
public:
    SCall()= default;
    ~SCall();

    int OnCallRing(pjsip_transaction* invTsx, pjsip_rx_data* rdata);

    int OnCallAnswer(int sc, std::string txt, pjsip_rx_data* rdata);

    int OnCallInvite(pjsip_rx_data* rdata);

    int OnCallAck(pjsip_rx_data* rdata);

    int OnCallAck2(pjsip_rx_data* rdata);

    int OnCallInfo(pjsip_rx_data* rdata);

    int OnCallMessage(pjsip_rx_data* rdata);

    int OnCallBye(pjsip_rx_data* rdata);
private:
    std::mutex _tsxLock;
    std::map<std::string, pjsip_transaction*> _tsxMap;
};
