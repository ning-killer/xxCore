#pragma once
#include <string>
#include <memory>
/*ws secrity*/
class WSAuth :public std::enable_shared_from_this<WSAuth>
{
public:
	static WSAuth& Instance();

	~WSAuth();

	void setPassword(const std::string& pwd);

	bool VerifyPassword(const std::string& cipherPassword, const std::string& nonce, const std::string& createdTime);
private:
	WSAuth();
private:
	std::string _password;
};

