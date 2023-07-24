#pragma once
#include "soapSearch.h"

class SoapDispatcher;
class BasePTZResponse
{
public:
	tinyxml2::XMLElement* InitHeader(const std::string& tag);
	virtual ~BasePTZResponse();
protected:
	tinyxml2::XMLDocument _doc;
};
class tt__PTZSpeed
{
public:
	std::shared_ptr<tt__Vector2D> PanTilt;// Optional element 'tt:PanTilt'
	std::shared_ptr<tt__Vector1D> Zoom;// Optional element 'tt:Zoom'
};
class tt__Space2DDescription
{
public:
	std::string URI;// Required element 'tt:URI'
	// Required element 'tt:XRange' 
	float xMin;// Required element 'tt:Min'
	float xMax;// Required element 'tt:Max'
	// Required element 'tt:YRange'
	float yMin;// Required element 'tt:Min'
	float yMax;// Required element 'tt:Max'
};
class tt__PanTiltLimits
{
public:
	tt__Space2DDescription Range;// Required element 'tt:Range'
};
class tt__Space1DDescription
{
public:
	std::string URI;// Required element 'tt:URI'
	// Required element 'tt:XRange'
	float Min;// Required element 'tt:Min'
	float Max;// Required element 'tt:Max'
};
class tt__ZoomLimits
{
public:
	tt__Space1DDescription Range;// Required element 'tt:Range'
};
class tt__PTZConfiguration
{
public:
	std::string Name;// Required element 'tt:Name'
	int UseCount;// Required element 'tt:UseCount'
	std::string token;// required attribute 'token'

	std::string NodeToken;// Required element 'tt:NodeToken'
	std::shared_ptr<std::string> DefaultAbsolutePantTiltPositionSpace;// Optional element 'tt:DefaultAbsolutePantTiltPositionSpace'
	std::shared_ptr<std::string> DefaultAbsoluteZoomPositionSpace;// Optional element 'tt:DefaultAbsoluteZoomPositionSpace'
	std::shared_ptr<std::string> DefaultRelativePanTiltTranslationSpace;// Optional element 'tt:DefaultRelativePanTiltTranslationSpace'
	std::shared_ptr<std::string> DefaultRelativeZoomTranslationSpace;// Optional element 'tt:DefaultRelativeZoomTranslationSpace'
	std::shared_ptr<std::string> DefaultContinuousPanTiltVelocitySpace;// Optional element 'tt:DefaultContinuousPanTiltVelocitySpace'
	std::shared_ptr<std::string> DefaultContinuousZoomVelocitySpace;// Optional element 'tt:DefaultContinuousZoomVelocitySpace'
	std::shared_ptr<tt__PTZSpeed> DefaultPTZSpeed;// Optional element 'tt:DefaultPTZSpeed'
	std::shared_ptr<std::string> DefaultPTZTimeout;// Optional element 'tt:DefaultPTZTimeout'
	std::shared_ptr<tt__PanTiltLimits> PanTiltLimits;// Optional element 'tt:PanTiltLimits'
	std::shared_ptr<tt__ZoomLimits> ZoomLimits;// Optional element 'tt:ZoomLimits'
	//tt__PTZConfigurationExtension* Extension;// Optional element 'tt:Extension'
	std::shared_ptr<int> MoveRamp;// optional attribute 'MoveRamp'
	std::shared_ptr<int> PresetRamp;// optional attribute 'PresetRamp'
	std::shared_ptr<int> PresetTourRamp;// optional attribute 'PresetTourRamp'
};
class _tptz__GetConfigurationsResponse :public BasePTZResponse
{
public:
	std::vector<std::shared_ptr<tt__PTZConfiguration>> PTZConfiguration;// Optional element 'tptz:PTZConfiguration'
	std::string Serialization();
};
class tt__PTZPreset 
{
public:
	std::string Name;// Optional element 'tt:Name'
	std::shared_ptr<tt__PTZVector> PTZPosition;// Optional element 'tt:PTZPosition'
	std::string token;// optional attribute 'token' 
};
class _tptz__GetPresetsResponse :public BasePTZResponse
{
public:
	std::vector<std::shared_ptr<tt__PTZPreset>> Preset;// Optional element 'tptz:Preset'
	std::string Serialization();
};
class _tptz__GotoPresetResponse :public BasePTZResponse
{
public:
	std::string Serialization();
};
class _tptz__GetConfigurationResponse :public BasePTZResponse
{
public:
	std::shared_ptr<tt__PTZConfiguration> PTZConfiguration;// Required element 'tptz:PTZConfiguration'
	std::string Serialization();
};
class tt__PTZSpaces
{
public:
	std::vector<std::shared_ptr<tt__Space2DDescription>> AbsolutePanTiltPositionSpace;// Optional element 'tt:AbsolutePanTiltPositionSpace'
	std::vector<std::shared_ptr<tt__Space1DDescription>> AbsoluteZoomPositionSpace;// Optional element 'tt:AbsoluteZoomPositionSpace'
	std::vector<std::shared_ptr<tt__Space2DDescription>> RelativePanTiltTranslationSpace;// Optional element 'tt:RelativePanTiltTranslationSpace'
	std::vector<std::shared_ptr<tt__Space1DDescription>> RelativeZoomTranslationSpace;// Optional element 'tt:RelativeZoomTranslationSpace'
	std::vector<std::shared_ptr<tt__Space2DDescription>> ContinuousPanTiltVelocitySpace;// Optional element 'tt:ContinuousPanTiltVelocitySpace'
	std::vector<std::shared_ptr<tt__Space1DDescription>> ContinuousZoomVelocitySpace;// Optional element 'tt:ContinuousZoomVelocitySpace' 
	std::vector<std::shared_ptr<tt__Space1DDescription>> PanTiltSpeedSpace;// Optional element 'tt:PanTiltSpeedSpace'
	std::vector<std::shared_ptr<tt__Space1DDescription>> ZoomSpeedSpace;// Optional element 'tt:ZoomSpeedSpace'
	//tt__PTZSpacesExtension* Extension;// Optional element 'tt:Extension'
};
class tt__PTZNode
{
public:
	std::shared_ptr<std::string> Name;// Optional element 'tt:Name'
	tt__PTZSpaces SupportedPTZSpaces;// Required element 'tt:SupportedPTZSpaces' 
	int MaximumNumberOfPresets;// Required element 'tt:MaximumNumberOfPresets' 
	bool HomeSupported;// Required element 'tt:HomeSupporte
	std::vector<std::string> AuxiliaryCommands;// Optional element 'tt:AuxiliaryCommands'
	//tt__PTZNodeExtension* Extension;// Optional element 'tt:Extension'
	std::shared_ptr<bool> FixedHomePosition;// optional attribute 'FixedHomePosition'
	std::shared_ptr<bool> GeoMove;// optional attribute 'GeoMove'
	std::string token;// required attribute 'token' of XSD type 'tt:ReferenceToken'
};
class _tptz__GetNodesResponse :public BasePTZResponse
{
public:
	std::vector<std::shared_ptr<tt__PTZNode>> PTZNode;// Optional element 'tptz:PTZNode' 
	std::string Serialization();
};
class _tptz__GetNodeResponse :public BasePTZResponse
{
public:
	tt__PTZNode PTZNode;// Required element 'tptz:PTZNode' 
	std::string Serialization();
};
class tt__PTZMoveStatus
{
public:
	/*IDLE MOVING UNKNOWN*/
	std::string PanTilt;// Optional element 'tt:PanTilt' 
	std::string Zoom;// Optional element 'tt:Zoom'
};
class tt__PTZStatus
{
public:
	std::shared_ptr<tt__PTZVector> Position;// Optional element 'tt:Position'
	std::shared_ptr<tt__PTZMoveStatus> MoveStatus;// Optional element 'tt:MoveStatus'
	std::string Error;// Optional element 'tt:Error'
	time_t UtcTime;// Required element 'tt:UtcTime' 
};

class _tptz__GetStatusResponse :public BasePTZResponse
{
public:
	tt__PTZStatus PTZStatus;//equired element 'tptz:PTZStatus'
	std::string Serialization();
};

class soapPTZ
{
public:
	soapPTZ() = default;
private:
	int OnGetPresets(void* req, std::string& body);

	int OnGotoPreset(void* req, std::string& body);

	int OnGetConfigurations(void* req, std::string& body);

	int OnGetConfiguration(void* req, std::string& body);

	int OnGetNodes(void* req, std::string& body);

	int OnGetNode(void* req, std::string& body);

	int OnGetConfigurationOptions(void* req, std::string& body);

	int OnContinuousMove(void* req, std::string& body);

	int OnRelativeMove(void* req, std::string& body);

	int OnAbsoluteMove(void* req, std::string& body);

	int OnGeoMove(void* req, std::string& body);

	int OnStop(void* req, std::string& body);

	int OnGetStatus(void* req, std::string& body);

	int OnGotoHomePosition(void* req, std::string& body);

	int OnRemovePreset(void* req, std::string& body);

	int OnSetPreset(void* req, std::string& body);

	int OnSetHomePosition(void* req, std::string& body);
private:
	friend SoapDispatcher;
};

