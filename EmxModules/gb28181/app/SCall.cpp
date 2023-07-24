#include "SCall.h"
#include "PlayHandler.h"
#include "PlaybackHandler.h"
#include "GBSdp.h"
#include "EmxCore.hpp"
#include "SipUAC.h"

SCall::~SCall()
{}

int SCall::OnCallRing(pjsip_transaction* invTsx, pjsip_rx_data* rdata)
{
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);

    return 0;
}

int SCall::OnCallAnswer(int sc, std::string txt, pjsip_rx_data* rdata)
{
    std::string sdp;
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    if (rdata->msg_info.msg->body)
        sdp.assign((char*)rdata->msg_info.msg->body->data, rdata->msg_info.msg->body->len);

    auto dlg = pjsip_rdata_get_dlg(rdata);
    if (dlg)
    {
        SipUAC::Instance().SendCallMessage(dlg, "ACK", rdata->msg_info.cseq->cseq, NULL);
        SipUAC::Instance().SendCallMessage(dlg, "BYE", -1, NULL);
    }

    return 0;
}

int SCall::OnCallInvite(pjsip_rx_data* rdata)
{
    auto dlg = pjsip_rdata_get_dlg(rdata);
    auto invTsx = pjsip_rdata_get_tsx(rdata);

//    auto fromUri = (pjsip_sip_uri*)pjsip_uri_get_uri(rdata->msg_info.from->uri);
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);

    std::string strSdp;
    if (rdata->msg_info.msg->body)
        strSdp.assign((char*)rdata->msg_info.msg->body->data, rdata->msg_info.msg->body->len);
    else
        return SipUAC::Instance().SendCallAnswer(dlg, invTsx, 400, NULL);

    emxlogi("%s\n", strSdp.c_str());
    GBSdp sdp;
    sdp.parse(strSdp);

    //we need cache invTs
    {
        std::lock_guard<std::mutex> lck(_tsxLock);
        _tsxMap.insert({ cid, invTsx });
    }

//    auto channelId = fmt::format("{:.{}}", fromUri->user.ptr, fromUri->user.slen);

    if (sdp.session_name_ == "Play")
    {
        std::string outSdp;
        std::shared_ptr<PlayHandler> ph(new PlayHandler(cid));
        ph->Init(dlg, sdp, outSdp);
        AddHandler(ph, cid);
  
        return SipUAC::Instance().SendCallAnswer(dlg, invTsx, 200, outSdp.c_str());
    }
    else if (sdp.session_name_ == "Download" || sdp.session_name_ == "Playback")
    {
        std::string outSdp;
        std::shared_ptr<PlaybackHandler> ph(new PlaybackHandler(cid));
        ph->Init(dlg, sdp, outSdp);

        AddHandler(ph, cid);
        return SipUAC::Instance().SendCallAnswer(dlg, invTsx, 200, outSdp.c_str());
    }

    return SipUAC::Instance().SendCallAnswer(dlg, invTsx, 486, NULL);
}

int SCall::OnCallAck2(pjsip_rx_data* rdata)
{
    //on uas recv ack2 ,we need terminate invite tsx
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    {
        std::lock_guard<std::mutex> lck(_tsxLock);
        auto it = _tsxMap.find(cid);
        if (it != _tsxMap.end())
            pjsip_tsx_terminate(it->second, it->second->status_code);
    }
    return OnCallAck(rdata);
}

int SCall::OnCallAck(pjsip_rx_data* rdata){
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    RunHandler(cid);
    return 0;
}

int SCall::OnCallInfo(pjsip_rx_data* rdata) {
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    if(rdata->msg_info.msg->body) {
        std::string str((char*)rdata->msg_info.msg->body->data, rdata->msg_info.msg->body->len);
        PlaybackControlInfo ctrInfo;
        ctrInfo.Unserialize(str);
        if (ctrInfo.Command == "PLAY")
        {
            if (ctrInfo.Scale != 0.f)
            {
                OperationHandler(cid, 0, ctrInfo.Scale);
            }
            else if (ctrInfo.Range == 0.f)
            {
                OperationHandler(cid, 1, 0);
            }
            else
            {
                OperationHandler(cid, 2, ctrInfo.Range);
            }
        }
        else if (ctrInfo.Command == "PAUSE")
        {
            OperationHandler(cid, 1, 0);
        }
    }
    return 0;
}

int SCall::OnCallMessage(pjsip_rx_data* rdata)
{
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    std::string body;
    if (rdata->msg_info.msg->body)
        body.assign((char*)rdata->msg_info.msg->body->data, rdata->msg_info.msg->body->len);

    return 0;
}

int SCall::OnCallBye(pjsip_rx_data* rdata) {
    auto toUri = (pjsip_sip_uri*)pjsip_uri_get_uri(rdata->msg_info.to->uri);
    std::string cid(rdata->msg_info.cid->id.ptr, rdata->msg_info.cid->id.slen);
    emxlogi("%.*s bye with %s\n", toUri->user.slen, toUri->user.ptr, cid.c_str());
   
    RemoveHandler(cid);

    return 0;
}