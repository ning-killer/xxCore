#include "soapDeviceBinding.h"
#include "soapdefine.h"
#include "soapMedia.h"
#include "EmxCore.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


BaseDeviceBindingResponse::~BaseDeviceBindingResponse()
{
}

tinyxml2::XMLElement* BaseDeviceBindingResponse::InitHeader(const std::string& tag)
{
	auto declare = _doc.NewDeclaration();
	_doc.InsertFirstChild(declare);

	/*Envelope*/
	auto envelopeNode = _doc.NewElement("s:Envelope");
	_doc.InsertEndChild(envelopeNode);
	envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
	envelopeNode->SetAttribute("xmlns:sc", "http://www.w3.org/2003/05/soap-encoding");
	envelopeNode->SetAttribute("xmlns:tt", "http://www.onvif.org/ver10/schema");
	envelopeNode->SetAttribute("xmlns:tds", "http://www.onvif.org/ver10/device/wsdl");
	/*header*/
	auto headerNode = _doc.NewElement("s:Header");
	envelopeNode->InsertEndChild(headerNode);
	/*body*/
	auto bodyNode = _doc.NewElement("s:Body");
	envelopeNode->InsertEndChild(bodyNode);

	auto respNode = _doc.NewElement(tag.c_str());
	bodyNode->InsertEndChild(respNode);
	return respNode;
}

/*_tds__GetScopesResponse*/
std::string _tds__GetScopesResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetScopesResponse");
	for (size_t i = 0; i < Scopes.size(); i++)
	{
		auto scopesNode = _doc.NewElement("tds:Scopes");
		auto scopeDefNode = _doc.NewElement("tt:ScopeDef");
		if (Scopes[i]->ScopeDef == tt__ScopeDefinition__Fixed)
			scopeDefNode->SetText("Fixed");
		else
			scopeDefNode->SetText("Configurable");

		auto scopeItemNode = _doc.NewElement("tt:ScopeItem");
		scopeItemNode->SetText(Scopes[i]->ScopeItem.c_str());
		scopesNode->InsertEndChild(scopeDefNode);
		scopesNode->InsertEndChild(scopeItemNode);
		respNode->InsertEndChild(scopesNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
/*_tds__GetDeviceInformationResponse*/
std::string _tds__GetDeviceInformationResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetDeviceInformationResponse");
	auto manufNode = _doc.NewElement("tds:Manufacturer");
	manufNode->SetText(Manufacturer.c_str());
	respNode->InsertEndChild(manufNode);
	auto modelNode = _doc.NewElement("tds:Model");
	modelNode->SetText(Model.c_str());
	respNode->InsertEndChild(modelNode);
	auto fwNode = _doc.NewElement("tds:FirmwareVersion");
	fwNode->SetText(FirmwareVersion.c_str());
	respNode->InsertEndChild(fwNode);
	auto snNode = _doc.NewElement("tds:SerialNumber");
	snNode->SetText(SerialNumber.c_str());
	respNode->InsertEndChild(snNode);
	auto hdNode = _doc.NewElement("tds:HardwareId");
	hdNode->SetText(HardwareId.c_str());
	respNode->InsertEndChild(hdNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetCapabilitiesResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetCapabilitiesResponse");
	auto capabilityNode = _doc.NewElement("tds:Capabilities");
	respNode->InsertEndChild(capabilityNode);
	if (Analytics)
	{
		auto analyticsNode = _doc.NewElement("tt:Analytics");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(Analytics->XAddr.c_str());
		auto ruleSupportNode = _doc.NewElement("tt:RuleSupport");
		ruleSupportNode->SetText(Analytics->RuleSuppor);
		auto modelSupportNode = _doc.NewElement("tt:AnalyticsModuleSupport");
		modelSupportNode->SetText(Analytics->AnalyticsModuleSupport);
		analyticsNode->InsertEndChild(addrNode);
		analyticsNode->InsertEndChild(ruleSupportNode);
		analyticsNode->InsertEndChild(modelSupportNode);
		capabilityNode->InsertEndChild(analyticsNode);
	}
	if (Device)
	{
		auto deviceNode = _doc.NewElement("tt:Device");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(Device->XAddr.c_str());
		deviceNode->InsertEndChild(addrNode);
		/*NetWork*/
		if (Device->Network)
		{
			auto netNode = _doc.NewElement("tt:Network");
			auto ipFilterNode = _doc.NewElement("tt:IPFilter");
			ipFilterNode->SetText(Device->Network->IPFilter);
			auto zeroCfgNode = _doc.NewElement("tt:ZeroConfiguration");
			zeroCfgNode->SetText(Device->Network->ZeroConfiguration);
			auto ipv6Node = _doc.NewElement("tt:IPVersion6");
			ipv6Node->SetText(Device->Network->IPVersion6);
			auto dnsNode = _doc.NewElement("tt:DynDNS");
			dnsNode->SetText(Device->Network->DynDNS);
			netNode->InsertEndChild(ipFilterNode);
			netNode->InsertEndChild(zeroCfgNode);
			netNode->InsertEndChild(ipv6Node);
			netNode->InsertEndChild(dnsNode);
			deviceNode->InsertEndChild(netNode);
		}
		/*System*/
		if (Device->System)
		{
			auto systemNode = _doc.NewElement("tt:System");
			auto resolveNode = _doc.NewElement("tt:DiscoveryResolve");
			resolveNode->SetText(Device->System->DiscoveryResolve);
			auto byeNode = _doc.NewElement("tt:DiscoveryBye");
			byeNode->SetText(Device->System->DiscoveryBye);
			auto discoveryNode = _doc.NewElement("tt:RemoteDiscovery");
			discoveryNode->SetText(Device->System->RemoteDiscovery);
			auto sysBackupNode = _doc.NewElement("tt:SystemBackup");
			sysBackupNode->SetText(Device->System->SystemBackup);
			auto sysLoggingNode = _doc.NewElement("tt:SystemLogging");
			sysLoggingNode->SetText(Device->System->SystemLogging);
			auto firmwareNode = _doc.NewElement("tt:FirmwareUpgrade");
			firmwareNode->SetText(Device->System->FirmwareUpgrade);
			auto verNode = _doc.NewElement("tt:SupportedVersions");
			auto majorNode = _doc.NewElement("tt:Major");
			auto minorNode = _doc.NewElement("tt:Minor");
			majorNode->SetText(1);
			minorNode->SetText(0);
			verNode->InsertEndChild(majorNode);
			verNode->InsertEndChild(minorNode);

			systemNode->InsertEndChild(resolveNode);
			systemNode->InsertEndChild(byeNode);
			systemNode->InsertEndChild(discoveryNode);
			systemNode->InsertEndChild(sysBackupNode);
			systemNode->InsertEndChild(sysLoggingNode);
			systemNode->InsertEndChild(firmwareNode);
			systemNode->InsertEndChild(verNode);
			deviceNode->InsertEndChild(systemNode);
		}
		/*IO*/
		if (Device->IO)
		{
			auto ioNode = _doc.NewElement("tt:IO");
			auto icNode = _doc.NewElement("tt:InputConnectors");
			icNode->SetText(Device->IO->InputConnectors);
			auto roNode = _doc.NewElement("tt:RelayOutputs");
			roNode->SetText(Device->IO->RelayOutputs);
			ioNode->InsertEndChild(icNode);
			ioNode->InsertEndChild(roNode);
			deviceNode->InsertEndChild(ioNode);
		}
		/*Security*/
		if (Device->Security)
		{
			auto securityNode = _doc.NewElement("tt:Security");
			auto t1Node = _doc.NewElement("tt:TLS1.1");
			auto t2Node = _doc.NewElement("tt:TLS1.2");
			auto okgNode = _doc.NewElement("tt:OnboardKeyGeneration");
			auto apcNode = _doc.NewElement("tt:AccessPolicyConfig");
			auto x5tNode = _doc.NewElement("tt:X.509Token");
			auto stNode = _doc.NewElement("tt:SAMLToken");
			auto ktNode = _doc.NewElement("tt:KerberosToken");
			auto rtNode = _doc.NewElement("tt:RELToken");

			t1Node->SetText(Device->Security->TLS1_x002e1);
			t2Node->SetText(Device->Security->TLS1_x002e2);
			okgNode->SetText(Device->Security->OnboardKeyGeneration);
			apcNode->SetText(Device->Security->AccessPolicyConfig);
			x5tNode->SetText(Device->Security->X_x002e509Token);
			stNode->SetText(Device->Security->SAMLToken);
			ktNode->SetText(Device->Security->KerberosToken);
			rtNode->SetText(Device->Security->RELToken);

			securityNode->InsertEndChild(t1Node);
			securityNode->InsertEndChild(t2Node);
			securityNode->InsertEndChild(okgNode);
			securityNode->InsertEndChild(apcNode);
			securityNode->InsertEndChild(x5tNode);
			securityNode->InsertEndChild(stNode);
			securityNode->InsertEndChild(ktNode);
			securityNode->InsertEndChild(rtNode);
			deviceNode->InsertEndChild(securityNode);
		}
		capabilityNode->InsertEndChild(deviceNode);
	}
	if (Events)
	{
		auto eventNode = _doc.NewElement("tt:Events");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(Events->XAddr.c_str());
		auto spsNode = _doc.NewElement("tt:WSSubscriptionPolicySupport");
		spsNode->SetText(Events->WSSubscriptionPolicySupport);
		auto ppsNode = _doc.NewElement("tt:WSPullPointSupport");
		ppsNode->SetText(Events->WSPullPointSupport);
		auto psmfsNode = _doc.NewElement("tt:WSPausableSubscriptionManagerInterfaceSupport");
		psmfsNode->SetText(Events->WSPausableSubscriptionManagerInterfaceSupport);

		eventNode->InsertEndChild(addrNode);
		eventNode->InsertEndChild(spsNode);
		eventNode->InsertEndChild(ppsNode);
		eventNode->InsertEndChild(psmfsNode);
		capabilityNode->InsertEndChild(eventNode);
	}
	if (Imaging)
	{
		auto imagingNode = _doc.NewElement("tt:Imaging");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(Imaging->XAddr.c_str());
		imagingNode->InsertEndChild(addrNode);
		capabilityNode->InsertEndChild(imagingNode);
	}
	if (Media)
	{
		auto mediaNode = _doc.NewElement("tt:Media");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(Media->XAddr.c_str());
		mediaNode->InsertEndChild(addrNode);
		if (Media->StreamingCapabilities)
		{
			auto streamCapabilityNode = _doc.NewElement("tt:StreamingCapabilities");
			auto multicastNode = _doc.NewElement("tt:RTPMulticast");
			multicastNode->SetText(Media->StreamingCapabilities->RTPMulticast);
			auto tcpNode = _doc.NewElement("tt:RTP_TCP");
			tcpNode->SetText(Media->StreamingCapabilities->RTP_USCORETCP);
			auto rtspTcpNode = _doc.NewElement("tt:RTP_RTSP_TCP");
			rtspTcpNode->SetText(Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP);
			streamCapabilityNode->InsertEndChild(multicastNode);
			streamCapabilityNode->InsertEndChild(tcpNode);
			streamCapabilityNode->InsertEndChild(rtspTcpNode);
			mediaNode->InsertEndChild(streamCapabilityNode);
		}
		capabilityNode->InsertEndChild(mediaNode);
	}
	if (PTZ)
	{
		auto ptzNode = _doc.NewElement("tt:PTZ");
		auto addrNode = _doc.NewElement("tt:XAddr");
		addrNode->SetText(PTZ->XAddr.c_str());
		ptzNode->InsertEndChild(addrNode);
		capabilityNode->InsertEndChild(ptzNode);
	}
	if (Extension)
	{
		auto extensionNode = _doc.NewElement("tt:Extension");
		if (Extension->DeviceIO)
		{
//			auto deviceIONode = _doc.NewElement("tt:DeviceIO");
			_doc.NewElement("tt:DeviceIO");
		}
		if (Extension->Display)
		{

		}
		if (Extension->Receiver)
		{

		}
		if (Extension->Recording)
		{
			auto recordingNode = _doc.NewElement("tt:Recording");
			auto addrNode = _doc.NewElement("tt:XAddr");
			addrNode->SetText(Extension->Recording->XAddr.c_str());
			auto rsNode = _doc.NewElement("tt:ReceiverSource");
			rsNode->SetText(Extension->Recording->ReceiverSource);
			auto mpsNode = _doc.NewElement("tt:MediaProfileSource");
			mpsNode->SetText(Extension->Recording->MediaProfileSource);
			auto drNode = _doc.NewElement("tt:DynamicRecordings");
			drNode->SetText(Extension->Recording->DynamicRecordings);
			auto dtNode = _doc.NewElement("tt:DynamicTracks");
			dtNode->SetText(Extension->Recording->DynamicTracks);
			auto mslNode = _doc.NewElement("tt:MaxStringLength");
			mslNode->SetText(Extension->Recording->MaxStringLength);

			recordingNode->InsertEndChild(addrNode);
			recordingNode->InsertEndChild(rsNode);
			recordingNode->InsertEndChild(mpsNode);
			recordingNode->InsertEndChild(drNode);
			recordingNode->InsertEndChild(dtNode);
			recordingNode->InsertEndChild(mslNode);
			extensionNode->InsertEndChild(recordingNode);
		}
		if (Extension->Replay)
		{
			auto replayNode = _doc.NewElement("tt:Replay");
			auto addrNode = _doc.NewElement("tt:XAddr");
			addrNode->SetText(Extension->Replay->XAddr.c_str());
			replayNode->InsertEndChild(addrNode);
			extensionNode->InsertEndChild(replayNode);
		}
		if (Extension->Search)
		{
			auto searchNode = _doc.NewElement("tt:Search");
			auto addrNode = _doc.NewElement("tt:XAddr");
			addrNode->SetText(Extension->Search->XAddr.c_str());
			auto msNode = _doc.NewElement("tt:MetadataSearch");
			msNode->SetText(Extension->Search->MetadataSearch);
			searchNode->InsertEndChild(addrNode);
			searchNode->InsertEndChild(msNode);

			extensionNode->InsertEndChild(searchNode);
		}
		capabilityNode->InsertEndChild(extensionNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetNetworkInterfacesResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetNetworkInterfacesResponse");
	for (auto& v : NetworkInterfaces)
	{
		auto ifNode = _doc.NewElement("tds:NetworkInterfaces");
		ifNode->SetAttribute("token", v->attrToken.c_str());
		auto ttEnableNode = _doc.NewElement("tt:Enabled");
		if (v->Enabled)
			ttEnableNode->SetText("true");
		else
			ttEnableNode->SetText("false");
		ifNode->InsertEndChild(ttEnableNode);
		if (v->Info)/*tt:Info*/
		{
			auto ttInfoNode = _doc.NewElement("tt:Info");
			auto nameNode = _doc.NewElement("tt:Name");
			nameNode->SetText(v->Info->Name.c_str());
			auto hwNode = _doc.NewElement("tt:HwAddress");
			hwNode->SetText(v->Info->HwAddress.c_str());
			auto mtuNode = _doc.NewElement("tt:MTU");
			mtuNode->SetText(v->Info->MTU);
			ttInfoNode->InsertEndChild(nameNode);
			ttInfoNode->InsertEndChild(hwNode);
			ttInfoNode->InsertEndChild(mtuNode);
			ifNode->InsertEndChild(ttInfoNode);
		}
		if (v->IPv4)/*tt:IPv4*/
		{
			auto ttIpv4Node = _doc.NewElement("tt:IPv4");
			auto teNode = _doc.NewElement("tt:Enabled");
			teNode->SetText(v->IPv4->Enabled);
			ttIpv4Node->InsertEndChild(teNode);
			if (v->IPv4->Config)
			{
				auto cfgNode = _doc.NewElement("tt:Config");
				ttIpv4Node->InsertEndChild(cfgNode);
				if (v->IPv4->Config->Manual)
				{
					auto manualNode = _doc.NewElement("tt:Manual");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv4->Config->Manual->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					preLenNode->SetText(v->IPv4->Config->Manual->second);
					manualNode->InsertEndChild(addressNode);
					manualNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(manualNode);
				}
				if (v->IPv4->Config->LinkLocal)
				{
					auto linkLocalNode = _doc.NewElement("tt:LinkLocal");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv4->Config->LinkLocal->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					preLenNode->SetText(v->IPv4->Config->LinkLocal->second);
					linkLocalNode->InsertEndChild(addressNode);
					linkLocalNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(linkLocalNode);
				}
				if (v->IPv4->Config->FromDHCP)
				{
					auto fromDHCPNode = _doc.NewElement("tt:FromDHCP");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv4->Config->FromDHCP->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					preLenNode->SetText(v->IPv4->Config->FromDHCP->second);
					fromDHCPNode->InsertEndChild(addressNode);
					fromDHCPNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(fromDHCPNode);
				}
				auto dhcpNode = _doc.NewElement("tt:DHCP");
				dhcpNode->SetText(v->IPv4->Config->DHCP);
				cfgNode->InsertEndChild(dhcpNode);
			}
			ifNode->InsertEndChild(ttIpv4Node);
		}
		if (v->IPv6)/*tt:IPv6*/
		{
			auto ttIpv6Node = _doc.NewElement("tt:IPv6");
			auto teNode = _doc.NewElement("tt:Enabled");
			teNode->SetText(v->IPv4->Enabled);
			ifNode->InsertEndChild(teNode);
			if (v->IPv6->Config)
			{
				auto cfgNode = _doc.NewElement("tt:Config");
				ttIpv6Node->InsertEndChild(cfgNode);

				auto araNode = _doc.NewElement("tt:AcceptRouterAdvert");
				araNode->SetText(v->IPv6->Config->AcceptRouterAdvert);
				cfgNode->InsertEndChild(araNode);

				auto dhcpNode = _doc.NewElement("tt:DHCP");
				dhcpNode->SetText(v->IPv6->Config->DHCPConiguration.c_str());
				cfgNode->InsertEndChild(dhcpNode);

				if (v->IPv6->Config->Manual)
				{
					auto manualNode = _doc.NewElement("tt:Manual");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv6->Config->Manual->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					addressNode->SetText(v->IPv6->Config->Manual->second);
					manualNode->InsertEndChild(addressNode);
					manualNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(manualNode);
				}
				if (v->IPv6->Config->FromDHCP)
				{
					auto fromDHCPNode = _doc.NewElement("tt:FromDHCP");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv6->Config->FromDHCP->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					addressNode->SetText(v->IPv6->Config->FromDHCP->second);
					fromDHCPNode->InsertEndChild(addressNode);
					fromDHCPNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(fromDHCPNode);
				}
				if (v->IPv6->Config->LinkLocal)
				{
					auto linkLocalNode = _doc.NewElement("tt:LinkLocal");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv6->Config->LinkLocal->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					addressNode->SetText(v->IPv6->Config->LinkLocal->second);
					linkLocalNode->InsertEndChild(addressNode);
					linkLocalNode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(linkLocalNode);
				}
				if (v->IPv6->Config->FromRA)
				{
					auto fromRANode = _doc.NewElement("tt:FromRA");
					auto addressNode = _doc.NewElement("tt:Address");
					addressNode->SetText(v->IPv6->Config->FromRA->first.c_str());
					auto preLenNode = _doc.NewElement("tt:PrefixLength");
					addressNode->SetText(v->IPv6->Config->FromRA->second);
					fromRANode->InsertEndChild(addressNode);
					fromRANode->InsertEndChild(preLenNode);
					cfgNode->InsertEndChild(fromRANode);
				}
			}
			ifNode->InsertEndChild(ttIpv6Node);
		}
		respNode->InsertEndChild(ifNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetHostnameResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetHostnameResponse");
	auto infoNode = _doc.NewElement("tds:HostnameInformation");
	auto dhcpNode = _doc.NewElement("tt:FromDHCP");
	dhcpNode->SetText(FromDHCP);
	auto nameNode = _doc.NewElement("tt:Name");
	nameNode->SetText(Name.c_str());
	infoNode->InsertEndChild(dhcpNode);
	infoNode->InsertEndChild(nameNode);
	respNode->InsertEndChild(infoNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetSystemDateAndTimeResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetSystemDateAndTimeResponse");
	auto dateNode = _doc.NewElement("tds:SystemDateAndTime");
	respNode->InsertEndChild(dateNode);
	auto dateTypeNode = _doc.NewElement("tt:DateTimeType");
	dateTypeNode->SetText(SetDateTimeType.c_str());
	dateNode->InsertEndChild(dateTypeNode);

	auto daylightNode = _doc.NewElement("tt:DaylightSavings");
	daylightNode->SetText(DaylightSavings);
	dateNode->InsertEndChild(daylightNode);

	auto timeZoneNode = _doc.NewElement("tt:TimeZone");
	auto tzNode = _doc.NewElement("tt:TZ");
	tzNode->SetText(tz.c_str());
	timeZoneNode->InsertEndChild(tzNode);
	dateNode->InsertEndChild(timeZoneNode);
	/*UTCDate*/
	auto utcNode = _doc.NewElement("tt:UTCDateTime");
	auto uTimeNode = _doc.NewElement("tt:Time");
	auto uHourNode = _doc.NewElement("tt:Hour");
	uHourNode->SetText(UTCDateTime.Hour);
	auto uMinuteNode = _doc.NewElement("tt:Minute");
	uMinuteNode->SetText(UTCDateTime.Minute);
	auto uSecondNode = _doc.NewElement("tt:Second");
	uSecondNode->SetText(UTCDateTime.Second);
	uTimeNode->InsertEndChild(uHourNode);
	uTimeNode->InsertEndChild(uMinuteNode);
	uTimeNode->InsertEndChild(uSecondNode);
	auto uDateNode = _doc.NewElement("tt:Date");
	auto uYearNode = _doc.NewElement("tt:Year");
	uYearNode->SetText(UTCDateTime.Year);
	auto uMonthNode = _doc.NewElement("tt:Month");
	uMonthNode->SetText(UTCDateTime.Month);
	auto uDayNode = _doc.NewElement("tt:Day");
	uDayNode->SetText(UTCDateTime.Day);
	uDateNode->InsertEndChild(uYearNode);
	uDateNode->InsertEndChild(uMonthNode);
	uDateNode->InsertEndChild(uDayNode);
	utcNode->InsertEndChild(uTimeNode);
	utcNode->InsertEndChild(uDateNode);
	dateNode->InsertEndChild(utcNode);
	/*LocalDateTime*/
	auto localNode = _doc.NewElement("tt:LocalDateTime");
	auto lTimeNode = _doc.NewElement("tt:Time");
	auto lHourNode = _doc.NewElement("tt:Hour");
	lHourNode->SetText(LocalDateTime.Hour);
	auto lMinuteNode = _doc.NewElement("tt:Minute");
	lMinuteNode->SetText(LocalDateTime.Minute);
	auto lSecondNode = _doc.NewElement("tt:Second");
	lSecondNode->SetText(LocalDateTime.Second);
	lTimeNode->InsertEndChild(lHourNode);
	lTimeNode->InsertEndChild(lMinuteNode);
	lTimeNode->InsertEndChild(lSecondNode);
	auto lDateNode = _doc.NewElement("tt:Date");
	auto lYearNode = _doc.NewElement("tt:Year");
	lYearNode->SetText(LocalDateTime.Year);
	auto lMonthNode = _doc.NewElement("tt:Month");
	lMonthNode->SetText(LocalDateTime.Month);
	auto lDayNode = _doc.NewElement("tt:Day");
	lDayNode->SetText(LocalDateTime.Day);
	lDateNode->InsertEndChild(lYearNode);
	lDateNode->InsertEndChild(lMonthNode);
	lDateNode->InsertEndChild(lDayNode);
	localNode->InsertEndChild(lTimeNode);
	localNode->InsertEndChild(lDateNode);
	dateNode->InsertEndChild(localNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetServicesResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetServicesResponse");

	for (size_t i = 0; i < Service.size(); i++)
	{
		auto serviceNode = _doc.NewElement("tds:Service");
		auto nsNode = _doc.NewElement("tds:Namespace");
		nsNode->SetText(Service[i]->Namespace.c_str());
		auto xaddrNode = _doc.NewElement("tds:XAddr");
		xaddrNode->SetText(Service[i]->XAddr.c_str());

		/*tds:Capabilities*/
		/*tds:Version*/
		auto verNode = _doc.NewElement("tds:Version");
		auto majorNode = _doc.NewElement("tt:Major");
		majorNode->SetText(Service[i]->Major);
		auto minorNode = _doc.NewElement("tt:Minor");
		minorNode->SetText(Service[i]->Minor);
		verNode->InsertEndChild(majorNode);
		verNode->InsertEndChild(minorNode);

		serviceNode->InsertEndChild(nsNode);
		serviceNode->InsertEndChild(xaddrNode);
		serviceNode->InsertEndChild(verNode);
		respNode->InsertEndChild(serviceNode);
	}

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetDNSResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetDNSResponse");
	auto infoNode = _doc.NewElement("tds:DNSInformation");
	respNode->InsertEndChild(infoNode);
	auto dhcpNode = _doc.NewElement("tt:FromDHCP");
	dhcpNode->SetText(FromDHCP);
	auto domainNode = _doc.NewElement("tt:SearchDomain");
	domainNode->SetText(SearchDomain.c_str());
	infoNode->InsertEndChild(dhcpNode);
	infoNode->InsertEndChild(domainNode);
	for (size_t i = 0; i < DNSFromDHCP.size(); i++)
	{
		auto dnsNode = _doc.NewElement("tt:DNSFromDHCP");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(DNSFromDHCP[i]->Type.c_str());
		dnsNode->InsertEndChild(typeNode);
		if (DNSFromDHCP[i]->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(DNSFromDHCP[i]->IPv4Address->c_str());
			dnsNode->InsertEndChild(ipv4Node);
		}
		if (DNSFromDHCP[i]->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv6Address");
			ipv6Node->SetText(DNSFromDHCP[i]->IPv6Address->c_str());
			dnsNode->InsertEndChild(ipv6Node);
		}
		infoNode->InsertEndChild(dnsNode);
	}
	for (size_t i = 0; i < DNSManual.size(); i++)
	{
		auto dnsNode = _doc.NewElement("tt:DNSManual");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(DNSManual[i]->Type.c_str());
		dnsNode->InsertEndChild(typeNode);
		if (DNSManual[i]->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(DNSManual[i]->IPv4Address->c_str());
			dnsNode->InsertEndChild(ipv4Node);
		}
		if (DNSManual[i]->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv6Address");
			ipv6Node->SetText(DNSManual[i]->IPv6Address->c_str());
			dnsNode->InsertEndChild(ipv6Node);
		}
		infoNode->InsertEndChild(dnsNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tds__GetNetworkProtocolsResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetNetworkProtocolsResponse");
	for (size_t i = 0; i < NetworkProtocols.size(); i++)
	{
		auto protocolNode = _doc.NewElement("tds:NetworkProtocols");
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(NetworkProtocols[i]->Name.c_str());
		auto enableNode = _doc.NewElement("tt:Enabled");
		enableNode->SetText(NetworkProtocols[i]->Enabled);
		auto portNode = _doc.NewElement("tt:Port");
		portNode->SetText(NetworkProtocols[i]->Port);
		protocolNode->InsertEndChild(nNode);
		protocolNode->InsertEndChild(enableNode);
		protocolNode->InsertEndChild(portNode);
		respNode->InsertEndChild(protocolNode);
	} 

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tds__GetNTPResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetNTPResponse");
	auto infoNode = _doc.NewElement("tds:NTPInformation");
	auto dhcpNode = _doc.NewElement("tt:FromDHCP");
	dhcpNode->SetText(FromDHCP);
	infoNode->InsertEndChild(dhcpNode);
	respNode->InsertEndChild(infoNode);
	if (NTPFromDHCP)
	{
		auto maualNode = _doc.NewElement("tt:NTPFromDHCP");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(NTPFromDHCP->Type.c_str());
		maualNode->InsertEndChild(typeNode);
		if (NTPFromDHCP->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(NTPFromDHCP->IPv4Address->c_str());
			maualNode->InsertEndChild(ipv4Node);
		}
		if (NTPFromDHCP->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv6Address");
			ipv6Node->SetText(NTPFromDHCP->IPv6Address->c_str());
			maualNode->InsertEndChild(ipv6Node);
		}
		if (NTPFromDHCP->DNSname)
		{
			auto dnNode = _doc.NewElement("tt:DNSname");
			dnNode->SetText(NTPFromDHCP->DNSname->c_str());
			maualNode->InsertEndChild(dnNode);
		}
		infoNode->InsertEndChild(maualNode);
	}
	if (NTPManual)
	{
		auto maualNode = _doc.NewElement("tt:NTPManual");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(NTPManual->Type.c_str());
		maualNode->InsertEndChild(typeNode);
		if (NTPManual->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(NTPManual->IPv4Address->c_str());
			maualNode->InsertEndChild(ipv4Node);
		}
		if (NTPManual->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv6Address");
			ipv6Node->SetText(NTPManual->IPv6Address->c_str());
			maualNode->InsertEndChild(ipv6Node);
		}
		if (NTPManual->DNSname)
		{
			auto dnNode = _doc.NewElement("tt:DNSname");
			dnNode->SetText(NTPManual->DNSname->c_str());
			maualNode->InsertEndChild(dnNode);
		}
		infoNode->InsertEndChild(maualNode);
	}
	
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tds__GetNetworkDefaultGatewayResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetNetworkDefaultGatewayResponse");
	auto gateWayNode = _doc.NewElement("tds:NetworkGateway");
	auto ipv4Node = _doc.NewElement("tt:IPv4Address");
	ipv4Node->SetText(IPv4Address.c_str());
	gateWayNode->InsertEndChild(ipv4Node);
	respNode->InsertEndChild(gateWayNode);
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tds__GetUsersResponse::Serialization()
{
	auto respNode = InitHeader("tds:GetUsersResponse");
	for (size_t i = 0; i < User.size(); i++)
	{
		auto userNode = _doc.NewElement("tds:User");
		auto nNode = _doc.NewElement("tt:Username");
		nNode->SetText(User[i]->Username.c_str());
		userNode->InsertEndChild(nNode);
		if (!User[i]->Password.empty())
		{
			auto pwdNode = _doc.NewElement("tt:Password");
			pwdNode->SetText(User[i]->Password.c_str());
			userNode->InsertEndChild(pwdNode);
		}
		auto levelNode = _doc.NewElement("tt:UserLevel");
		levelNode->SetText(User[i]->UserLevel.c_str());
		userNode->InsertEndChild(levelNode);
		respNode->InsertEndChild(userNode);
	}
	
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
soapDeviceBinding::soapDeviceBinding()
{
}

soapDeviceBinding::~soapDeviceBinding()
{
}

int soapDeviceBinding::OnGetScopes(void* req, std::string& body)
{
	_tds__GetScopesResponse resp;
	for (auto& i : EPSoap::Instance().vScopes)
	{
		std::shared_ptr<tt__Scope> item(new tt__Scope);
		item->ScopeDef = tt__ScopeDefinition__Fixed;
		item->ScopeItem = i;
		resp.Scopes.emplace_back(std::move(item));
	}
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetDeviceInformation(void* req, std::string& body)
{

    Emx::DevInfo devInfo;
    Emx::ErrCodeE e = devInfo.Load();
    if (e != Emx::ErrCodeE::Success) {
        return -1;
    }

    Emx::Sn sn;
    e = sn.Load();
    if (e != Emx::ErrCodeE::Success) {
        return-1;
    }

	_tds__GetDeviceInformationResponse resp;
	resp.FirmwareVersion = devInfo.GetHardware()->version;
	resp.HardwareId = devInfo.GetHardware()->type;
	resp.Manufacturer = "Epail";
	resp.Model = "IPC";
	resp.SerialNumber = sn.Get();
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetDNS(void* req, std::string& body)
{
	_tds__GetDNSResponse resp;
	resp.FromDHCP = false;
	resp.SearchDomain = "epail";
	std::shared_ptr<tt__IPAddress> ipv4(new tt__IPAddress);
	ipv4->Type = "IPv4";
	ipv4->IPv4Address = std::make_shared<std::string>("8.8.8.8");
	resp.DNSManual.emplace_back(std::move(ipv4));

	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnSetDNS(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string type;
	std::string address;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		
		for (auto i = child->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "DNSManual"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
					if (strstr(j->Value(), "Type"))
						type = j->GetText();
					if (strstr(j->Value(), "IPv4Address"))
						address = j->GetText();
					if (strstr(j->Value(), "IPv6Address"))
						address = j->GetText();
				}
			}
		}
	}
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">");
	body.append("<s:Header/><s:Body><tds:SetDNSResponse/></s:Body></s:Envelope>\n");
	return 200;
}
int soapDeviceBinding::OnGetCapabilities(void* req, std::string& body)
{
	tt__CapabilityCategory Category = tt__CapabilityCategory__All;
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
		{
			child = child->NextSiblingElement();//Body
		}
		
		auto dataNode = child->FirstChildElement()->FirstChildElement();
		if (dataNode && strstr(dataNode->Value(), "Category"))
		{
			auto t = dataNode->GetText();
			if (!strcmp("Analytics", t))
				Category = tt__CapabilityCategory__Analytics;
			else if (!strcmp("Device", t))
				Category = tt__CapabilityCategory__Device;
			else if (!strcmp("Events", t))
				Category = tt__CapabilityCategory__Events;
			else if (!strcmp("Imaging", t))
				Category = tt__CapabilityCategory__Imaging;
			else if (!strcmp("Media", t))
				Category = tt__CapabilityCategory__Media;
			else if (!strcmp("PTZ", t))
				Category = tt__CapabilityCategory__PTZ;
		}
	}
	_tds__GetCapabilitiesResponse resp;
	if ((Category == tt__CapabilityCategory__All) || (Category == tt__CapabilityCategory__Device))
	{
		resp.Device = std::make_shared<tt__DeviceCapabilities>();
		resp.Device->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/device/wsdl"];
		resp.Device->System = std::make_shared<tt__SystemCapabilities>();
		resp.Device->System->DiscoveryBye = false;
		resp.Device->System->DiscoveryResolve = false;
		resp.Device->System->FirmwareUpgrade = false;
		resp.Device->System->RemoteDiscovery = true;
		resp.Device->System->SystemBackup = false;
		resp.Device->System->SystemLogging = false;
		resp.Device->System->Major = 1;
		resp.Device->System->Minor = 0;
		//resp.Device->IO = std::make_shared<tt__IOCapabilities>();
		resp.Device->Network = std::make_shared<tt__NetworkCapabilities>();
		resp.Device->Network->IPFilter = false;
		resp.Device->Network->DynDNS = false;
		resp.Device->Network->IPVersion6 = true;
		resp.Device->Network->ZeroConfiguration = false;
		resp.Device->Security = std::make_shared<tt__SecurityCapabilities>();
	}
	if ((Category == tt__CapabilityCategory__All) || (Category == tt__CapabilityCategory__Events))
	{
		/*resp.Events = std::make_shared<tt__EventCapabilities>();
		resp.Events->WSPausableSubscriptionManagerInterfaceSupport = false;
		resp.Events->WSPullPointSupport = true;
		resp.Events->WSSubscriptionPolicySupport = false;
		resp.Events->XAddr= EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/events/wsdl"];*/
	}
	if ((Category == tt__CapabilityCategory__All) || (Category == tt__CapabilityCategory__Imaging))
	{
		resp.Imaging = std::make_shared<tt__ImagingCapabilities>();
		resp.Imaging->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver20/imaging/wsdl"];
	}
	if ((Category == tt__CapabilityCategory__All) || (Category == tt__CapabilityCategory__PTZ))
	{
		resp.PTZ = std::make_shared<tt__PTZCapabilities>();
		resp.PTZ->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver20/ptz/wsdl"];
	}
	if ((Category == tt__CapabilityCategory__All) || (Category == tt__CapabilityCategory__Media))
	{
		resp.Media = std::make_shared<tt__MediaCapabilities>();
		resp.Media->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/media/wsdl"];
		resp.Media->StreamingCapabilities = std::make_shared<tt__RealTimeStreamingCapabilities>();
		resp.Media->StreamingCapabilities->RTPMulticast = false;
		resp.Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = true;
		resp.Media->StreamingCapabilities->RTP_USCORETCP = true;
	}
	if (Category == tt__CapabilityCategory__All)
	{
		/*resp.Extension = std::make_shared<tt__CapabilitiesExtension>();
		resp.Extension->Recording = std::make_shared<tt__RecordingCapabilities>();
		resp.Extension->Recording->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/recording/wsdl"];
		resp.Extension->Recording->DynamicRecordings = false;
		resp.Extension->Recording->DynamicTracks = false;
		resp.Extension->Recording->MediaProfileSource = true;
		resp.Extension->Recording->ReceiverSource = false;
		resp.Extension->Recording->MaxStringLength = 0;
		resp.Extension->Search = std::make_shared<tt__SearchCapabilities>();
		resp.Extension->Search->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/search/wsdl"];
		resp.Extension->Search->MetadataSearch = false;
		resp.Extension->Replay = std::make_shared<tt__ReplayCapabilities>();
		resp.Extension->Replay->XAddr = EPSoap::Instance().nsXaddrMap["http://www.onvif.org/ver10/replay/wsdl"];*/
	}

	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetNetworkInterfaces(void* req, std::string& body)
{
	_tds__GetNetworkInterfacesResponse resp;
	for (auto& v : EPSoap::Instance().ifMap)
	{
		std::shared_ptr<tt__NetworkInterface> netInterFace(new tt__NetworkInterface());
		netInterFace->Enabled = v.second.enable;
		netInterFace->attrToken = v.first;
		netInterFace->Info = std::make_shared<tt__NetworkInterfaceInfo>();
		netInterFace->Info->HwAddress = v.second.mac;
		netInterFace->Info->Name = v.first;
		netInterFace->Info->MTU = v.second.mtu;
		if (v.second.ipv4Enable)
		{
			netInterFace->IPv4 = std::make_shared<tt__IPv4NetworkInterface>();
			netInterFace->IPv4->Enabled = v.second.ipv4Enable;
			netInterFace->IPv4->Config = std::make_shared<tt__IPv4Configuration>();
			netInterFace->IPv4->Config->DHCP = v.second.dhcp;
			if (v.second.dhcp)
			{
				netInterFace->IPv4->Config->FromDHCP = std::make_shared<std::pair<std::string, int>>();
				netInterFace->IPv4->Config->FromDHCP->first = v.second.ipv4;
				netInterFace->IPv4->Config->FromDHCP->second = v.second.ipv4Prefixlen;
			}
		}
		resp.NetworkInterfaces.emplace_back(std::move(netInterFace));
	}
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetHostname(void* req, std::string& body)
{
	char name[64];
	gethostname(name, 64);
	_tds__GetHostnameResponse resp;
	resp.FromDHCP = false;
	resp.Name = name;
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnSystemReboot(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">");
	body.append("<s:Header/><s:Body><tds:SystemRebootResponse/></s:Body></s:Envelope>\n");

	return 200;
}

int soapDeviceBinding::OnGetSystemDateAndTime(void* req, std::string& body)
{
	_tds__GetSystemDateAndTimeResponse resp;
	resp.SetDateTimeType = "NTP";
	resp.DaylightSavings = false;
#ifdef _WIN32
	SYSTEMTIME t;
	GetLocalTime(&t);
	resp.LocalDateTime.Year = t.wYear;
	resp.LocalDateTime.Month = t.wMonth;
	resp.LocalDateTime.Day = t.wDay;
	resp.LocalDateTime.Hour = t.wHour;
	resp.LocalDateTime.Minute = t.wMinute;
	resp.LocalDateTime.Second = t.wMinute;
	GetSystemTime(&t);
	resp.UTCDateTime.Year = t.wYear;
	resp.UTCDateTime.Month = t.wMonth;
	resp.UTCDateTime.Day = t.wDay;
	resp.UTCDateTime.Hour = t.wHour;
	resp.UTCDateTime.Minute = t.wMinute;
	resp.UTCDateTime.Second = t.wSecond;
#else
	auto r = time(NULL);
	auto t1 = localtime(&r);
	resp.LocalDateTime.Year = t1->tm_year + 1900;
	resp.LocalDateTime.Month = t1->tm_mon + 1;
	resp.LocalDateTime.Day = t1->tm_mday;
	resp.LocalDateTime.Hour = t1->tm_hour;
	resp.LocalDateTime.Minute = t1->tm_min;
	resp.LocalDateTime.Second = t1->tm_sec;
	auto t2 = gmtime(&r);
	resp.UTCDateTime.Year = t2->tm_year + 1900;
	resp.UTCDateTime.Month = t2->tm_mon + 1;
	resp.UTCDateTime.Day = t2->tm_mday;
	resp.UTCDateTime.Hour = t2->tm_hour;
	resp.UTCDateTime.Minute = t2->tm_min;
	resp.UTCDateTime.Second = t2->tm_sec;
#endif 
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnSetSystemDateAndTime(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
	
		_tds__SetSystemDateAndTime d;
		auto dateNode = child->FirstChildElement();
		for (auto i = dateNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "DateTimeType"))
				d.DateTimeType = i->GetText();
			else if (strstr(i->Value(), "DaylightSavings"))
				d.DaylightSavings = i->BoolText();
			else if (strstr(i->Value(), "TimeZone"))
			{
				auto tzNode = i->FirstChildElement();
				d.TZ = tzNode->GetText();
			}
			else if (strstr(i->Value(), "UTCDateTime"))
			{
				d.UTCDateTime = std::make_shared<tt__DateTime>();
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
					if (strstr(j->Value(), "Time"))
					{
						for (auto k = j->FirstChildElement(); k; k = k->NextSiblingElement())
						{
							if (strstr(k->Value(), "Hour"))
								d.UTCDateTime->Hour = k->IntText();
							else if (strstr(k->Value(), "Minute"))
								d.UTCDateTime->Minute = k->IntText();
							else if (strstr(k->Value(), "Second"))
								d.UTCDateTime->Second = k->IntText();
						}
					}
					else if (strstr(j->Value(), "Date"))
					{
						for (auto k = j->FirstChildElement(); k; k = k->NextSiblingElement())
						{
							if (strstr(k->Value(), "Year"))
								d.UTCDateTime->Year = k->IntText();
							else if (strstr(k->Value(), "Month"))
								d.UTCDateTime->Month = k->IntText();
							else if (strstr(k->Value(), "Day"))
								d.UTCDateTime->Day = k->IntText();
						}
					}
				}
			}//UTCDATE
		}
		/*set time*/
		//int a = 2;
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">");
	body.append("<s:Header/><s:Body><tds:SetSystemDateAndTimeResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapDeviceBinding::OnGetServices(void* req, std::string& body)
{
	_tds__GetServicesResponse resp;
	for (auto& v : EPSoap::Instance().nsXaddrMap)
	{
		std::shared_ptr<tds__Service> serviceItem(new tds__Service);
		serviceItem->Namespace = v.first;
		serviceItem->XAddr = v.second;
		serviceItem->Major = 1;
		serviceItem->Minor = 0;
		resp.Service.emplace_back(std::move(serviceItem));
	}
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetDiscoveryMode(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">");
	body.append("<s:Header/><s:Body><tds:GetDiscoveryModeResponse><tds:DiscoveryMode>Discoverable</tds:DiscoveryMode></tds:GetDiscoveryModeResponse></s:Body></s:Envelope>\n");
	return 200;
}
int soapDeviceBinding::OnGetNetworkProtocols(void* req, std::string& body)
{
	_tds__GetNetworkProtocolsResponse resp;
	std::shared_ptr<tt__NetworkProtocol> httpProtocol(new tt__NetworkProtocol);
	httpProtocol->Name = "HTTP";
	httpProtocol->Enabled = true;
	httpProtocol->Port = 80;
	std::shared_ptr<tt__NetworkProtocol> rtspProtocol(new tt__NetworkProtocol);
	rtspProtocol->Name = "RTSP";
	rtspProtocol->Enabled = true;
	rtspProtocol->Port = 554;
	resp.NetworkProtocols.emplace_back(httpProtocol);
	resp.NetworkProtocols.emplace_back(rtspProtocol);
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetNTP(void* req, std::string& body)
{
	_tds__GetNTPResponse resp;
	resp.FromDHCP = false;
	resp.NTPManual = std::make_shared<tt__NetworkHost>();
	resp.NTPManual->Type = "DNS";
	resp.NTPManual->DNSname = std::make_shared<std::string>("clock.isc.org");
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetNetworkDefaultGateway(void* req, std::string& body)
{
	_tds__GetNetworkDefaultGatewayResponse resp;
	resp.IPv4Address = "192.168.0.1";
	FILE* fp = fopen("/proc/net/route", "r");
	if (fp)
	{
		char tmp[256];
		char iface[16];
		unsigned long dest_addr, gate_addr;
		/* Skip title line */
		fgets(tmp, sizeof(tmp), fp);
		while (fgets(tmp, sizeof(tmp), fp)) {
			if (sscanf(tmp, "%s\t%lX\t%lX", iface, &dest_addr, &gate_addr) != 3 || dest_addr != 0)
				continue;
			char ip[24];
			sockaddr_in sa;
			sa.sin_addr.s_addr = gate_addr;
			inet_ntop(AF_INET, &sa.sin_addr, ip, 32);
			resp.IPv4Address = ip;
			break;
		}
	}
	
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnGetUsers(void* req, std::string& body)
{
	_tds__GetUsersResponse resp;
	std::shared_ptr<tt__User> user(new tt__User);
	user->Username = "admin";
	user->UserLevel = "Administrator";
	resp.User.emplace_back(std::move(user));
	body = std::move(resp.Serialization());
	return 200;
}

int soapDeviceBinding::OnSetUsers(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string userName;
	std::string pwd;
	std::string level;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		for (auto i = child->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "SetUser"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
					if (strstr(j->Value(), "Username"))
						userName = j->GetText();
					else if (strstr(j->Value(), "Password"))
						pwd = j->GetText();
					else if (strstr(j->Value(), "UserLevel"))
						level = j->GetText();
				}
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">");
	body.append("<s:Header/><s:Body><tds:SetUserResponse/></s:Body></s:Envelope>\n");
	return 200;
}