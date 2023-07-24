#pragma once
#include <string>
#include <vector>
#include <memory>
#include "tinyxml2.h"

class SoapDispatcher;
class tt__IPAddress;
class BaseDeviceBindingResponse
{
public:
	tinyxml2::XMLElement* InitHeader(const std::string& tag);
	virtual ~BaseDeviceBindingResponse();
protected:
	tinyxml2::XMLDocument _doc;
};
/* tt:ScopeDefinition */
enum tt__ScopeDefinition {
	tt__ScopeDefinition__Fixed = 0,
	tt__ScopeDefinition__Configurable = 1
};
enum tt__CapabilityCategory {
	tt__CapabilityCategory__All = 0,
	tt__CapabilityCategory__Analytics = 1,
	tt__CapabilityCategory__Device = 2,
	tt__CapabilityCategory__Events = 3,
	tt__CapabilityCategory__Imaging = 4,
	tt__CapabilityCategory__Media = 5,
	tt__CapabilityCategory__PTZ = 6
};
class tt__Scope
{
public:
	enum tt__ScopeDefinition ScopeDef;
	std::string ScopeItem;
};

class _tds__GetScopesResponse :public BaseDeviceBindingResponse
{
public:
	std::vector<std::shared_ptr<tt__Scope>> Scopes;
	std::string Serialization();
};

class _tds__GetDeviceInformationResponse :public BaseDeviceBindingResponse
{
public:
	std::string Manufacturer;
	std::string Model;
	std::string FirmwareVersion;
	std::string SerialNumber;
	std::string HardwareId;

	std::string Serialization();
};
//CapabilitiesResponse
class tt__AnalyticsCapabilities
{
public:
	std::string XAddr;
	bool RuleSuppor;
	bool AnalyticsModuleSupport;
};
//for tt__DeviceCapabilities
class tt__NetworkCapabilities
{
public:
	bool IPFilter;
	bool ZeroConfiguration;
	bool IPVersion6;
	bool DynDNS;
};

class tt__SystemCapabilities
{
public:
	bool DiscoveryResolve;
	bool DiscoveryBye;
	bool RemoteDiscovery;
	bool SystemBackup;
	bool SystemLogging;
	bool FirmwareUpgrade;
	/*tt:OnvifVersion*/
	int Major;//SupportedVersions
	int Minor;
	//tt__SystemCapabilitiesExtension* Extension;
};
class tt__IOCapabilities
{
public:
	int InputConnectors;
	int RelayOutputs;
};

class tt__SecurityCapabilities
{
public:
	bool TLS1_x002e1;
	bool TLS1_x002e2;
	bool OnboardKeyGeneration;
	bool AccessPolicyConfig;
	bool X_x002e509Token;
	bool SAMLToken;
	bool KerberosToken;
	bool RELToken;
};
class tt__EventCapabilities
{
public:
	std::string XAddr;
	bool WSSubscriptionPolicySupport = false;
	bool WSPullPointSupport = true;
	bool WSPausableSubscriptionManagerInterfaceSupport = false;
};
class tt__RealTimeStreamingCapabilities
{
public:
	bool RTPMulticast;//tt:RTPMulticast
	bool RTP_USCORETCP;//tt:RTP_TCP
	bool RTP_USCORERTSP_USCORETCP;//tt:RTP_RTSP_TCP
};
class tt__MediaCapabilities
{
public:
	std::string XAddr;
	std::shared_ptr<tt__RealTimeStreamingCapabilities> StreamingCapabilities;
	//tt__MediaCapabilitiesExtension *Extension;
};
class tt__ImagingCapabilities
{
public:
	std::string XAddr;
};
class tt__PTZCapabilities
{
public:
	std::string XAddr;
};
//for extension capability
class tt__DeviceIOCapabilities
{
public:
	std::string XAddr;
	int VideoSources;
	int VideoOutputs;
	int AudioSources;
	int AudioOutputs;
	int RelayOutputs;
};
class tt__DisplayCapabilities
{
public:
	std::string XAddr;
	bool FixedLayout;
};
class tt__RecordingCapabilities
{
public:
	std::string XAddr;
	bool ReceiverSource;//tt:ReceiverSource
	bool MediaProfileSource;//tt:MediaProfileSource
	bool DynamicRecordings; //tt:DynamicRecordings
	bool DynamicTracks;//tt:DynamicTracks
	int MaxStringLength;//tt:MaxStringLength
};
class tt__SearchCapabilities
{
public:
	std::string XAddr;
	bool MetadataSearch;
};
class tt__ReplayCapabilities
{
public:
	std::string XAddr;
};
class tt__ReceiverCapabilities
{
public:
	std::string XAddr;
	bool RTP_USCOREMulticast;//tt:RTP_Multicast
	bool RTP_USCORETCP;//tt:RTP_TCP
	bool RTP_USCORERTSP_USCORETCP;//tt:RTP_RTSP_TCP
	int SupportedReceivers;//tt:SupportedReceivers
	int MaximumRTSPURILength;//tt:MaximumRTSPURILength
};
class tt__AnalyticsDeviceCapabilities
{
public:
	std::string XAddr;
	bool* RuleSupport;//tt:RuleSupport
	//tt__AnalyticsDeviceExtension *Extension;
};
class tt__CapabilitiesExtension
{
public:
	std::shared_ptr<tt__DeviceIOCapabilities> DeviceIO;//tt:DeviceIOCapabilities
	std::shared_ptr<tt__DisplayCapabilities> Display; //tt:DisplayCapabilities
	std::shared_ptr<tt__RecordingCapabilities> Recording; ////tt:RecordingCapabilities
	std::shared_ptr<tt__SearchCapabilities> Search; //tt:SearchCapabilities
	std::shared_ptr<tt__ReplayCapabilities> Replay;//tt:ReceiverCapabilities
	std::shared_ptr<tt__ReceiverCapabilities> Receiver;//tt:ReceiverCapabilities
	std::shared_ptr<tt__AnalyticsDeviceCapabilities> AnalyticsDevice;//tt:AnalyticsDeviceCapabilities
	//tt__CapabilitiesExtension2* Extensions;tt:CapabilitiesExtension2
};
class tt__DeviceCapabilities
{
public:
	std::string XAddr;
	std::shared_ptr<tt__NetworkCapabilities> Network;
	std::shared_ptr<tt__SystemCapabilities> System;
	std::shared_ptr<tt__IOCapabilities> IO;
	std::shared_ptr<tt__SecurityCapabilities> Security;
	//std::shared_ptr <tt__DeviceCapabilitiesExtension> Extension;
};
class _tds__GetCapabilitiesResponse :public BaseDeviceBindingResponse
{
public:
	//tt__Capabilities{}
	std::shared_ptr<tt__AnalyticsCapabilities> Analytics;
	std::shared_ptr<tt__DeviceCapabilities> Device;
	std::shared_ptr<tt__EventCapabilities> Events;
	std::shared_ptr<tt__ImagingCapabilities> Imaging;
	std::shared_ptr<tt__MediaCapabilities> Media;
	std::shared_ptr<tt__PTZCapabilities> PTZ;
	std::shared_ptr<tt__CapabilitiesExtension> Extension;
	std::string Serialization();
};
/*_tds__GetNetworkInterfacesResponse*/
/* tt:Duplex */
class tt__NetworkInterfaceInfo
{
public:
	std::string Name;
	std::string HwAddress;
	int MTU;
};
class tt__IPv4Configuration
{
public:
	/* Address  PrefixLength*/
	std::shared_ptr<std::pair<std::string, int>> Manual;//o
	std::shared_ptr<std::pair<std::string, int>> LinkLocal;//o
	std::shared_ptr<std::pair<std::string, int>> FromDHCP;//o
	bool DHCP;
};
class tt__IPv4NetworkInterface
{
public:
	bool Enabled;
	std::shared_ptr<tt__IPv4Configuration> Config;
};
class tt__IPv6Configuration
{
public:
	bool AcceptRouterAdvert;
	std::string DHCPConiguration;//Auto Stateful Stateless Off
	std::shared_ptr<std::pair<std::string, int>> Manual;
	std::shared_ptr<std::pair<std::string, int>> LinkLocal;
	std::shared_ptr<std::pair<std::string, int>> FromDHCP;
	std::shared_ptr<std::pair<std::string, int>> FromRA;
};
class tt__IPv6NetworkInterface
{
public:
	bool Enabled;
	std::shared_ptr<tt__IPv6Configuration> Config;
};
class tt__NetworkInterface
{
public:
	bool Enabled;
	std::string attrToken;
	std::shared_ptr<tt__NetworkInterfaceInfo> Info;
	std::shared_ptr<tt__IPv4NetworkInterface> IPv4;
	std::shared_ptr<tt__IPv6NetworkInterface> IPv6;
};
class _tds__GetNetworkInterfacesResponse :public BaseDeviceBindingResponse
{
public:
	std::vector<std::shared_ptr<tt__NetworkInterface>> NetworkInterfaces;
	std::string Serialization();
};
/*_tds__GetHostnameResponse*/
class _tds__GetHostnameResponse :public BaseDeviceBindingResponse
{
public:
	/*tt__HostnameInformation*/
	bool FromDHCP;
	std::string Name;
	std::string Serialization();
};

class tt__DateTime
{
public:
	/*tt__Time*/
	int Hour;
	int Minute;
	int Second;
	/*tt__Date*/
	int Year;
	int Month;
	int Day;
};
class _tds__GetSystemDateAndTimeResponse :public BaseDeviceBindingResponse
{
public:
	/*tt__SystemDateTime*/
	std::string SetDateTimeType;//Manual or NTP
	bool DaylightSavings;
	std::string tz = "+08";
	tt__DateTime UTCDateTime;
	tt__DateTime LocalDateTime;
	std::string Serialization();
};
class _tds__SetSystemDateAndTime
{
public:
	std::string DateTimeType;//Manual or NTP Required element 'tds:DateTimeType'
	bool DaylightSavings;// Required element 'tds:DaylightSavings'
	std::string TZ;//tt:TZ  // Optional element 'tds:TimeZone'
	std::shared_ptr<tt__DateTime> UTCDateTime;// Optional element 'tds:UTCDateTime'
};

class tds__Service
{
public:
	std::string Namespace;
	/// Required element 'tds:XAddr' of XSD type 'xsd:anyURI'
	std::string XAddr;
	/// Optional element 'tds:Capabilities' of XSD type 'tds:Service-Capabilities'
	//_tds__Service_Capabilities* Capabilities;
	/*tt__OnvifVersion* Version*/
	int Major;
	int Minor;
};
class _tds__GetServicesResponse :public BaseDeviceBindingResponse
{
public:
	std::vector<std::shared_ptr<tds__Service>> Service;
	std::string Serialization();
};

class _tds__GetDNSResponse :public BaseDeviceBindingResponse
{
public:
	// Required element 'tds:DNSInformation'
	bool FromDHCP;// Required element 'tt:FromDHCP' 
	std::string SearchDomain;// Optional element 'tt:SearchDomain'
	std::vector<std::shared_ptr<tt__IPAddress>> DNSFromDHCP;// Optional element 'tt:DNSFromDHCP'
	std::vector<std::shared_ptr<tt__IPAddress>> DNSManual;// Optional element 'tt:DNSManual'
	std::string Serialization();
};

class tt__NetworkProtocol
{
public:
	std::string Name;// HTTP HTTPS RTSP Required element 'tt:Name'
	bool Enabled;// Required element 'tt:Enabled'
	int Port;// Required element 'tt:Port'
};
class _tds__GetNetworkProtocolsResponse :public BaseDeviceBindingResponse
{
public:
	std::vector<std::shared_ptr<tt__NetworkProtocol>> NetworkProtocols;
	std::string Serialization();
};
class tt__NetworkHost
{
public:
	std::string Type;//IPv4 IPv6 DNS Required element 'tt:Type'
	std::shared_ptr<std::string> IPv4Address;// Optional element 'tt:IPv4Address'
	std::shared_ptr<std::string> IPv6Address;// Optional element 'tt:IPv6Address'
	std::shared_ptr<std::string> DNSname;// Optional element 'tt:DNSname'
};
class _tds__GetNTPResponse :public BaseDeviceBindingResponse
{
public:
	/*Required element 'tds:NTPInformation'*/
	bool FromDHCP;
	std::shared_ptr<tt__NetworkHost> NTPFromDHCP;// Optional element 'tt:NTPFromDHCP'
	std::shared_ptr<tt__NetworkHost> NTPManual;// Optional element 'tt:NTPManual'
	//tt__NTPInformationExtension* Extension;// Optional element 'tt:Extension'
	std::string Serialization();
};
class _tds__GetNetworkDefaultGatewayResponse :public BaseDeviceBindingResponse
{
public:
	/*Required element 'tds:NetworkGateway'*/
	std::string IPv4Address;
	//std::vector<std::string> IPv6Address;// Optional element 'tt:IPv6Address'
	std::string Serialization();
};
class tt__User :public BaseDeviceBindingResponse
{
public:
	std::string Username;// Required element 'tt:Username'
	std::string Password;// Optional element 'tt:Password'
	/// Required element 'tt:UserLevel' of XSD type 'tt:UserLevel'
	std::string	UserLevel;//Administrator Operator User Anonymous Extended
	//tt__UserExtension* Extension;// Optional element 'tt:Extension' 
};
class _tds__GetUsersResponse :public BaseDeviceBindingResponse
{
public:
	std::vector<std::shared_ptr<tt__User>> User;
	std::string Serialization();
};
class soapDeviceBinding
{
public:
	soapDeviceBinding();
	~soapDeviceBinding();

private:
	int OnGetScopes(void* req, std::string& body);

	int OnGetDeviceInformation(void* req, std::string& body);

	int OnGetDNS(void* req, std::string& body);

	int OnSetDNS(void* req, std::string& body);

	int OnGetCapabilities(void* req, std::string& body);

	int OnGetNetworkInterfaces(void* req, std::string& body);

	int OnGetHostname(void* req, std::string& body);

	int OnSystemReboot(void* req, std::string& body);

	int OnGetSystemDateAndTime(void* req, std::string& body);

	int OnSetSystemDateAndTime(void* req, std::string& body);

	int OnGetServices(void* req, std::string& body);

	int OnGetDiscoveryMode(void* req, std::string& body);

	int OnGetNetworkProtocols(void* req, std::string& body);

	int OnGetNTP(void* req, std::string& body);

	int OnGetNetworkDefaultGateway(void* req, std::string& body);

	int OnGetUsers(void* req, std::string& body);

	int OnSetUsers(void* req, std::string& body);
public:
	friend SoapDispatcher;
};