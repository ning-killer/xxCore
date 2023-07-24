#include "DigestAuth.h"
#include "soapUtil.h"
#include <stdio.h>
#ifndef _WIN32
#include <string.h>
#endif

DigestAuth::DigestAuth()
{
}

DigestAuth::~DigestAuth()
{
}

void DigestAuth::setUserPassword(std::string usr, std::string password)
{
	fUserName = usr;
	fPassword = password;
}

DigestAuth& DigestAuth::Instance()
{
	static std::shared_ptr<DigestAuth> g_instance(new DigestAuth);
	static DigestAuth& g_instance_ref = *g_instance;
	return g_instance_ref;
}

std::string DigestAuth::createWWWAuth(unsigned char m)
{
	char buf[256];
	if(m==0)
		snprintf(buf, 256, "WWW-Authenticate: Digest realm=\"epail001\", qop=\"auth\", nonce=\"%s\"\r\n", SoapUtil::soap_Nonce().c_str());
	else
		snprintf(buf, 256, "WWW-Authenticate: Digest realm=\"epail001\", nonce=\"%s\"\r\n", SoapUtil::soap_Nonce().c_str());
	return std::string(buf);
}

bool DigestAuth::VerifyResponse(AuthorizationInfo& info)
{
	//ha1=md5(user:realm:pwd)
	//ha2=md5(method:digestURI)
	//response=md5(ha1:nonce:ha2) 
	std::string ha1;
	std::string ha2;
	std::string ha3;

	ha1.append(fUserName);
	ha1.append(":");
	ha1.append(info.realm);
	ha1.append(":");
	ha1.append(fPassword);

	ha2.append(info.method);
	ha2.append(":");
	ha2.append(info.uri);

	auto data1 = SoapUtil::soap_md5(ha1);
	auto data2 = SoapUtil::soap_md5(ha2);

	std::string response;
	if (info.qop.empty())
	{
		ha3.append(data1);
		ha3.append(":");
		ha3.append(info.nonce);
		ha3.append(":");
		ha3.append(data2);
		response = SoapUtil::soap_md5(ha3);
	}
	else if (!strcmp(info.qop.c_str(), "auth"))
	{
		ha3.append(data1);
		ha3.append(":");
		ha3.append(info.nonce);
		ha3.append(":");
		ha3.append(info.nc); 
		ha3.append(":");
		ha3.append(info.cnonce);
		ha3.append(":");
		ha3.append(info.qop);
		ha3.append(":");
		ha3.append(data2);
		response = SoapUtil::soap_md5(ha3);
	}
	else if (strcmp(info.qop.c_str(), "auth-int"))
	{
		//TODO
	}
	if (!strcmp(response.c_str(), info.response.c_str()))
		return true;
   
	return false;
}
