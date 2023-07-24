#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>

#define SOAP_NAMESPACE_OF_wsa	"http://schemas.xmlsoap.org/ws/2004/08/addressing"
#define SOAP_NAMESPACE_OF_wsdd	"http://schemas.xmlsoap.org/ws/2005/04/discovery"
#define SOAP_NAMESPACE_OF_tdn	"http://www.onvif.org/ver10/network/wsdl"    
#define CAM_HARDWARE "hardware/EP-QJ02-W"   //todo
#define CAM_NAME "name/IPC"
#define CAM_MANUFACTURER "manufacturer/Emx"

class NetworkInterfaceInfo
{
public:
	bool enable = true;
	int mtu = 1500;
	unsigned int type = 6;//MIB_IF_TYPE_ETHERNET 71 IF_TYPE_IEEE80211
	std::string ipv4;
	bool dhcp = true;
	bool ipv4Enable = false;
	int ipv4Prefixlen = 24;
	std::string ipv6;
	int ipv6Prefixlen = 64;
	bool ipv6Enable = false;
	std::string mac;
};

class SoapHeader
{
public:
	std::string wsa_MessageID;
	std::string wsa_To;
	std::string wsa_Action;
	std::string wsa_RelatesTo;

	void Reset();
};

class _wsse__UsernameToken
{
public:
	std::string Username;
	/*password*/
	std::string cipherPassword;
	std::string Type;
	/*nonce*/
	std::string cipherNonce;
	std::string EncodingType;

	std::string wsu__Created;
	std::string wsu__Id;
};

class _wsse__Security
{
public:
	std::shared_ptr<_wsse__UsernameToken> userToken;
};

class EPSoap :public std::enable_shared_from_this<EPSoap>
{
public:
	~EPSoap();

	static EPSoap& Instance();

	void Init();

	std::string CreateSoapFaultMessage(std::string&& value, std::string&& subValue, std::string&& text);

	std::string PasswordTextURI;
	std::string PasswordDigestURI;
	std::string Base64BinaryURI;
	std::string HexBinaryURI;

	std::vector<std::string> vScopes;

	std::map<std::string, NetworkInterfaceInfo> ifMap;//�豸������������Ϣ��
	std::map<std::string, std::string> nsXaddrMap;//�����ռ�ӳ���xaddr��

	std::string snapshotUri;

	std::string RtspMainStreamUri;
	std::string RtspSubStreamUri;
	std::string ReplayUri;

	std::string SubscriptionUri;
	std::string strAuthenticationError;
private:
	void getNetworkInterface();

	void InitNamespaceXaddr();

	void InitErrorString();
private:
	EPSoap();
	
};

class SoapRequest
{
public:
	std::shared_ptr<_wsse__Security> securityHeader;
	std::string tag;
};


