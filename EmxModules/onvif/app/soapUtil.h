#pragma once
#include <string>

class SoapUtil
{
public:
	static const char* soap_base642s(const char* s, char* t, size_t l, int* n);

	static std::string soap_rand_uuid(char* msg);

	static const char* soap_hex2s(const char* s, char* t, size_t l, int* n);

	static std::string soap_createTime(time_t n);

	static std::string soap_strRand();

	static std::string soap_md5(const std::string& ha);

	static std::string soap_Nonce();
};

