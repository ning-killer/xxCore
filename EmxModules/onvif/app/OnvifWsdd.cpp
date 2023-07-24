#include "OnvifWsdd.h"
#include "sockutil.h"
#include <iostream>
//#include <iomanip>
#include "tinyxml2.h"
#include "soapdefine.h"
#include "soapUtil.h"

OnvifWsdd::OnvifWsdd(const EventPoller::Ptr poller)
{
	if (!_poller)
		_poller = EventPollerPool::Instance().getPoller();

	recvMsg = new char[65536];
	m_localIP = SockUtil::guess_local_ipv4();
	//std::cout << m_localIP << std::endl;
}

OnvifWsdd::~OnvifWsdd()
{
	_poller->delEvent(_fd, [](bool b) {});

	setsockopt(_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, _broadCastAddr, sizeof(ip_mreq));
	if (_fd)
	{
#ifdef _WIN32
		closesocket(_fd);
#else
		close(_fd);
#endif 
	}
	delete[] recvMsg;
}

void OnvifWsdd::initNamespaces()
{
	_namespaces.push_back({ "xmlns:SOAP-ENV","http://www.w3.org/2003/05/soap-envelope" });
	_namespaces.push_back({ "xmlns:SOAP-ENC","http://www.w3.org/2003/05/soap-encoding" });
	_namespaces.push_back({ "xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance" });
	_namespaces.push_back({ "xmlns:xsd","http://www.w3.org/2001/XMLSchema" });
	_namespaces.push_back({ "xmlns:wsa","http://schemas.xmlsoap.org/ws/2004/08/addressing" });
	_namespaces.push_back({ "xmlns:d","http://schemas.xmlsoap.org/ws/2005/04/discovery" });
	_namespaces.push_back({ "xmlns:tdn","http://www.onvif.org/ver10/network/wsdl" });
	_namespaces.push_back({ "xmlns:tds","http://www.onvif.org/ver10/device/wsdl" });
}

int OnvifWsdd::run()
{
	_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_fd == -1)	{
		std::clog << "create socket failed " << errno << std::endl;
		return 1;
	}
	ip_mreq mcast;
	inet_pton(AF_INET, "239.255.255.250", &mcast.imr_multiaddr);
	mcast.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast)) < 0)
		return 1;
	unsigned char loop = 0;
	if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loop, sizeof(loop)) < 0)
		return 1;
	
	sockaddr_in sa = { AF_INET };
	sa.sin_port = htons(3702);
	sa.sin_addr.s_addr = INADDR_ANY;
	if (bind(_fd, (sockaddr*)&sa, sizeof(sockaddr_in)) < 0)
	{
		std::clog << "bind failed " << errno << std::endl;
		return 1;
	}
	
	memcpy(_broadCastAddr, &mcast, sizeof(ip_mreq));

	_header = std::make_shared<SoapHeader>();
	initNamespaces();
	_addresID = SoapUtil::soap_rand_uuid(recvMsg);
	
	//sendHello();
	_poller->addEvent(_fd, std::bind(&OnvifWsdd::wsddServer, this));
	std::cout << "start run device discovery service" << std::endl;
	return 0;
}

int OnvifWsdd::wsddServer()
{
	ssize_t ret;
	memset(recvMsg, 0, 65536);
	sockaddr_in perrAddr;
	socklen_t addrLen = sizeof(sockaddr_in);
	ret = ::recvfrom(_fd, recvMsg, 65536, 0, (sockaddr*)&perrAddr, &addrLen);
	if (ret < 0)
	{
		std::cout << "recvfrom failed " << errno << std::endl;
		return 0;
	}
	memcpy(_peerAddr, &perrAddr, sizeof(sockaddr_in));
	parseSoap(ret);

	return 0;
}

int OnvifWsdd::parseSoap(size_t dataSize)
{
	tinyxml2::XMLDocument doc;
	auto err = doc.Parse(recvMsg, dataSize);
	if (err != tinyxml2::XML_SUCCESS)
	{
		fprintf(stderr, "parse xml failed\n");
		return 1;
	}
	//Envelope
	auto envelopeNode = doc.FirstChildElement();
	auto strEnvelope = envelopeNode->Value();
	auto p = strstr(strEnvelope, "Envelope");
	if (!p)
	{
		std::cout << "not a incorrect soap message" << std::endl;
		return 2;
	}
	//Header or body
	auto child = envelopeNode->FirstChildElement();
	if (strstr(child->Value(),"Header"))
	{
		for (auto head = child->FirstChildElement(); head; head = head->NextSiblingElement())
		{
			if (strstr(head->Value(), "MessageID"))
			{
				_header->wsa_MessageID = head->GetText();
			}
			else if (strstr(head->Value(), "ReplyTo"))
			{
			}
			else if (strstr(head->Value(), "To"))
			{
				_header->wsa_To = head->GetText();
			}
			else if (strstr(head->Value(), "Action"))
			{
				_header->wsa_Action = head->GetText();
			}
			else if (strstr(head->Value(), "RelatesTo"))
			{
				_header->wsa_RelatesTo = head->GetText();
			}
			else
			{
				if (child->BoolText())
					std::cout << "not handle " << child->Value() << ":" << child->GetText() << std::endl;
			}
		}
		child = child->NextSiblingElement();
	}
	std::string tag;
	if (strstr(child->Value(), "Body"))
	{
		tag = child->FirstChildElement()->Value();
		//for (auto body = child->FirstChildElement(); body; body = body->NextSiblingElement())
		//{
		//
		//}
	}
	if (strstr(tag.c_str(), "Probe"))
	{
		sendProbeMatches();
	}
	else if(strstr(tag.c_str(), "Reslove"))
	{
	}
	else if (strstr(tag.c_str(), "Hello"))
	{
        
	}
//	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//std::cout << std::put_time(std::localtime(&t), "%F %T") << " " << recvMsg << std::endl;
	//std::cout << std::put_time(std::localtime(&t), "%F %T") << " " << tag << std::endl;
	return 0;
}

int OnvifWsdd::sendHello()
{
	_header->Reset();
	_header->wsa_Action = SOAP_NAMESPACE_OF_wsdd"/Hello";
	_header->wsa_MessageID = SoapUtil::soap_rand_uuid(recvMsg);
	_header->wsa_To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";

	tinyxml2::XMLDocument doc;
	/*declare*/
	auto declare = doc.NewDeclaration();
	doc.InsertFirstChild(declare);
	/*Envelope*/
	auto envelopeNode = doc.NewElement("SOAP-ENV:Envelope");
	doc.InsertEndChild(envelopeNode);
	auto it = _namespaces.begin();
	while (it != _namespaces.end())
	{
		envelopeNode->SetAttribute(it->first.c_str(), it->second.c_str());
		++it;
	}
	/*header*/
	auto headerNode = doc.NewElement("SOAP-ENV:Header");
	envelopeNode->InsertEndChild(headerNode);
	if (!_header->wsa_MessageID.empty())
	{
		auto messageIdNode = doc.NewElement("wsa:MessageID");
		messageIdNode->SetText(_header->wsa_MessageID.c_str());
		headerNode->InsertEndChild(messageIdNode);
	}
	if (!_header->wsa_To.empty())
	{
		auto toNode = doc.NewElement("wsa:To");
		toNode->SetText(_header->wsa_To.c_str());
		toNode->SetAttribute("SOAP-ENV:mustUnderstand", "true");
		headerNode->InsertEndChild(toNode);
	}
	if (!_header->wsa_Action.empty())
	{
		auto actionNode = doc.NewElement("wsa:Action");
		actionNode->SetText(_header->wsa_Action.c_str());
		actionNode->SetAttribute("SOAP-ENV:mustUnderstand", "true");
		headerNode->InsertEndChild(actionNode);
	}
	if (!_header->wsa_RelatesTo.empty())
	{
		auto relatesToNode = doc.NewElement("wsa:RelatesTo");
		relatesToNode->SetText(_header->wsa_RelatesTo.c_str());
		headerNode->InsertEndChild(relatesToNode);
	}
	/*body*/
	auto bodyNode = doc.NewElement("SOAP-ENV:Body");
	envelopeNode->InsertEndChild(bodyNode);
	auto helloNode = doc.NewElement("d:Hello");
	bodyNode->InsertEndChild(helloNode);
	/*ProbeMatch*/

	auto endpointReferenceNode = doc.NewElement("wsa:EndpointReference");
	helloNode->InsertEndChild(endpointReferenceNode);
	auto addressNode = doc.NewElement("wsa:Address");
	addressNode->SetText(_addresID.c_str());
	endpointReferenceNode->InsertEndChild(addressNode);

	auto typesNode = doc.NewElement("d:Types");
	typesNode->SetText("tdn:NetworkVideoTransmitter tds:Device");
	helloNode->InsertEndChild(typesNode);

	auto scopesNode = doc.NewElement("d:Scopes");
	std::string strScopes;
	for (auto& s : EPSoap::Instance().vScopes)
	{
		strScopes.append(s);
		strScopes.append(" ");
	}
	strScopes.pop_back();
	scopesNode->SetText(strScopes.c_str());
	helloNode->InsertEndChild(scopesNode);

	auto xAddrsNode = doc.NewElement("d:XAddrs");
	char uri[256];
	snprintf(uri, 256, "http://%s/onvif/device_service", m_localIP.c_str());
	xAddrsNode->SetText(uri);
	helloNode->InsertEndChild(xAddrsNode);

	auto metadataVersionNode = doc.NewElement("d:MetadataVersion");
	metadataVersionNode->SetText(1);
	helloNode->InsertEndChild(metadataVersionNode);

	tinyxml2::XMLPrinter printer(0, true);
	doc.Print(&printer);
	printer.PushText("\r\n");
//	auto p = printer.CStr();

	sockaddr_in castAdddr = { AF_INET };
	castAdddr.sin_port = htons(3702);
	inet_pton(AF_INET, "239.255.255.250", &castAdddr.sin_addr);
	auto b = ::sendto(_fd, printer.CStr(), printer.CStrSize() - 1, 0, (sockaddr*)&castAdddr, sizeof(sockaddr_in));
	if (b <= 0)
		std::cerr << "sent hello failed " << errno << std::endl;
	return 0;
}

int OnvifWsdd::sendProbeMatches()
{
	_header->wsa_Action = SOAP_NAMESPACE_OF_wsdd"/ProbeMatches";
	_header->wsa_RelatesTo = _header->wsa_MessageID;
	_header->wsa_MessageID = SoapUtil::soap_rand_uuid(recvMsg);
	_header->wsa_To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";

	tinyxml2::XMLDocument doc;
	/*declare*/
	auto declare = doc.NewDeclaration();
	doc.InsertFirstChild(declare);
	/*Envelope*/
	auto envelopeNode = doc.NewElement("SOAP-ENV:Envelope");
	doc.InsertEndChild(envelopeNode);
	auto it = _namespaces.begin();
	while (it != _namespaces.end())
	{
		envelopeNode->SetAttribute(it->first.c_str(), it->second.c_str());
		++it;
	}
	/*header*/
	auto headerNode = doc.NewElement("SOAP-ENV:Header");
	envelopeNode->InsertEndChild(headerNode);
	if (!_header->wsa_MessageID.empty())
	{
		auto messageIdNode = doc.NewElement("wsa:MessageID");
		messageIdNode->SetText(_header->wsa_MessageID.c_str());
		headerNode->InsertEndChild(messageIdNode);
	}
	if (!_header->wsa_To.empty())
	{
		auto toNode = doc.NewElement("wsa:To");
		toNode->SetText(_header->wsa_To.c_str());
		toNode->SetAttribute("SOAP-ENV:mustUnderstand", "true");
		headerNode->InsertEndChild(toNode);
	}
	if (!_header->wsa_Action.empty())
	{
		auto actionNode = doc.NewElement("wsa:Action");
		actionNode->SetText(_header->wsa_Action.c_str());
		actionNode->SetAttribute("SOAP-ENV:mustUnderstand", "true");
		headerNode->InsertEndChild(actionNode);
	}
	if (!_header->wsa_RelatesTo.empty())
	{
		auto relatesToNode = doc.NewElement("wsa:RelatesTo");
		relatesToNode->SetText(_header->wsa_RelatesTo.c_str());
		headerNode->InsertEndChild(relatesToNode);
	}
	/*body*/
	auto bodyNode = doc.NewElement("SOAP-ENV:Body");
	envelopeNode->InsertEndChild(bodyNode);
	auto probeMatchesNode = doc.NewElement("d:ProbeMatches");
	bodyNode->InsertEndChild(probeMatchesNode);
	/*ProbeMatch*/
	auto probeMatchNode = doc.NewElement("d:ProbeMatch");
	probeMatchesNode->InsertEndChild(probeMatchNode);
	
	auto endpointReferenceNode = doc.NewElement("wsa:EndpointReference");
	probeMatchNode->InsertEndChild(endpointReferenceNode);
	auto addressNode = doc.NewElement("wsa:Address");
	addressNode->SetText(_addresID.c_str());
	endpointReferenceNode->InsertEndChild(addressNode);
	
	auto typesNode = doc.NewElement("d:Types");
	typesNode->SetText("tdn:NetworkVideoTransmitter tds:Device");
	probeMatchNode->InsertEndChild(typesNode);

	auto scopesNode = doc.NewElement("d:Scopes");
	std::string strScopes;
	for (auto& s : EPSoap::Instance().vScopes)
	{
		strScopes.append(s);
		strScopes.append(" ");
	}
	strScopes.pop_back();
	scopesNode->SetText(strScopes.c_str());
	probeMatchNode->InsertEndChild(scopesNode);

	auto xAddrsNode = doc.NewElement("d:XAddrs");
	char uri[256];
	snprintf(uri, 256, "http://%s/onvif/device_service", m_localIP.c_str());
	xAddrsNode->SetText(uri);
	probeMatchNode->InsertEndChild(xAddrsNode);

	auto metadataVersionNode = doc.NewElement("d:MetadataVersion");
	metadataVersionNode->SetText(1);
	probeMatchNode->InsertEndChild(metadataVersionNode);

	tinyxml2::XMLPrinter printer(0, true);
	doc.Print(&printer);
	printer.PushText("\r\n");
	auto b = ::sendto(_fd, printer.CStr(), printer.CStrSize() - 1, 0, (sockaddr*)_peerAddr, sizeof(sockaddr_in));
	if (b <= 0)
		std::cerr << "sento failed" << errno << std::endl;
	return 0;
}
