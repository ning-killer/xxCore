#pragma once
#include "pjsip.h"
#include <stdint.h>

void regc_tsx_callback(void* token, pjsip_event* event);

class RegClient
{
public:
	~RegClient();
public:
	pj_pool_t* _reg_pool = NULL;
	pjsip_auth_clt_sess auth_sess;
	//pj_timer_entry _timer;

	pjsip_uri* _srv_uri = NULL;
	pjsip_from_hdr* _from_hdr = NULL;
	pjsip_expires_hdr* _expires_hdr = NULL;
	pjsip_cseq_hdr* _cseq_hdr = NULL;
	pjsip_cid_hdr* _cid_hdr = NULL;
	pjsip_contact_hdr* _contact_hdr = NULL;
	int _nExpires = 0;

	uint8_t _reg_status = 0;
	time_t _lastRegisterTime;
	time_t _lastHeartbeatTime;
	uint8_t _heartbeatFaildCount = 0;

	char _reg_addr[64];
};

