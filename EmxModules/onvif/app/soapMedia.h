#pragma once
#include <string>
#include <memory>
#include "tinyxml2.h"
#include "soapPTZ.h"
#include "soapSearch.h"

class SoapDispatcher;

class BaseMediaResponse
{
public:
    tinyxml2::XMLElement* InitHeader(const std::string& tag);
    virtual ~BaseMediaResponse();
protected:
    tinyxml2::XMLDocument _doc;
};

enum tt__H264Profile {
    tt__H264Profile__Baseline = 0,
    tt__H264Profile__Main = 1,
    tt__H264Profile__Extended = 2,
    tt__H264Profile__High = 3
};
class tt__IntRectangle
{
public:
    int x;
    int y;
    int width;
    int height;
};
class tt__ConfigurationEntity
{
public:
    std::string Name;// Required element 'tt:Name'
    int UseCount;// Required element 'tt:UseCount'
    std::string token;// required attribute 'token'
};
class tt__IPAddress
{
public:
    std::string Type;//IPv4 IPv6 // Required element 'tt:Type'
    std::shared_ptr<std::string> IPv4Address;// Optional element 'tt:IPv4Address'
    std::shared_ptr<std::string> IPv6Address;// Optional element 'tt:IPv6Address'
};
class tt__MulticastConfiguration
{
public:
    std::shared_ptr<tt__IPAddress> Address;// Required element 'tt:Address'
    int Port;// Required element 'tt:Port'
    int TTL;// Required element 'tt:TTL'
    bool AutoStart;// Required element 'tt:AutoStart'
};
class tt__PTZFilter
{
public:
    bool Status;// Required element 'tt:Status'
    bool Position;// Required element 'tt:Position'
};
//class tt__PTZConfiguration
//{
//public:
//};
class tt__Config
{
public:
    tt__ItemList Parameters;// Required element 'tt:Parameters' 
    std::string Name;// required attribute 'Name'
    std::string Type;// required attribute 'Type'
};
class tt__AnalyticsEngineConfiguration
{
public:
    std::vector<std::shared_ptr<tt__Config>> AnalyticsModule;// Optional element 'tt:AnalyticsModule'
    /// Optional element 'tt:Extension' of XSD type 'tt:AnalyticsEngineConfigurationExtension'
    //tt__AnalyticsEngineConfigurationExtension* Extension;
};
class tt__RuleEngineConfiguration
{
public:
    std::vector<std::shared_ptr<tt__Config>> Rule;// Optional element 'tt:Rule'
    //tt__RuleEngineConfigurationExtension* Extension;// Optional element 'tt:Extension'
};
class tt__VideoAnalyticsConfiguration :public tt__ConfigurationEntity
{
public:
    tt__AnalyticsEngineConfiguration AnalyticsEngineConfiguration;// Required element 'tt:AnalyticsEngineConfiguration' 
    tt__RuleEngineConfiguration RuleEngineConfiguration;// Required element 'tt:RuleEngineConfiguration'
};
class tt__EventSubscription
{
public:
    wsnt__FilterType Filter;// Optional element 'tt:Filter'
    //_tt__EventSubscription_SubscriptionPolicy* SubscriptionPolicy;// Optional element 'tt:SubscriptionPolicy'
};
class tt__MetadataConfiguration :public tt__ConfigurationEntity
{
public:
    std::shared_ptr<tt__PTZFilter> PTZStatus;// Optional element 'tt:PTZStatus'
    std::shared_ptr<tt__EventSubscription> Events;// Optional element 'tt:Events' 
    bool Analytics;// Optional element 'tt:Analytics'
    tt__MulticastConfiguration Multicast;// Required element 'tt:Multicast' 
    std::string SessionTimeout;// Required element 'tt:SessionTimeout'
};
class tt__AudioEncoderConfiguration :public tt__ConfigurationEntity
{
public:
    std::string Encoding;//G711 G726 AAC Required element 'tt:Encoding'
    int Bitrate;// Required element 'tt:Bitrate' kbps
    int SampleRate;// Required element 'tt:SampleRate' kHz
    tt__MulticastConfiguration Multicast;// Required element 'tt:Multicast'
    std::string SessionTimeout;// Required element 'tt:SessionTimeout'
};
class tt__VideoRateControl
{
public:
    int FrameRateLimit;// Required element 'tt:FrameRateLimit'
    int EncodingInterval;// Required element 'tt:EncodingInterval'
    int BitrateLimit;// Required element 'tt:BitrateLimit' kbps
};
class tt__H264Configuration
{
public:
    int GovLength;// Required element 'tt:GovLength'
    enum tt__H264Profile H264Profile;// Required element 'tt:H264Profile'
};
class tt__VideoEncoderConfiguration :public tt__ConfigurationEntity
{
public:
    std::string Encoding;//JPEG MEG4 H264 Required element 'tt:Encoding'
    int Width;// Required element 'tt:Resolution'
    int Height;
    float Quality;// Required element 'tt:Quality'
    std::shared_ptr<tt__VideoRateControl> RateControl;// Optional element 'tt:RateControl'
    //tt__Mpeg4Configuration* MPEG4;// Optional element 'tt:MPEG4'
    std::shared_ptr<tt__H264Configuration> H264;// Optional element 'tt:H264'
    tt__MulticastConfiguration Multicast;// Required element 'tt:Multicast'
    std::string SessionTimeout;// Required element 'tt:SessionTimeout'
    bool* GuaranteedFrameRate = nullptr;// optional attribute 'GuaranteedFrameRate'
};
class tt__AudioSourceConfiguration :public tt__ConfigurationEntity
{
public:
    std::string SourceToken;
};
class tt__VideoSourceConfiguration :public tt__ConfigurationEntity
{
public:
    std::string SourceToken;// Required element 'tt:SourceToken'
    tt__IntRectangle Bounds;// Required element 'tt:Bounds'
    std::shared_ptr<std::string> ViewMode;// optional attribute 'ViewMode' of XSD type 'xsd:string'
};
class tt__Profile
{
public:
    std::string Name;// Required element 'tt:Name'
    std::shared_ptr<tt__VideoSourceConfiguration> VideoSourceConfiguration;
    std::shared_ptr<tt__AudioSourceConfiguration> AudioSourceConfiguration;
    std::shared_ptr<tt__VideoEncoderConfiguration> VideoEncoderConfiguration;
    std::shared_ptr<tt__AudioEncoderConfiguration> AudioEncoderConfiguration;
    std::shared_ptr<tt__VideoAnalyticsConfiguration> VideoAnalyticsConfiguration;// Optional element 'tt:VideoAnalyticsConfiguration'
    std::shared_ptr<tt__PTZConfiguration> PTZConfiguration;// Optional element 'tt:PTZConfiguration'
    std::shared_ptr<tt__MetadataConfiguration> MetadataConfiguration;// Optional element 'tt:MetadataConfiguration'
    //tt__ProfileExtension* Extension;// Optional element 'tt:Extension'
    std::string token;//r attr
    bool fixed;//attr
};

class _trt__GetProfilesResponse :public BaseMediaResponse
{
public:
    std::vector<std::shared_ptr<tt__Profile>> Profiles;
    std::string Serialization();
};

class tt__StreamSetup
{
public:
    std::string Stream;//RTP_Unicast RTP_Multicast Required element 'tt:Stream'  
    std::string Transport;//UDP TCP RTSP HTTP Required element 'tt:Protocol'
};
class _trt__GetStreamUri
{
public:
    tt__StreamSetup StreamSetup;// Required element 'trt:StreamSetup'
    std::string ProfileToken;// Required element 'trt:ProfileToken'
};

class tt__MediaUri
{
public:
    std::string Uri;// Required element 'tt:Uri'
    bool InvalidAfterConnect;// Required element 'tt:InvalidAfterConnect'
    bool InvalidAfterReboot;// Required element 'tt:InvalidAfterReboot'
    std::string Timeout;// Required element 'tt:Timeout'
};
class _trt__GetStreamUriResponse :public BaseMediaResponse
{
public:
    tt__MediaUri MediaUri;
    std::string Serialization();
};

class tt__VideoSource
{
public:
    std::string token;
    float Framerate;// Required element 'tt:Framerate' 
    /// Required element 'tt:Resolution'
    int Width;
    int Heigth;
    //std::shared_ptr<tt__ImagingSettings> Imaging;// Optional element 'tt:Imaging'
    //tt__VideoSourceExtension* Extension;// Optional element 'tt:Extension' 
};
class _trt__GetVideoSourcesResponse :public BaseMediaResponse
{
public:
    std::vector<std::shared_ptr<tt__VideoSource>> VideoSources;
    std::string Serialization();
};
class tt__AudioSource
{
public:
    std::string token;
    int Channels;
};
class _trt__GetAudioSourcesResponse :public BaseMediaResponse
{
public:
    std::vector<std::shared_ptr<tt__AudioSource>> AudioSources;
    std::string Serialization();
};

class _trt__GetProfileResponse :public BaseMediaResponse
{
public:
    std::shared_ptr<tt__Profile> Profile;
    std::string Serialization();
};

class _trt__GetVideoSourceConfigurationResponse :public BaseMediaResponse
{
public:
    tt__VideoSourceConfiguration Configuration;
    std::string Serialization();
};
class tt__VideoResolution
{
public:
    int Width;// Required element 'tt:Width'
    int Height;// Required element 'tt:Height'
};
class tt__IntRange
{
public:
    // Required element 'tt:QualityRange'
    int Min;
    /// Required element 'tt:Max' of XSD type 'xsd:int'
    int Max;
};
class tt__H264Options
{
public:
    /// Required element 'tt:ResolutionsAvailable' of XSD type 'tt:VideoResolution'
    std::vector<std::shared_ptr<tt__VideoResolution>> ResolutionsAvailable;
    /// Required element 'tt:GovLengthRange' of XSD type 'tt:IntRange'
    tt__IntRange GovLengthRange;
    /// Required element 'tt:FrameRateRange' of XSD type 'tt:IntRange'
    tt__IntRange FrameRateRange;
    /// Required element 'tt:EncodingIntervalRange' of XSD type 'tt:IntRange'
    tt__IntRange EncodingIntervalRange;
    // Required element 'tt:H264ProfilesSupported' of XSD type 'tt:H264Profile'
    std::vector<enum tt__H264Profile> H264ProfilesSupported;
};
class tt__VideoEncoderConfigurationOptions
{
public:
    std::string token;
    tt__IntRange QualityRange;
    /// Optional element 'tt:JPEG' of XSD type 'tt:JpegOptions'
    //tt__JpegOptions* JPEG;
    /// Optional element 'tt:MPEG4' of XSD type 'tt:Mpeg4Options'
    //tt__Mpeg4Options* MPEG4;
    std::shared_ptr<tt__H264Options>  H264;// Optional element 'tt:H264'
    //tt__VideoEncoderOptionsExtension* Extension;// Optional element 'tt:Extension' 
};
class _trt__GetVideoEncoderConfigurationOptionsResponse :public BaseMediaResponse
{
public:
    tt__VideoEncoderConfigurationOptions Options;
    std::string Serialization();
};

class trt__StreamingCapabilities
{
public:
    bool RTPMulticast;// optional attribute 'RTPMulticast'
    bool RTP_USCORETCP;// optional attribute 'RTP_TCP'
    bool RTP_USCORERTSP_USCORETCP;// optional attribute 'RTP_RTSP_TCP'
    bool NonAggregateControl;// optional attribute 'NonAggregateControl'
    bool NoRTSPStreaming;// optional attribute 'NoRTSPStreaming'
};
class trt__Capabilities
{
public:
    /*Required element 'trt:ProfileCapabilities'*/
    int MaximumNumberOfProfiles;// optional attribute 'MaximumNumberOfProfiles'
    trt__StreamingCapabilities StreamingCapabilities;// Required element 'trt:StreamingCapabilities'
    bool SnapshotUri;// optional attribute 'SnapshotUri'
    bool Rotation;// optional attribute 'Rotation'
    bool VideoSourceMode;// optional attribute 'VideoSourceMode'
    bool OSD;// optional attribute 'OSD'
    bool TemporaryOSDText;// optional attribute 'TemporaryOSDText'
    bool EXICompression;// optional attribute 'EXICompression'
};
class _trt__GetServiceCapabilitiesResponse :public BaseMediaResponse
{
public:
    trt__Capabilities Capabilities;
    std::string Serialization();
};
class _trt__AddPTZConfigurationResponse :public BaseMediaResponse
{
public:
    std::string Serialization();
};
class _trt__GetAudioEncoderConfigurationsResponse :public BaseMediaResponse
{
public:
    tt__AudioEncoderConfiguration  AudioEncoderConfigurations;
    std::string Serialization();
};
class _trt__GetVideoEncoderConfigurationsResponse :public BaseMediaResponse
{
public:
    std::vector<std::shared_ptr<tt__VideoEncoderConfiguration>> VideoEncoder;
    std::string Serialization();
};
class soapMedia
{
public:
	soapMedia();
	~soapMedia();
private:
    int OnGetServiceCapabilities(void* req, std::string& body);

    int OnGetProfiles(void* req, std::string& body);

    int OnGetStreamUri(void* req, std::string& body);

    int OnGetOSD(void* req, std::string& body);

    int OnSetOSD(void* req, std::string& body);

    int OnGetOSDOptions(void* req, std::string& body);

    int OnGetVideoSources(void* req, std::string& body);
    
    int OnGetAudioSources(void* req, std::string& body);

    int OnGetProfile(void* req, std::string& body);

    int OnGetSnapshotUri(void* req, std::string& body);

    int OnGetVideoSourceConfiguration(void* req, std::string& body);

    int OnGetVideoEncoderConfigurationOptions(void* req, std::string& body);

    int OnGetVideoEncoderConfiguration(void* req, std::string& body);

    int OnGetVideoEncoderConfigurations(void* req, std::string& body);

    int OnGetAudioEncoderConfigurationOptions(void* req, std::string& body);
    
    int OnGetAudioEncoderConfiguration(void* req, std::string& body);

    int OnGetAudioEncoderConfigurations(void* req, std::string& body);

    int OnAddPTZConfiguration(void* req, std::string& body);

    int OnSetVideoEncoderConfiguration(void* req, std::string& body);

    int OnGetSnapshot(void* req, std::string& body);

    int OnGetMetadataConfigurationOptions(void* req, std::string& body);

    int OnGetMetadataConfiguration(void* req, std::string& body);

    int OnSetSynchronizationPoint(void* req, std::string& body);
private:
	friend SoapDispatcher;
};

