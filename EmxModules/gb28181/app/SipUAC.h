#pragma once
#include <pjlib.h>
#include <pjsip.h>
#include <list>
#include <mutex>
#include <memory>
#include <functional>
#include <condition_variable>
#include "SMessage.h"
#include "SCall.h"

class RegClient;

class SipRespCheck
{
public:
	SipRespCheck()
	{
		Cond.reset(new std::condition_variable);
		CreateTime = time(NULL);
	}
	~SipRespCheck() {}
	bool flag = false;
	std::shared_ptr<std::condition_variable> Cond;
	time_t CreateTime;
	int SC = 0;
};

class SipUAC
{
public:
	~SipUAC();

	static SipUAC& Instance();

	int Init();

	int EventRun();

	int OnRegister(pjsip_rx_data* rdata);

	int OnMessage(pjsip_rx_data* rdata);

	int OnMessageResponse(pjsip_rx_data* rdata);

	int OnCallRing(pjsip_transaction* invTsx, pjsip_rx_data* rdata);

	int OnCallAnswer(int sc, std::string txt, pjsip_rx_data* rdata);

	int OnCallInvite(pjsip_rx_data* rdata);

    int OnCallAck(pjsip_rx_data* rdata);

	int OnCallAck2(pjsip_rx_data* rdata);

    int OnCallInfo(pjsip_rx_data* rdata);

	int OnCallMessage(pjsip_rx_data* rdata);

    int OnCallBye(pjsip_rx_data* rdata);

	int SendStatelessResponse(pjsip_rx_data* rdata, pjsip_tx_data* tdata);

	int SendStatelessResponse(pjsip_rx_data* rdata, int sc);

    pjsip_dialog* CreateUacDlg(const char* target, const char* to);

    pjsip_dialog* CreateUasDlg(pjsip_rx_data* rdata);

	int SendInvite(pjsip_dialog* dlg, const char* body, const char* subject);

	int SendCallAnswer(pjsip_dialog* dlg, pjsip_transaction* invTsx, int sc, const char* sdp);

	int SendCallMessage(pjsip_dialog* dlg, const char* m, int cseq = -1, const char* body = NULL);

	int SendStatefulResponse(pjsip_dialog* dlg, pjsip_rx_data* rdata, int st = 200);

	int SendMessageRequest(const char* body, std::shared_ptr<SipRespCheck> sc);

	void PushAsyncCommand(std::function<void()>&& f);

	int OnAuthorized(pjsip_event* e);

	int OnRegSucceed(pjsip_rx_data* rdata);

	int HeartbeatTask();

	int SendNewRegister();
public:
	std::shared_ptr<RegClient> _regc;

	void InitRegisterClient();
public:
	pj_str_t MediaType;
	pj_str_t XmlSubType;
	pj_str_t hUserAgent;
	pj_str_t vUserAgent;

	char _localIP[22] = { 0 };
private:
	SipUAC();

	void InitRandSeed();

	void NormalInit();

	pj_status_t UpdateContact(pjsip_rx_data* rdata);

	pjsip_date_hdr* CreateDateHdr(pj_pool_t* pool);

    void AddUserAgentHdr(pjsip_tx_data* tdata);
private:

private:
	pjsip_endpoint* _pj_endpt = NULL;
	pj_pool_t* _pool = NULL;
	pj_pool_t* _contact_pool = NULL;
	pj_pool_t* _reg_pool = NULL;
	const pj_str_t* hostName = NULL;
	pj_caching_pool _cache_pool;
	pjsip_transport* _transport = NULL;
	pjsip_contact_hdr* _contact_hdr = NULL;
	bool _quitFlag = false;
	pjsip_auth_srv _auth_srv;
	pj_thread_t* _pj_thr[4];

	pjsip_method _messageMethod;
	pj_str_t _from;
	pj_str_t _to;
	pj_str_t _contact;

	char _reg_addr[256] = { 0 };

    SMessage _sMsg;
    SCall _sCall;
private:
	std::mutex _sipCmdLock;
	std::list <std::function<void()>> _sipCmdList;

	std::mutex _regLock;
	std::condition_variable _regCond;

	std::mutex _sLock;
	std::map<std::string, std::shared_ptr<SipRespCheck>> scp;
};

