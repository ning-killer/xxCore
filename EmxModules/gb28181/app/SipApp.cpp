#include "SipApp.h"
#include "SipUAC.h"
#include <string>

//message request
pj_bool_t app_on_rx_request(pjsip_rx_data* rdata)
{
    if (rdata->msg_info.msg->line.req.method.id == PJSIP_REGISTER_METHOD)
    {
        SipUAC::Instance().OnRegister(rdata);
    }
    else if (rdata->msg_info.msg->line.req.method.id == PJSIP_OTHER_METHOD)
    {
        if (pj_strcmp2(&rdata->msg_info.msg->line.req.method.name, "MESSAGE") == 0)
        {
            SipUAC::Instance().OnMessage(rdata);
        }
        else
        {

        }
    }
    
    return PJ_TRUE;
}
//message response
pj_bool_t app_on_rx_response(pjsip_rx_data* rdata) {
    if (pj_strcmp2(&rdata->msg_info.cseq->method.name, "MESSAGE") == 0) {
        SipUAC::Instance().OnMessageResponse(rdata);
    }

    return PJ_TRUE;
}

//invite
pj_bool_t inv_on_rx_request(pjsip_rx_data* rdata) {
    auto dlg = pjsip_rdata_get_dlg(rdata);
    if (!dlg && rdata->msg_info.msg->line.req.method.id != PJSIP_INVITE_METHOD)
        return PJ_FALSE;

    if (rdata->msg_info.msg->line.req.method.id == PJSIP_BYE_METHOD) {
        SipUAC::Instance().OnCallBye(rdata);
    }
    else if (pj_strcmp2(&rdata->msg_info.msg->line.req.method.name, "INFO") == 0) {
        SipUAC::Instance().OnCallInfo(rdata);
    }
    else if (pj_strcmp2(&rdata->msg_info.msg->line.req.method.name, "MESSAGE") == 0)
        SipUAC::Instance().OnCallMessage(rdata);
    else if (rdata->msg_info.msg->line.req.method.id == PJSIP_INVITE_METHOD || !dlg) {
        auto dlg = SipUAC::Instance().CreateUasDlg(rdata);
        if (dlg)
        {
            SipUAC::Instance().OnCallInvite(rdata);
        }
        else
            return PJ_FALSE;
    }
    else if (rdata->msg_info.msg->line.req.method.id == PJSIP_ACK_METHOD)
    {
        printf("2.ack\n");
        SipUAC::Instance().OnCallAck2(rdata);
    }
    return PJ_TRUE;
}

pj_bool_t inv_on_rx_response(pjsip_rx_data* rdata)
{
    auto dlg = pjsip_rdata_get_dlg(rdata);
    if (dlg)
    {
        auto tsx = pjsip_rdata_get_tsx(rdata);
        if (tsx && tsx->method.id == PJSIP_INVITE_METHOD)           
        {
            if (tsx->status_code >= 100 && tsx->status_code < 200)
                SipUAC::Instance().OnCallRing(tsx, rdata);
            else if (tsx->status_code >= 200)
                SipUAC::Instance().OnCallAnswer(tsx->status_code, std::string(tsx->status_text.ptr, tsx->status_text.slen), rdata);
        }
        return PJ_TRUE;
    }
    else
        printf("not found dlg\n");
    return PJ_FALSE;
}

void inv_on_tx_statue(pjsip_transaction* tsx, pjsip_event* event)
{
    switch (tsx->state)
    {
    case PJSIP_TSX_STATE_NULL:
        break;
    case PJSIP_TSX_STATE_TRYING:
    {
        auto dlg = pjsip_tsx_get_dlg(tsx);
        if (dlg)
        {
            if (pj_strcmp2(&tsx->method.name, "INFO") == 0 && (tsx->role == PJSIP_ROLE_UAS))
                SipUAC::Instance().SendStatefulResponse(dlg, event->body.rx_msg.rdata);
            else if ((tsx->method.id == PJSIP_BYE_METHOD) && tsx->role == PJSIP_ROLE_UAS)
                SipUAC::Instance().SendStatefulResponse(dlg, event->body.rx_msg.rdata);
            else if (pj_strcmp2(&tsx->method.name, "MESSAGE") == 0 && (tsx->role == PJSIP_ROLE_UAS))
                SipUAC::Instance().SendStatefulResponse(dlg, event->body.rx_msg.rdata);
        }
    }break;
    case PJSIP_TSX_STATE_CALLING:
        //printf("tsx calling %d\n", tsx->status_code);
        break;
    case PJSIP_TSX_STATE_PROCEEDING:
        //printf("tsx proceeding %d\n", tsx->status_code);
        break;
    case PJSIP_TSX_STATE_COMPLETED: {
        //printf("tsx completed %d\n", tsx->status_code);
    } break;
    case PJSIP_TSX_STATE_CONFIRMED:
    {
        if (tsx->method.id == PJSIP_INVITE_METHOD)
            SipUAC::Instance().OnCallAck(event->body.rx_msg.rdata);
    }break;
    case PJSIP_TSX_STATE_TERMINATED:
        if ((tsx->method.id == PJSIP_INVITE_METHOD && tsx->status_code >= 400)
            || tsx->method.id == PJSIP_BYE_METHOD) {
            auto dlg = pjsip_tsx_get_dlg(tsx);
            if (dlg && dlg->sess_count > 1)
                pjsip_dlg_dec_session(dlg, &inv_mod);
        }break;
    case PJSIP_TSX_STATE_DESTROYED:
    default:
        break;
    }
}

pjsip_module app_mod = 
{
    NULL, NULL,		    /* prev, next.			*/
    { (char*)"app-mod", 7 },	    /* Name.				*/
    -1,			    /* Id				*/
    PJSIP_MOD_PRIORITY_APPLICATION,	/* Priority			*/
    NULL,			    /* load()				*/
    NULL,			    /* start()				*/
    NULL,			    /* stop()				*/
    NULL,			    /* unload()				*/
    &app_on_rx_request,   /* on_rx_request()			*/
    &app_on_rx_response,  /* on_rx_response()			*/
    NULL,			    /* on_tx_request.			*/
    NULL,			    /* on_tx_response()			*/
    NULL,			    /* on_tsx_state()			*/
};

pjsip_module inv_mod = {
    NULL, NULL,		    /* prev, next.			*/
    { (char*)"inv-mod", 3 },	    /* Name.				*/
    2,			    /* Id				*/
    PJSIP_MOD_PRIORITY_DIALOG_USAGE,	/* Priority			*/
    NULL,			    /* load()				*/
    NULL,			    /* start()				*/
    NULL,			    /* stop()				*/
    NULL,			    /* unload()				*/
    &inv_on_rx_request,   /* on_rx_request()			*/
    &inv_on_rx_response,  /* on_rx_response()			*/
    NULL,			    /* on_tx_request.			*/
    NULL,			    /* on_tx_response()			*/
    &inv_on_tx_statue,
};