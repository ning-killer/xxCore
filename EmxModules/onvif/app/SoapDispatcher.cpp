#include "SoapDispatcher.h"
#include "soapDeviceBinding.h"
#include "soapMedia.h"
#include "soapImaging.h"
#include "soapPTZ.h"
//#include "soapSearch.h"
//#include "soapReplay.h"
//#include "soapEvent.h"
//#include "soapRecording.h"

SoapDispatcher::SoapDispatcher()
{}

SoapDispatcher::~SoapDispatcher()
{}

SoapDispatcher& SoapDispatcher::Instance()
{
	static std::shared_ptr<SoapDispatcher> g_instance(new SoapDispatcher);
	static SoapDispatcher& g_instance_ref = *g_instance;
	return g_instance_ref;
}

void SoapDispatcher::Init()
{
	/*/onvif/device_service*/
	std::shared_ptr<soapDeviceBinding> sdb(new soapDeviceBinding);
	auto& a = mapFunc["/onvif/device_service"];
	a.insert({ "GetScopes", std::bind(&soapDeviceBinding::OnGetScopes, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetDeviceInformation", std::bind(&soapDeviceBinding::OnGetDeviceInformation, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetDNS", std::bind(&soapDeviceBinding::OnGetDNS, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "SetDNS", std::bind(&soapDeviceBinding::OnSetDNS, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetCapabilities", std::bind(&soapDeviceBinding::OnGetCapabilities, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetNetworkInterfaces", std::bind(&soapDeviceBinding::OnGetNetworkInterfaces, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetHostname", std::bind(&soapDeviceBinding::OnGetHostname, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "SystemReboot", std::bind(&soapDeviceBinding::OnSystemReboot, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetSystemDateAndTime", std::bind(&soapDeviceBinding::OnGetSystemDateAndTime, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "SetSystemDateAndTime", std::bind(&soapDeviceBinding::OnSetSystemDateAndTime, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetServices", std::bind(&soapDeviceBinding::OnGetServices, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetDiscoveryMode", std::bind(&soapDeviceBinding::OnGetDiscoveryMode, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetNetworkProtocols", std::bind(&soapDeviceBinding::OnGetNetworkProtocols, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetNTP", std::bind(&soapDeviceBinding::OnGetNTP, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetNetworkDefaultGateway", std::bind(&soapDeviceBinding::OnGetNetworkDefaultGateway, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "GetUsers", std::bind(&soapDeviceBinding::OnGetUsers, sdb, std::placeholders::_1,std::placeholders::_2) });
	a.insert({ "SetUsers", std::bind(&soapDeviceBinding::OnSetUsers, sdb, std::placeholders::_1,std::placeholders::_2) });
	/*media*/
	std::shared_ptr<soapMedia> sm(new soapMedia);
	auto& b = mapFunc["/onvif/media_service"];
    b.insert({ "GetServiceCapabilities", std::bind(&soapMedia::OnGetServiceCapabilities, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetProfiles", std::bind(&soapMedia::OnGetProfiles, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetProfile", std::bind(&soapMedia::OnGetProfile, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetStreamUri", std::bind(&soapMedia::OnGetStreamUri, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetOSD", std::bind(&soapMedia::OnGetOSD, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "SetOSD", std::bind(&soapMedia::OnSetOSD, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetVideoSources", std::bind(&soapMedia::OnGetVideoSources, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetAudioSources", std::bind(&soapMedia::OnGetAudioSources, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetSnapshotUri", std::bind(&soapMedia::OnGetSnapshotUri, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetVideoSourceConfiguration", std::bind(&soapMedia::OnGetVideoSources, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetVideoEncoderConfigurationOptions", std::bind(&soapMedia::OnGetVideoEncoderConfigurationOptions, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "AddPTZConfiguration", std::bind(&soapMedia::OnAddPTZConfiguration, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "SetVideoEncoderConfiguration", std::bind(&soapMedia::OnSetVideoEncoderConfiguration, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetVideoEncoderConfigurations", std::bind(&soapMedia::OnGetVideoEncoderConfigurations, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetVideoEncoderConfiguration", std::bind(&soapMedia::OnGetVideoEncoderConfiguration, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetSnapshot", std::bind(&soapMedia::OnGetSnapshot, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetMetadataConfigurationOptions", std::bind(&soapMedia::OnGetMetadataConfigurationOptions, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetMetadataConfiguration", std::bind(&soapMedia::OnGetMetadataConfiguration, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetAudioEncoderConfigurationOptions", std::bind(&soapMedia::OnGetAudioEncoderConfigurationOptions, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetAudioEncoderConfiguration", std::bind(&soapMedia::OnGetAudioEncoderConfiguration, sm, std::placeholders::_1,std::placeholders::_2) });
    b.insert({ "GetAudioEncoderConfigurations", std::bind(&soapMedia::OnGetAudioEncoderConfigurations, sm, std::placeholders::_1,std::placeholders::_2) });
	b.insert({ "GetOSDOptions", std::bind(&soapMedia::OnGetOSDOptions, sm, std::placeholders::_1,std::placeholders::_2) });
	b.insert({ "SetSynchronizationPoint", std::bind(&soapMedia::OnSetSynchronizationPoint, sm, std::placeholders::_1,std::placeholders::_2) });

	std::shared_ptr<soapImaging> si(new soapImaging);
	auto& c = mapFunc["/onvif/imaging_service"];
	c.insert({ "GetMoveOptions",std::bind(&soapImaging::OnGetMoveOptions,si,std::placeholders::_1,std::placeholders::_2) });
	c.insert({ "GetImagingSettings",std::bind(&soapImaging::OnGetImagingSettings,si,std::placeholders::_1,std::placeholders::_2) });
	c.insert({ "SetImagingSettings",std::bind(&soapImaging::OnSetImagingSettings,si,std::placeholders::_1,std::placeholders::_2) });
	c.insert({ "GetOptions",std::bind(&soapImaging::OnGetOptions, si, std::placeholders::_1, std::placeholders::_2) });
	c.insert({ "Move",std::bind(&soapImaging::OnMove, si, std::placeholders::_1, std::placeholders::_2) });
	c.insert({ "Stop",std::bind(&soapImaging::OnStop, si, std::placeholders::_1, std::placeholders::_2) });

	std::shared_ptr<soapSearch> ss(new soapSearch);
	auto& d = mapFunc["/onvif/search_service"];
//	d.insert({ "GetRecordingInformation", std::bind(&soapSearch::OnGetRecordingInformation,ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "GetRecordingSearchResults", std::bind(&soapSearch::OnGetRecordingSearchResults,ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "GetRecordingSummary", std::bind(&soapSearch::OnGetRecordingSummary, ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "FindRecordings", std::bind(&soapSearch::OnFindRecordings, ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "EndSearch", std::bind(&soapSearch::OnEndSearch, ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "FindEvents", std::bind(&soapSearch::OnFindEvents, ss,std::placeholders::_1, std::placeholders::_2) });
	d.insert({ "GetEventSearchResults", std::bind(&soapSearch::OnGetEventSearchResults, ss,std::placeholders::_1, std::placeholders::_2) });

//	std::shared_ptr<soapReplay> sr(new soapReplay);
//	auto& e = mapFunc["/onvif/replay_service"];
//	e.insert({ "GetReplayUri",std::bind(&soapReplay::OnGetReplayUri, sr, std::placeholders::_1, std::placeholders::_2) });
//	e.insert({ "GetServiceCapabilities",std::bind(&soapReplay::OnGetServiceCapabilities, sr, std::placeholders::_1, std::placeholders::_2) });
//	e.insert({ "GetReplayConfiguration",std::bind(&soapReplay::OnGetReplayConfiguration, sr, std::placeholders::_1, std::placeholders::_2) });
//	e.insert({ "SetReplayConfiguration",std::bind(&soapReplay::OnSetReplayConfiguration, sr, std::placeholders::_1, std::placeholders::_2) });

	std::shared_ptr<soapPTZ> sp(new soapPTZ);
	auto& f = mapFunc["/onvif/ptz_service"];
	f.insert({ "GetNodes",std::bind(&soapPTZ::OnGetNodes, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GetNode",std::bind(&soapPTZ::OnGetNode, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GetPresets",std::bind(&soapPTZ::OnGetPresets, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GetConfigurations",std::bind(&soapPTZ::OnGetConfigurations, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "AbsoluteMove",std::bind(&soapPTZ::OnAbsoluteMove, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "RelativeMove",std::bind(&soapPTZ::OnRelativeMove, sp, std::placeholders::_1, std::placeholders::_2) });
	//f.insert({ "GeoMove",std::bind(&soapPTZ::OnGeoMove, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "ContinuousMove",std::bind(&soapPTZ::OnContinuousMove, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "Stop",std::bind(&soapPTZ::OnStop, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GetStatus",std::bind(&soapPTZ::OnGetStatus, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GotoHomePosition",std::bind(&soapPTZ::OnGotoHomePosition, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "GotoPreset",std::bind(&soapPTZ::OnGotoPreset, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "RemovePreset",std::bind(&soapPTZ::OnRemovePreset, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "SetPreset",std::bind(&soapPTZ::OnSetPreset, sp, std::placeholders::_1, std::placeholders::_2) });
	f.insert({ "SetHomePosition",std::bind(&soapPTZ::OnSetHomePosition, sp, std::placeholders::_1, std::placeholders::_2) });

//	std::shared_ptr<soapEvent> se1(new soapEvent);
//	auto& g1 = mapFunc["/onvif/event_service"];
//	g1.insert({ "GetServiceCapabilities",std::bind(&soapEvent::OnGetServiceCapabilities, se1, std::placeholders::_1, std::placeholders::_2) });
//	g1.insert({ "CreatePullPointSubscription",std::bind(&soapEvent::OnCreatePullPointSubscription, se1, std::placeholders::_1, std::placeholders::_2) });
//	g1.insert({ "GetEventProperties",std::bind(&soapEvent::OnGetEventProperties, se1, std::placeholders::_1, std::placeholders::_2) });
//	g1.insert({ "Unsubscribe", std::bind(&soapEvent::OnUnsubscribe, se1, std::placeholders::_1, std::placeholders::_2) });
//	std::shared_ptr<soapEvent> se2(new soapEvent);
//	auto& g2 = mapFunc["/onvif/Subscription"];
//	g2.insert({ "PullMessages",std::bind(&soapEvent::OnPullMessages, se2, std::placeholders::_1, std::placeholders::_2) });
//	g2.insert({ "Unsubscribe",std::bind(&soapEvent::OnUnsubscribe, se2, std::placeholders::_1, std::placeholders::_2) });
//	g2.insert({ "Renew",std::bind(&soapEvent::OnRenew, se2, std::placeholders::_1, std::placeholders::_2) });

//	std::shared_ptr<soapRecording> rec(new soapRecording);
//	auto& h = mapFunc["/onvif/recording_service"];
//	h.insert({ "GetRecordings",std::bind(&soapRecording::OnGetRecordings, rec, std::placeholders::_1, std::placeholders::_2) });
//	h.insert({ "CreateRecordingJob",std::bind(&soapRecording::OnCreateRecordingJob, rec,std::placeholders::_1, std::placeholders::_2) });
//	h.insert({ "DeleteRecordingJob",std::bind(&soapRecording::OnDeleteRecordingJob, rec,std::placeholders::_1, std::placeholders::_2) });
}

bool SoapDispatcher::QueryHandler(const std::string& suffixUri, const std::string& tag, std::function<int(void*, std::string&)>& fn)
{
	auto it1 = mapFunc.find(suffixUri);
	if (it1 != mapFunc.end())
	{
		auto it = it1->second.find(tag);
		if (it != it1->second.end())
		{
			fn = it->second;
			return true;
		}
	}
	return false;
}
