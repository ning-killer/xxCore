#include "soapdefine.h"
#include "sockutil.h"
#include "tinyxml2.h"
#ifdef _WIN32
#include <iphlpapi.h>
#pragma comment(lib,"iphlpapi.lib")
#else
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#endif
//#include "system/te_env.h"

void SoapHeader::Reset()
{
    wsa_MessageID.clear();
    wsa_To.clear();
    wsa_Action.clear();
    wsa_RelatesTo.clear();
}

EPSoap::EPSoap()
{
}

EPSoap::~EPSoap()
{
}

EPSoap& EPSoap::Instance()
{
    static std::shared_ptr<EPSoap> g_instance(new EPSoap);
    static EPSoap& g_instance_ref = *g_instance;
    return g_instance_ref;
}

void EPSoap::Init()
{
    PasswordTextURI = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText";
    PasswordDigestURI = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest";
    Base64BinaryURI = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary";
    HexBinaryURI = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#HexBinary";

    vScopes.push_back("onvif://www.onvif.org/location/country/china");
    std::string baseUri = "onvif://www.onvif.org/";
    std::string name = baseUri + CAM_NAME;
    vScopes.emplace_back(name);
    std::string manufacturer = baseUri + CAM_MANUFACTURER;
    vScopes.emplace_back(manufacturer);

    std::string hardware = baseUri + "hardware/v1.0";
    vScopes.emplace_back(hardware);

    //char strSerialnumber[128];
    //sprintf(strSerialnumber,"onvif://www.onvif.org/serialnumber/%s",te_env_get()->gb28181.device_id);
    //vScopes.push_back(strSerialnumber);

    vScopes.push_back("onvif://www.onvif.org/Profile/Streaming");
    vScopes.push_back("onvif://www.onvif.org/type/Network_Video_Transmitter");
    vScopes.push_back("onvif://www.onvif.org/Profile/S");
    vScopes.push_back("onvif://www.onvif.org/type/ptz");

    getNetworkInterface();

    InitNamespaceXaddr();

    InitErrorString();
}

void EPSoap::getNetworkInterface()
{
#ifdef _WIN32
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES_LH);
	pAddresses = new IP_ADAPTER_ADDRESSES_LH;
    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        delete pAddresses;
        pAddresses = (IP_ADAPTER_ADDRESSES_LH*)new char[outBufLen];
        GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST, NULL, pAddresses, &outBufLen);
    }
	pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {
        auto& info = ifMap[pCurrAddresses->AdapterName];
        if (IfOperStatusUp & pCurrAddresses->OperStatus)
            info.enable = true;
        else if (IfOperStatusDown & pCurrAddresses->OperStatus)
            info.enable = false;

        IP_ADAPTER_DHCP_ENABLED;
        if (IP_ADAPTER_IPV4_ENABLED & pCurrAddresses->Flags)
            info.ipv4Enable = true;
        if (IP_ADAPTER_IPV6_ENABLED & pCurrAddresses->Flags)
            info.ipv6Enable = true;
        info.mtu = pCurrAddresses->Mtu;

        auto pUnicast = pCurrAddresses->FirstUnicastAddress;
        while (pUnicast)
        {
            //IP_ADAPTER_ADDRESS_DNS_ELIGIBLE
            if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
            {
                auto ipAddr = (sockaddr_in*)pUnicast->Address.lpSockaddr;
                char buf[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &ipAddr->sin_addr, buf, INET_ADDRSTRLEN);
                info.ipv4 = buf;
                info.ipv4Prefixlen = pUnicast->OnLinkPrefixLength;
            }
            else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
            {
                auto ipAddr = (sockaddr_in6*)pUnicast->Address.lpSockaddr;
                char buf[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &ipAddr->sin6_addr, buf, INET6_ADDRSTRLEN);
                info.ipv6 = buf;
                info.ipv6Prefixlen = pUnicast->OnLinkPrefixLength;
                auto xx = pUnicast->PrefixOrigin;//IpPrefixOriginDhcp IpPrefixOriginRouterAdvertisement
            }
            pUnicast = pUnicast->Next;
        }
        char hwAddr[32];
        snprintf(hwAddr, 32, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", pCurrAddresses->PhysicalAddress[0],
            pCurrAddresses->PhysicalAddress[1], pCurrAddresses->PhysicalAddress[2], pCurrAddresses->PhysicalAddress[3],
            pCurrAddresses->PhysicalAddress[4], pCurrAddresses->PhysicalAddress[5]);
        info.mac = hwAddr;
        info.type = pCurrAddresses->IfType;
        pCurrAddresses = pCurrAddresses->Next;
    }
    delete[] pAddresses;
#else
    ifconf ifconf;
    char buf[1024 * 10];
    //��ʼ��ifconf
    ifconf.ifc_len = sizeof(buf);
    ifconf.ifc_buf = buf;
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return;
    }
    if (-1 == ioctl(sockfd, SIOCGIFCONF, &ifconf)) //��ȡ���нӿ���Ϣ
    {
        close(sockfd);
        return;
    }
    ifreq* adapter = (struct ifreq*)buf;
    for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; --i, ++adapter) {
        auto& info = ifMap[adapter->ifr_ifrn.ifrn_name];

        ioctl(sockfd, SIOCGIFMTU, adapter);
        info.mtu = adapter->ifr_ifru.ifru_mtu;

        ioctl(sockfd, SIOCGIFFLAGS, adapter);
        if (adapter->ifr_ifru.ifru_flags & IFF_UP)//IFF_RUNNING
            info.enable = true;
        else
            info.enable = false;

        ioctl(sockfd, SIOCGIFHWADDR, adapter);
        unsigned char g_hwaddr[IFHWADDRLEN];
        memcpy(g_hwaddr, adapter->ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);

        char a[32];
        snprintf(a, 24, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", g_hwaddr[0], g_hwaddr[1], g_hwaddr[2], g_hwaddr[3]
                , g_hwaddr[4], g_hwaddr[5]);
        info.mac = a;

        ioctl(sockfd, SIOCGIFADDR, adapter);
        auto pa = (sockaddr_in*)(&adapter->ifr_ifru.ifru_addr);
        inet_ntop(AF_INET, &pa->sin_addr, a, 32);
        info.ipv4 = a;
        info.ipv4Enable = true;

        ioctl(sockfd, SIOCGIFNETMASK, adapter);
        pa = (sockaddr_in*)(&adapter->ifr_ifru.ifru_netmask);
        int prefixLength = 0;
        uint32_t m = (uint32_t)ntohl(pa->sin_addr.s_addr);
        while (m & 0x80000000) {
            prefixLength++;
            m = m << 1;
        }
        info.ipv4Prefixlen = prefixLength;
        info.ipv6Enable = false;
        info.dhcp = true;
    }
    close(sockfd);
#endif
}

void EPSoap::InitNamespaceXaddr()
{
    std::string localIp = SockUtil::guess_local_ipv4();//may be not correct
    char buf[128];
    snprintf(buf, 128, "http://%s:80/onvif/device_service", localIp.c_str());
    nsXaddrMap.insert({ "http://www.onvif.org/ver10/device/wsdl", buf });

    snprintf(buf, 128, "http://%s:80/onvif/media_service", localIp.c_str());
    nsXaddrMap.insert({ "http://www.onvif.org/ver10/media/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/deviceIO_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver10/deviceIO/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/analytics_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver20/analytics/wsdl", buf });

    snprintf(buf, 128, "http://%s:80/onvif/imaging_service", localIp.c_str());
    nsXaddrMap.insert({ "http://www.onvif.org/ver20/imaging/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/event_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver10/events/wsdl", buf });

    snprintf(buf, 128, "http://%s:80/onvif/ptz_service", localIp.c_str());
    nsXaddrMap.insert({ "http://www.onvif.org/ver20/ptz/wsdl", buf });

    snprintf(buf, 128, "http://%s/onvif/search_service", localIp.c_str());
    nsXaddrMap.insert({ "http://www.onvif.org/ver10/search/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/replay_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver10/replay/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/recording_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver10/recording/wsdl", buf });

    //snprintf(buf, 128, "http://%s/onvif/media2_service", localIp.c_str());
    //nsXaddrMap.insert({ "http://www.onvif.org/ver20/media/wsdl", buf });

    snprintf(buf, 128, "http://%s:80/onvifsnapshot/media_service", localIp.c_str());
    snapshotUri = buf;

    snprintf(buf, 128, "rtsp://%s:554/main", localIp.c_str());
    RtspMainStreamUri = buf;
    snprintf(buf, 128, "rtsp://%s:554/sub", localIp.c_str());
    RtspSubStreamUri = buf;

    snprintf(buf, 128, "http://%s:80/onvif/Subscription", localIp.c_str());
    SubscriptionUri = buf;

    //snprintf(buf, 128, "rtsp://%s/record/replay", localIp.c_str());
    //ReplayUri = buf;
}

void EPSoap::InitErrorString()
{
    tinyxml2::XMLDocument doc;
    /*declare*/
    auto declare = doc.NewDeclaration();
    doc.InsertFirstChild(declare);
    /*Envelope*/
    auto envelopeNode = doc.NewElement("s:Envelope");
    doc.InsertEndChild(envelopeNode);
    envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
    envelopeNode->SetAttribute("xmlns:ter", "http://www.onvif.org/ver10/error");
    /*body*/
    auto bodyNode = doc.NewElement("s:Body");
    envelopeNode->InsertEndChild(bodyNode);

    auto faultNode = doc.NewElement("s:Fault");
    bodyNode->InsertEndChild(faultNode);
    auto codeNode = doc.NewElement("s:Code");
    auto valueNode = doc.NewElement("s:Value");
    auto subcodeNode = doc.NewElement("s:Subcode");
    auto subValueNode = doc.NewElement("s:Value");
    subValueNode->SetText("ter:NotAuthorized");
    valueNode->SetText("s:Sender");
    subcodeNode->InsertEndChild(subValueNode);
    codeNode->InsertEndChild(valueNode);
    codeNode->InsertEndChild(subcodeNode);

    auto reasonNode = doc.NewElement("s:Reason");
    auto textNode = doc.NewElement("s:Text");
    textNode->SetAttribute("xml:lang","en");
    textNode->SetText("Sender not Authorized");
    reasonNode->InsertEndChild(textNode);

    faultNode->InsertEndChild(codeNode);
    faultNode->InsertEndChild(reasonNode);
    tinyxml2::XMLPrinter printer(0, true);
    doc.Print(&printer);
    strAuthenticationError.assign(printer.CStr(), printer.CStrSize() - 1);
}

std::string EPSoap::CreateSoapFaultMessage(std::string&& value, std::string&& subValue, std::string&& text)
{
    tinyxml2::XMLDocument doc;
    /*declare*/
    auto declare = doc.NewDeclaration();
    doc.InsertFirstChild(declare);
    /*Envelope*/
    auto envelopeNode = doc.NewElement("s:Envelope");
    doc.InsertEndChild(envelopeNode);
    envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
    envelopeNode->SetAttribute("xmlns:ter", "http://www.onvif.org/ver10/error");
    /*body*/
    auto bodyNode = doc.NewElement("s:Body");
    envelopeNode->InsertEndChild(bodyNode);

    auto faultNode = doc.NewElement("s:Fault");
    bodyNode->InsertEndChild(faultNode);
    auto codeNode = doc.NewElement("s:Code");
    auto valueNode = doc.NewElement("s:Value");
    auto subcodeNode = doc.NewElement("s:Subcode");
    auto subValueNode = doc.NewElement("s:Value");
    if (!subValue.empty())
    {
        auto subcodeNode2 = doc.NewElement("s:Subcode");
        auto subCodeValueNode = doc.NewElement("s:Value");
        subCodeValueNode->SetText(subValue.c_str());
        subcodeNode2->InsertEndChild(subCodeValueNode);
        subcodeNode->InsertEndChild(subcodeNode2);
    }

    subValueNode->SetText(subValue.c_str());
    valueNode->SetText("s:Sender");
    subcodeNode->InsertEndChild(subValueNode);
    codeNode->InsertEndChild(valueNode);
    codeNode->InsertEndChild(subcodeNode);

    auto reasonNode = doc.NewElement("s:Reason");
    auto textNode = doc.NewElement("s:Text");
    textNode->SetAttribute("xml:lang", "en");
    textNode->SetText(text.c_str());
    reasonNode->InsertEndChild(textNode);

    faultNode->InsertEndChild(codeNode);
    faultNode->InsertEndChild(reasonNode);
    tinyxml2::XMLPrinter printer(0, true);
    doc.Print(&printer);

    return std::string(printer.CStr(), printer.CStrSize() - 1);
}
