#pragma once
#include <string>
#include <memory>

class AuthorizationInfo
{
public:
	std::string realm;
	std::string method;
	std::string nonce;
	std::string nc;
	std::string cnonce;
	std::string qop;//auth auth-int
	std::string uri;
	std::string response;
};

class DigestAuth :public std::enable_shared_from_this<DigestAuth>
{
public:
	static DigestAuth& Instance();

	~DigestAuth();

	void setUserPassword(std::string usr, std::string password);
	/*
	 @param m 0 http 1 rtsp
	*/
	std::string createWWWAuth(unsigned char m = 0);

	bool VerifyResponse(AuthorizationInfo& info);
private:
	DigestAuth();

private:
	std::string fUserName;
	std::string fPassword;
};

