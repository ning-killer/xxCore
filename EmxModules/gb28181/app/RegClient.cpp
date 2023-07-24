#include "RegClient.h"
#include "SipUAC.h"
#include "EmxCore.hpp"

//pj_bool_t is_private_ip(const pj_str_t* addr)
//{
//    const pj_str_t private_net[] =
//    {
//    { "10.", 3 },
//    { "127.", 4 },
//    { "172.16.", 7 }, { "172.17.", 7 }, { "172.18.", 7 }, { "172.19.", 7 },
//        { "172.20.", 7 }, { "172.21.", 7 }, { "172.22.", 7 }, { "172.23.", 7 },
//        { "172.24.", 7 }, { "172.25.", 7 }, { "172.26.", 7 }, { "172.27.", 7 },
//        { "172.28.", 7 }, { "172.29.", 7 }, { "172.30.", 7 }, { "172.31.", 7 },
//    { "192.168.", 8 }
//    };
//    unsigned i;
//
//    for (i = 0; i < PJ_ARRAY_SIZE(private_net); ++i) {
//        if (pj_strncmp(addr, &private_net[i], private_net[i].slen) == 0)
//            return PJ_TRUE;
//    }
//
//    return PJ_FALSE;
//}

void regc_tsx_callback(void* token, pjsip_event* event)
{
    pjsip_transaction* tsx = event->body.tsx_state.tsx;
    if (tsx->method.id == PJSIP_REGISTER_METHOD)
    {
        emxlogi("method register, code %d,event type %d\n", tsx->status_code, (int)event->body.tsx_state.type);
        if ((tsx->status_code == PJSIP_SC_UNAUTHORIZED || PJSIP_SC_PROXY_AUTHENTICATION_REQUIRED == tsx->status_code) 
            && event->body.tsx_state.type == PJSIP_EVENT_RX_MSG)
        {
            SipUAC::Instance().OnAuthorized(event);
        }
        else if (tsx->status_code == PJSIP_SC_OK)
        {
            auto rdata = event->body.tsx_state.src.rdata;
            SipUAC::Instance().OnRegSucceed(rdata);
        }
        else if (tsx->status_code == PJSIP_SC_MOVED_TEMPORARILY)
        {
            //auto rdata = event->body.tsx_state.src.rdata;
           //SipUAC::Instance().OnRegMoved(rdata);
        }
        else if (tsx->status_code == PJSIP_SC_REQUEST_TIMEOUT)
        {
            printf("req timeout\n");
        }
        else
        {
            printf("req %d\n", tsx->status_code);
        }
    }
    else if (pj_strcmp2(&tsx->method.name, "MESSAGE")==0)
    {

    }
}

RegClient::~RegClient()
{
}
