#include "SipUAC.h"
#include <pjlib-util.h>
#include "DeviceConfig.h"
#include "SipApp.h"
#include "EmxCore.hpp"
#include "RegClient.h"
#include <sys/prctl.h>

static int worker_thread(void* arg)
{
    SipUAC* s = (SipUAC*)arg;
    return s->EventRun();
}

static int heart_thread(void* arg)
{
    SipUAC* s = (SipUAC*)arg;
    return s->HeartbeatTask();
}

SipUAC::SipUAC()
{
}

SipUAC::~SipUAC()
{
    _quitFlag = true;
    for (int i = 0; i < 4; i++)
        pj_thread_join(_pj_thr[i]);

    pjsip_transport_shutdown(_transport);
    pjsip_endpt_destroy(_pj_endpt);
    pj_caching_pool_destroy(&_cache_pool);
    
    pj_shutdown();
}

SipUAC& SipUAC::Instance()
{
    static std::shared_ptr<SipUAC> g_instance(new SipUAC);
    static SipUAC& g_instance_ref = *g_instance;
    return g_instance_ref;
}

int SipUAC::Init()
{
    if (access("debug.d", F_OK) != 0)
        pj_log_set_level(0);
    int rc;
    rc = pj_init();
    if (rc != PJ_SUCCESS)
        return 1;
    hostName = pj_gethostname();

    InitRandSeed();

    rc = pjlib_util_init();
    if (rc != PJ_SUCCESS)
        return 1;
    
    pj_caching_pool_init(&_cache_pool, NULL, 0);
    _pool = pj_pool_create(&_cache_pool.factory, "app", 15 * 1024, 2 * 1024, NULL);
    _contact_pool = pj_pool_create(&_cache_pool.factory, "contact", 512, 512, NULL);
    rc = pjsip_endpt_create(&_cache_pool.factory, hostName->ptr, &_pj_endpt);

    rc = pjsip_tsx_layer_init_module(_pj_endpt);
    if (rc != PJ_SUCCESS)
        return 1;
    pjsip_cfg()->endpt.disable_tcp_switch = 1;

    pj_sockaddr_in pj_addr; 
    pj_sockaddr_in_init(&pj_addr, NULL, 0);
    rc = pjsip_udp_transport_start(_pj_endpt, &pj_addr, NULL, 1, &_transport);
    if(rc != PJ_SUCCESS)
    {
        emxloge("udp transport %d start failed\n", 0);
        return 1;
    }
    emxlogi("sip udp transport %s:%d start succeed\n", _localIP, _transport->local_name.port);

    pjsip_ua_init_module(_pj_endpt, NULL);

    rc = pjsip_endpt_register_module(_pj_endpt, &inv_mod);
    rc = pjsip_endpt_register_module(_pj_endpt, &app_mod);
    
    for (int i = 0; i < 2; i++)
    {
        char tmp[16];
        snprintf(tmp, sizeof tmp, "w%d", i);
        rc = pj_thread_create(_pool, tmp, &worker_thread, this, 0, 0, &_pj_thr[i]);
    }
    NormalInit();

    InitRegisterClient();

    pj_thread_create(_pool, "hb", &heart_thread, this, 0, 0, &_pj_thr[2]);

	return 0;
}

void SipUAC::InitRandSeed()
{
    pj_sockaddr addr;
    pj_uint32_t pid;
    pj_time_val t;
    unsigned seed = 0;

    seed = pj_hash_calc(seed, hostName->ptr, (unsigned int)hostName->slen);

    /* Add primary IP address */
    if (pj_gethostip(pj_AF_INET(), &addr) == PJ_SUCCESS)
        seed = pj_hash_calc(seed, &addr.ipv4.sin_addr, 4);

    pj_inet_ntop(pj_AF_INET(), &addr.ipv4.sin_addr, _localIP, sizeof _localIP);

    /* Get timeofday */
    pj_gettimeofday(&t);
    seed = pj_hash_calc(seed, &t, sizeof(t));

    /* Add PID */
    pid = pj_getpid();
    seed = pj_hash_calc(seed, &pid, sizeof(pid));

    /* Init random seed */
    pj_srand(seed);
}

void SipUAC::NormalInit() {
    // pj_strdup
    MediaType = pj_str((char*)"Application");
    hUserAgent = pj_str((char*)"User-Agent");
    vUserAgent = pj_str((char*)"galaxy");

    char tmp[128];
    pj_ansi_snprintf(tmp, sizeof tmp, "<sip:%s@%s>", DeviceConfig::Instance().DeviceID.c_str(),
        DeviceConfig::Instance().ServerDomain.c_str());
    pj_strdup2(_pool, &_from, tmp);

    pj_ansi_snprintf(tmp, sizeof tmp, "<sip:%s@%s>", DeviceConfig::Instance().ServerID.c_str(), DeviceConfig::Instance().ServerDomain.c_str());
    pj_strdup2(_pool, &_to, tmp);

    pj_ansi_snprintf(tmp, sizeof tmp, "<sip:%s@%s:%d>", DeviceConfig::Instance().DeviceID.c_str(), _localIP,
        _transport->local_name.port);
    pj_strdup2(_contact_pool, &_contact, tmp);

    _contact_hdr = pjsip_contact_hdr_create(_contact_pool);
    _contact_hdr->uri = pjsip_parse_uri(_contact_pool, _contact.ptr, _contact.slen, 0);

    _messageMethod.id = PJSIP_OTHER_METHOD;
    _messageMethod.name = pj_str((char*)"MESSAGE");

    pj_ansi_snprintf(_reg_addr, sizeof _reg_addr, "sip:%s@%s:%d", DeviceConfig::Instance().ServerID.c_str(),
        DeviceConfig::Instance().ServerIP.c_str(), DeviceConfig::Instance().ServerPort);
}

pj_status_t SipUAC::UpdateContact(pjsip_rx_data* rdata)
{
    const pj_str_t STR_CONTACT = { (char*)"Contact", 7 };
    pjsip_via_hdr* via;
    pj_pool_t* pool;
    pjsip_sip_uri* uri;
    pj_sockaddr contact_addr;
    pj_sockaddr recv_addr = { {0} };
    pj_status_t status;
    pj_bool_t matched;
    int rport;
    const pj_str_t* via_addr;
    auto tp = rdata->tp_info.transport;

    /* Get the received and rport info */
    via = rdata->msg_info.via;
    if (via->rport_param < 1) {
        /* Remote doesn't support rport */
        rport = via->sent_by.port;
        if (rport == 0) {
            pjsip_transport_type_e tp_type;
            tp_type = (pjsip_transport_type_e)tp->key.type;
            rport = pjsip_transport_get_default_port_for_type(tp_type);
        }
    }
    else
        rport = via->rport_param;

    if (via->recvd_param.slen != 0)
        via_addr = &via->recvd_param;
    else
        via_addr = &via->sent_by.host;

    pool = pj_pool_create(&_cache_pool.factory, "tmp", 512, 512, NULL);
    auto contact_hdr = (pjsip_contact_hdr*)pjsip_parse_hdr(pool, &STR_CONTACT, _contact.ptr, _contact.slen, NULL);
    pj_assert(contact_hdr != NULL);
    uri = (pjsip_sip_uri*)contact_hdr->uri;
    pj_assert(uri != NULL);
    uri = (pjsip_sip_uri*)pjsip_uri_get_uri(uri);

    if (uri->port == 0) {
        pjsip_transport_type_e tp_type;
        tp_type = (pjsip_transport_type_e)tp->key.type;
        uri->port = pjsip_transport_get_default_port_for_type(tp_type);
    }

    /* Convert IP address strings into sockaddr for comparison.
     * (http://trac.pjsip.org/repos/ticket/863)
     */
    status = pj_sockaddr_parse(pj_AF_UNSPEC(), 0, &uri->host, &contact_addr);
    if (status == PJ_SUCCESS)
        status = pj_sockaddr_parse(pj_AF_UNSPEC(), 0, via_addr, &recv_addr);
    if (status == PJ_SUCCESS) {
        matched = (uri->port == rport && pj_sockaddr_cmp(&contact_addr, &recv_addr) == 0);
    }
    else {
        /* Compare the addresses as string, as before */
        matched = (uri->port == rport && pj_stricmp(&uri->host, via_addr) == 0);
    }

    if (matched) {
        /* Address doesn't change */
        pj_pool_release(pool);
        return PJ_FALSE;
    }

    pj_pool_release(_contact_pool);
    _contact_pool = pj_pool_create(&_cache_pool.factory, "contact", 512, 512, NULL);

    char aa[256];
    pj_ansi_snprintf(aa, sizeof aa, "<sip:%s@%.*s:%d>", DeviceConfig::Instance().DeviceID.c_str(), (int)via_addr->slen, via_addr->ptr, rport);
    pj_strdup2(_contact_pool, &_contact, aa);

    emxlogi("out addr %.*s %d\n", (int)via_addr->slen, via_addr->ptr, rport);

    _contact_hdr = pjsip_contact_hdr_create(_contact_pool);
    _contact_hdr->uri = pjsip_parse_uri(_contact_pool, _contact.ptr, _contact.slen, 0);

    pj_pool_release(pool);
    return PJ_TRUE;
}

pjsip_date_hdr* SipUAC::CreateDateHdr(pj_pool_t* pool)
{
    pj_str_t hDate;
    pj_str_t vDate;

    pj_time_val now;
    pj_parsed_time pt;
    pj_gettimeofday(&now);
    pj_time_decode(&now, &pt);

    char strDate[64] = { 0 };
    snprintf(strDate, 64, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", pt.year, pt.mon + 1, pt.day,
        pt.hour, pt.min, pt.sec, pt.msec);

    hDate = pj_str((char*)"Date");
    vDate = pj_str(strDate);

    return pjsip_date_hdr_create(pool, &hDate, &vDate);
}

void SipUAC::AddUserAgentHdr(pjsip_tx_data* tdata) {
    pjsip_hdr *h;
    h = (pjsip_hdr *) pjsip_user_agent_hdr_create(tdata->pool, &hUserAgent, &vUserAgent);
    if (h)
        pjsip_msg_add_hdr(tdata->msg, h);
    else
        emxloge("create user agent header failed\n");
}

int SipUAC::OnRegister(pjsip_rx_data* rdata)
{   
    //TODO
    return 0;
}

int SipUAC::OnMessage(pjsip_rx_data* rdata)
{
    return _sMsg.OnRequest(rdata);
}

int SipUAC::OnMessageResponse(pjsip_rx_data* rdata)
{
    auto cidHdr = PJSIP_MSG_CID_HDR(rdata->msg_info.msg);
    if (cidHdr)
    {
        std::string cid(cidHdr->id.ptr, cidHdr->id.slen);
        std::lock_guard<std::mutex> lck(_sLock);
        auto it = scp.find(cid);
        if (it != scp.end())
        {
            it->second->flag = true;
            it->second->SC = rdata->msg_info.msg->line.status.code;
            it->second->Cond->notify_one();
            scp.erase(it);
        }
    }
    return 0;
}

int SipUAC::OnCallRing(pjsip_transaction* invTsx, pjsip_rx_data* rdata)
{
    return _sCall.OnCallRing(invTsx, rdata);
}

int SipUAC::OnCallAnswer(int sc, std::string txt, pjsip_rx_data* rdata) {
    return _sCall.OnCallAnswer(sc, txt, rdata);
}

int SipUAC::OnCallInvite(pjsip_rx_data* rdata) {
    return _sCall.OnCallInvite(rdata);
}

int SipUAC::OnCallAck(pjsip_rx_data* rdata)
{
    return _sCall.OnCallAck(rdata);
}

int SipUAC::OnCallAck2(pjsip_rx_data* rdata) {
    return _sCall.OnCallAck2(rdata);
}

int SipUAC::OnCallInfo(pjsip_rx_data* rdata) {
    return _sCall.OnCallInfo(rdata);
}

int SipUAC::OnCallMessage(pjsip_rx_data* rdata) {
    return _sCall.OnCallMessage(rdata);
}

int SipUAC::OnCallBye(pjsip_rx_data* rdata) {
    return _sCall.OnCallBye(rdata);
}

int SipUAC::SendStatelessResponse(pjsip_rx_data* rdata, pjsip_tx_data* tdata)
{
    return pjsip_endpt_send_response2(_pj_endpt, rdata, tdata, NULL, NULL);
}

int SipUAC::SendStatelessResponse(pjsip_rx_data* rdata, int sc)
{
    pjsip_tx_data* tdata;
    pjsip_endpt_create_response(_pj_endpt, rdata, sc, NULL, &tdata);
    AddUserAgentHdr(tdata);
    return SendStatelessResponse(rdata, tdata);
}

pjsip_dialog* SipUAC::CreateUacDlg(const char* target, const char* to) {
    pjsip_dialog *dlg = NULL;
    pj_str_t targetUri = pj_str((char *) target);
    pj_str_t toStr = pj_str((char *) to);
    pj_status_t status;
    //if (strstr(target, "tcp"))
    //    status = pjsip_dlg_create_uac(pjsip_ua_instance(), &_from, &_contact2, &toStr, &targetUri, &dlg);
    //else
        status = pjsip_dlg_create_uac(pjsip_ua_instance(), &_from, &_contact, &toStr, &targetUri, &dlg);

    if (status != PJ_SUCCESS) {
        emxloge("create dlg uac failed\n");
        return NULL;
    }
    return dlg;
}

pjsip_dialog* SipUAC::CreateUasDlg(pjsip_rx_data* rdata) {
    pjsip_dialog* dlg = NULL;
    pjsip_dlg_create_uas_and_inc_lock(pjsip_ua_instance(), rdata, &_contact, &dlg);
    if (dlg)
    {
        pjsip_dlg_add_usage(dlg, &inv_mod, NULL);
        pjsip_dlg_inc_session(dlg, &inv_mod);
        pjsip_dlg_dec_lock(dlg);
        pjsip_tx_data* tdata;
        pj_status_t status;
       
        status = pjsip_dlg_create_response(dlg, rdata, 100, NULL, &tdata);
        if (status != PJ_SUCCESS)
        {
            pjsip_dlg_dec_lock(dlg);
            return NULL;
        }
       
        status = pjsip_dlg_send_response(dlg, pjsip_rdata_get_tsx(rdata), tdata);
    }
    return dlg;
}

int SipUAC::SendInvite(pjsip_dialog* dlg, const char* body, const char* subject)
{
    pj_status_t status;

    pjsip_dlg_inc_lock(dlg);
    status = pjsip_dlg_add_usage(dlg, &inv_mod, NULL);
    pjsip_dlg_inc_session(dlg, &inv_mod);

    pjsip_tx_data* tdata;
    status = pjsip_dlg_create_request(dlg, pjsip_get_invite_method(), -1, &tdata);
    AddUserAgentHdr(tdata);
    if (body)
    {
        pj_str_t STR_SDP = { (char*)"sdp", 3 };
        pj_str_t text = pj_str((char*)body);
        tdata->msg->body = pjsip_msg_body_create(tdata->pool, &MediaType, &STR_SDP, &text);
    }

    if (subject)
    {
        pj_str_t hSubject;
        pj_str_t vSubject;
        pj_strdup2(tdata->pool, &hSubject, "Subject");
        pj_strdup2(tdata->pool, &vSubject, subject);
        auto subHdr = pjsip_subject_hdr_create(tdata->pool, &hSubject, &vSubject);
        pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)subHdr);
    }
    pjsip_dlg_dec_lock(dlg);

    status = pjsip_dlg_send_request(dlg, tdata, -1, NULL);
    if (status != PJ_SUCCESS)
    {
        emxloge("send invite request failed\n");
        return 1;
    }
    return 0;
}

int SipUAC::SendCallAnswer(pjsip_dialog* dlg, pjsip_transaction* invTsx, int sc, const char* sdp)
{
    pjsip_tx_data* tdata = invTsx->last_tx;

    pjsip_dlg_inc_lock(dlg);
    pjsip_dlg_modify_response(dlg, tdata, sc, NULL);
    if (sdp)
    {
        pj_str_t text = pj_str((char*)sdp);
        pj_str_t STR_SDP = { (char*)"sdp", 3 };
        tdata->msg->body = pjsip_msg_body_create(tdata->pool, &MediaType, &STR_SDP, &text);
    }
    pjsip_dlg_dec_lock(dlg);

    return pjsip_dlg_send_response(dlg, invTsx, tdata);
}

int SipUAC::SendCallMessage(pjsip_dialog* dlg, const char* m, int cseq, const char* body)
{
    pj_status_t status = PJ_SUCCESS;
    pjsip_dlg_inc_lock(dlg);
    pjsip_tx_data* tdata = NULL;
    if (strcasecmp(m, "BYE") == 0)
    {
        emxlogi("dlg %p call bye\n", dlg);
        status = pjsip_dlg_create_request(dlg, pjsip_get_bye_method(), cseq, &tdata);
    }
    else if (strcasecmp(m, "ACK") == 0)
        status = pjsip_dlg_create_request(dlg, pjsip_get_ack_method(), cseq, &tdata);
    else if (strcasecmp(m, "INFO") == 0)
    {
        pjsip_method method;
        method.id = PJSIP_OTHER_METHOD;
        method.name = pj_str((char*)"INFO");
        status = pjsip_dlg_create_request(dlg, &method, cseq, &tdata);
        if (body)
        {
            auto subType = pj_str((char*)"MANSRTSP");
            auto text = pj_str((char*)body);
            tdata->msg->body = pjsip_msg_body_create(tdata->pool, &MediaType, &subType, &text);
        }
    }
    else if (strcasecmp(m, "MESSAGE") == 0)
    {
        pjsip_method method;
        method.id = PJSIP_OTHER_METHOD;
        method.name = pj_str((char*)"MESSAGE");
        status = pjsip_dlg_create_request(dlg, &method, cseq, &tdata);
        if (body)
        {
            auto subType = pj_str((char*)"MANSCDP+xml");
            auto text = pj_str((char*)body);
            tdata->msg->body = pjsip_msg_body_create(tdata->pool, &MediaType, &subType, &text);
        }
    }

    if (status != PJ_SUCCESS)
    {
        pjsip_dlg_dec_lock(dlg);
        return 1;
    }

    pjsip_dlg_dec_lock(dlg);
    AddUserAgentHdr(tdata);
    status = pjsip_dlg_send_request(dlg, tdata, -1, NULL);
    if (status != PJ_SUCCESS)
        emxloge("send %s failed[%d]\n", m, status);

    return 0;
}

int SipUAC::SendStatefulResponse(pjsip_dialog* dlg, pjsip_rx_data* rdata, int st)
{
    pjsip_dlg_inc_lock(dlg);
    pjsip_tx_data* tdata;
    auto ss = pjsip_dlg_create_response(dlg, rdata, st, NULL, &tdata);
    if (ss == PJ_SUCCESS)
    {
        AddUserAgentHdr(tdata);
        ss = pjsip_dlg_send_response(dlg, pjsip_rdata_get_tsx(rdata), tdata);
    }
    pjsip_dlg_dec_lock(dlg);
    return 0;
}

int SipUAC::SendMessageRequest(const char* body, std::shared_ptr<SipRespCheck> sc)
{
    pj_status_t status;

    //pj_str_t strTo = pj_str((char*)to);
    pj_str_t strTarget = pj_str(_regc->_reg_addr);

    pjsip_tx_data* tdata;
    if (body)
    {
        pj_str_t strBody;
        strBody.ptr = (char*)body;
        strBody.slen = strlen(body);
        status = pjsip_endpt_create_request(_pj_endpt, &_messageMethod, &strTarget, &_from, &_to, NULL, NULL, -1, &strBody, &tdata);
        tdata->msg->body->content_type.type = MediaType;
        tdata->msg->body->content_type.subtype = pj_str((char*)"MANSCDP+xml");
    }
    else
        status = pjsip_endpt_create_request(_pj_endpt, &_messageMethod, &strTarget, &_from, &_to, NULL, NULL, -1, NULL, &tdata);

    if (status != PJ_SUCCESS)
        return 1;

    auto cidHdr = PJSIP_MSG_CID_HDR(tdata->msg);
    if (sc)
    {
        std::string cid(cidHdr->id.ptr, cidHdr->id.slen);
        std::lock_guard<std::mutex> lck(_sLock);
        scp.insert({ cid, sc });
    }

    AddUserAgentHdr(tdata);

    emxloge("%s ===>> %s\n", _regc->_reg_addr, body);

    status = pjsip_endpt_send_request_stateless(_pj_endpt, tdata, NULL, NULL);
    //status = pjsip_endpt_send_request(_pj_endpt, tdata, 10 * 1000, NULL, NULL);
    if (status != PJ_SUCCESS)
    {
        emxloge("send stateless message failed[%d]\n", status);
        pjsip_tx_data_dec_ref(tdata);
    }

    return 0;
}

void SipUAC::PushAsyncCommand(std::function<void()>&& f)
{
    if (pj_thread_is_registered())
    {
        f();
        return;
    }
    std::lock_guard <std::mutex> lck(_sipCmdLock);
    _sipCmdList.emplace_back(std::move(f));
}

void SipUAC::InitRegisterClient()
{
    _regc.reset(new RegClient, [this](RegClient* obj)
    {
        pjsip_auth_clt_deinit(&obj->auth_sess);
        pjsip_endpt_release_pool(_pj_endpt, obj->_reg_pool);
    });
    _regc->_reg_pool = pjsip_endpt_create_pool(_pj_endpt, "reg", 1024, 1024);
    pjsip_auth_clt_init(&_regc->auth_sess, _pj_endpt, _regc->_reg_pool, 0);

    pjsip_cred_info cred;
    cred.data = pj_str((char*)DeviceConfig::Instance().Password.c_str());
    cred.data_type = 0;
    cred.username = pj_str((char*)DeviceConfig::Instance().DeviceID.c_str());
    cred.scheme = pj_str((char*)"digest");
    cred.realm = pj_str((char*)"*");
    pjsip_auth_clt_set_credentials(&_regc->auth_sess, 1, &cred);

    _regc->_srv_uri = pjsip_parse_uri(_regc->_reg_pool, _reg_addr, strlen(_reg_addr), 0);
    if (_regc->_srv_uri == NULL)
    {
        emxloge("parse uri %s failed\n", _reg_addr);
        return;
    }
    
    _regc->_from_hdr = pjsip_from_hdr_create(_regc->_reg_pool);
    _regc->_from_hdr->uri = pjsip_parse_uri(_regc->_reg_pool, _from.ptr, _from.slen, PJSIP_PARSE_URI_AS_NAMEADDR);

    _regc->_nExpires = DeviceConfig::Instance().Expires;
    _regc->_expires_hdr = pjsip_expires_hdr_create(_regc->_reg_pool, _regc->_nExpires);

    _regc->_cid_hdr = pjsip_cid_hdr_create(_regc->_reg_pool);
    pj_create_unique_string(_regc->_reg_pool, &_regc->_cid_hdr->id);

    _regc->_cseq_hdr = pjsip_cseq_hdr_create(_regc->_reg_pool);
    _regc->_cseq_hdr->cseq = pj_rand() % 0xFFFF;
    pjsip_method_set(&_regc->_cseq_hdr->method, PJSIP_REGISTER_METHOD);

    strcpy(_regc->_reg_addr, _reg_addr);
}

int SipUAC::SendNewRegister()
{
    pj_status_t status;
    _regc->_reg_status = 0;
    _regc->_heartbeatFaildCount = 0;
    _regc->_lastRegisterTime = time(NULL);
    _regc->_lastHeartbeatTime = _regc->_lastRegisterTime;

    pjsip_tx_data* tdata = NULL;
    status = pjsip_endpt_create_request_from_hdr(_pj_endpt,
        pjsip_get_register_method(),
        _regc->_srv_uri,
        _regc->_from_hdr,
        _regc->_from_hdr,
        _contact_hdr,
        _regc->_cid_hdr,
        _regc->_cseq_hdr->cseq++,
        NULL,
        &tdata);

    if (status != PJ_SUCCESS)
        return status;

    AddUserAgentHdr(tdata);

    pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)_regc->_expires_hdr);

    return pjsip_endpt_send_request(_pj_endpt, tdata, 10 * 1000, NULL, &regc_tsx_callback);
}

int SipUAC::OnAuthorized(pjsip_event* e)
{
    pjsip_tx_data* tdata;
    pjsip_transaction* tsx = e->body.tsx_state.tsx;
    pjsip_rx_data* rdata = e->body.tsx_state.src.rdata;

    auto z = pjsip_auth_clt_reinit_req(&_regc->auth_sess,
        rdata,
        tsx->last_tx,
        &tdata);
    if (z != PJ_SUCCESS)
    {
        emxloge("pjsip_auth_clt_reinit_req failed %d\n", z);
        return 1;
    }

    return pjsip_endpt_send_request(_pj_endpt, tdata, 10 * 1000, NULL, &regc_tsx_callback);
}

int SipUAC::OnRegSucceed(pjsip_rx_data* rdata)
{
    auto fromUri = (pjsip_sip_uri*)pjsip_uri_get_uri(rdata->msg_info.from->uri);
    emxlogi("%.*s reg succeed\n", fromUri->user.slen, fromUri->user.ptr);

    _regc->_reg_status = 1;

    return UpdateContact(rdata);
}

int SipUAC::HeartbeatTask()
{
    int SN = 1;
    char tmp[256];
    std::shared_ptr<SipRespCheck> sc(new SipRespCheck);
    SendNewRegister();
    while (!_quitFlag)
    {
        {
            std::unique_lock<std::mutex> lck(_regLock);
            _regCond.wait_for(lck, std::chrono::seconds(5));
        }
        if (_quitFlag)
            break;

        auto now = time(NULL);
        if (_regc->_reg_status == 0)
        {
            if (difftime(now, _regc->_lastRegisterTime) >= 40)
                SendNewRegister();
            continue;
        }

        if (difftime(now, _regc->_lastRegisterTime) >= _regc->_nExpires - 10)
            SendNewRegister();
        if (difftime(now, _regc->_lastHeartbeatTime) >= DeviceConfig::Instance().HeartbeatInterval - 1)
        {
            _regc->_lastHeartbeatTime = now;

            pj_ansi_snprintf(tmp, sizeof tmp, "<?xml version=\"1.0\"?>\n"
                "<Notify><CmdType>Keepalive</CmdType>"
                "<SN>%d</SN>"
                "<DeviceID>%s</DeviceID>\n"
                "<Status>OK</Status>\n</Notify>", SN++, DeviceConfig::Instance().DeviceID.c_str());

            sc->flag = false;
            sc->SC = 0;

            SendMessageRequest(tmp, sc);
            std::unique_lock<std::mutex> lck(_sLock);
            sc->Cond->wait_for(lck, std::chrono::milliseconds(1200));
            if (!sc->flag)
            {
                emxloge("wait heartbeat resp failed\n");
                _regc->_lastHeartbeatTime -= 40;
                _regc->_heartbeatFaildCount++;
                if (_regc->_heartbeatFaildCount >= DeviceConfig::Instance().HeartbeatCount)
                {
                    emxlogi("send new register\n");
                    SendNewRegister();
                }
            }
            else if (sc->SC >= 400)/*response statuscode >= 400*/
            {
                emxlogw("heartbeat response statuscode %d\n", sc->SC);
                SendNewRegister();
            }
            else
                _regc->_heartbeatFaildCount = 0;

        }
    }
    return 0;
}

int SipUAC::EventRun()
{
    prctl(PR_SET_NAME, "pjsip-thr");
    enum { TIMEOUT = 10 };
    while (!_quitFlag) {
        unsigned int count;
        pj_time_val tv;
        pj_status_t status;

        tv.sec = 0;
        tv.msec = 10;
        pj_time_val_normalize(&tv);

        status = pjsip_endpt_handle_events2(_pj_endpt, &tv, &count);

        if (status != PJ_SUCCESS)
        {
            emxloge("pjsip_endpt_handle_events2 %d\n", (int)status);
            //return -status;
        }

        std::list<std::function<void()>> tmpList;
        {
            std::lock_guard <std::mutex> lck(_sipCmdLock);
            tmpList.swap(_sipCmdList);
        }

        for (auto& item : tmpList)
        {
            item();
        }

        if (count < 0)
            pj_thread_sleep(TIMEOUT);
    }
    return 0;
}