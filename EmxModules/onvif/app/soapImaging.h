#pragma once
#include <string>
#include <vector>
#include <memory>
#include "tinyxml2.h"

class SoapDispatcher;

class BaseImagingResponse
{
public:
	tinyxml2::XMLElement* InitHeader(const std::string& tag);
	virtual ~BaseImagingResponse();
protected:
	tinyxml2::XMLDocument _doc;
};

enum tt__ExposurePriority {
    tt__ExposurePriority__LowNoise = 0,
    tt__ExposurePriority__FrameRate = 1
};
enum tt__IrCutFilterMode {
    tt__IrCutFilterMode__ON = 0,
    tt__IrCutFilterMode__OFF = 1,
    tt__IrCutFilterMode__AUTO = 2
};
class tt__FloatRange
{
public:
    float Min;// Required element 'tt:Min'
    float Max;// Required element 'tt:Max'
};
class tt__BacklightCompensation20
{
public:
    std::string Mode;//OFF ON // Required element 'tt:Mode'
    float Level;// Optional element 'tt:Level' 
};
class tt__Rectangle
{
public:
    float bottom;// optional attribute 'bottom'
    float top;// optional attribute 'top'
    float right;// optional attribute 'right'
    float left;// optional attribute 'left'
};
class tt__Exposure20
{
public:
    std::string Mode;//AUTO MANUAL Required element 'tt:Mode'
    enum tt__ExposurePriority Priority;// Optional element 'tt:Priority'
    std::shared_ptr<tt__Rectangle> Window;// Optional element 'tt:Window'
    std::shared_ptr<float> MinExposureTime;// Optional element 'tt:MinExposureTime'
    std::shared_ptr<float> MaxExposureTime;// Optional element 'tt:MaxExposureTime'
    std::shared_ptr<float> MinGain;// Optional element 'tt:MinGain'
    std::shared_ptr<float> MaxGain;// Optional element 'tt:MaxGain'
    std::shared_ptr<float> MinIris;// Optional element 'tt:MinIris'
    std::shared_ptr<float> MaxIris;// Optional element 'tt:MaxIris'
    std::shared_ptr<float> ExposureTime;// Optional element 'tt:ExposureTime'
    std::shared_ptr<float> Gain;// Optional element 'tt:Gain'
    std::shared_ptr<float> Iris;// Optional element 'tt:Iris'
};
class tt__FocusConfiguration20
{
public:
    std::string AutoFocusMode;// AUTO MANUAL //Required element 'tt:AutoFocusMode'
    float DefaultSpeed;// Optional element 'tt:DefaultSpeed'
    float NearLimit;// Optional element 'tt:NearLimit'
    float FarLimit;// Optional element 'tt:FarLimit'
    //tt__FocusConfiguration20Extension* Extension;// Optional element 'tt:Extension'
    //std::string AFMode;// optional attribute 'AFMode'
};
class tt__WideDynamicRange20
{
public:
    std::string Mode;//OFF ON Required element 'tt:Mode'
    float Level;// Optional element 'tt:Level'
};
class tt__WhiteBalance20
{
public:
    std::string Mode;//AUTO MANUAL // Required element 'tt:Mode'
    std::shared_ptr<float> CrGain;// Optional element 'tt:CrGain'
    std::shared_ptr<float> CbGain;// Optional element 'tt:CbGain'
    //tt__WhiteBalance20Extension* Extension;// Optional element 'tt:Extension'
};
class tt__ImagingSettings20
{
public:
    std::shared_ptr<tt__BacklightCompensation20> BacklightCompensation;// Optional element 'tt:BacklightCompensation' 背光
    std::shared_ptr<float> Brightness;// Optional element 'tt:Brightness' 亮度
    std::shared_ptr<float> ColorSaturation;// Optional element 'tt:ColorSaturation' 饱和度
    std::shared_ptr<float> Contrast;// Optional element 'tt:Contrast' 对比度
    std::shared_ptr <tt__Exposure20> Exposure;// Optional element 'tt:Exposure' 曝光
    std::shared_ptr<tt__FocusConfiguration20> Focus;// Optional element 'tt:Focus'  对焦
    std::shared_ptr<enum tt__IrCutFilterMode> IrCutFilter;// Optional element 'tt:IrCutFilter'
    std::shared_ptr<float> Sharpness;// Optional element 'tt:Sharpness' 锐度
    std::shared_ptr<tt__WideDynamicRange20> WideDynamicRange;// Optional element 'tt:WideDynamicRange'
    std::shared_ptr<tt__WhiteBalance20> WhiteBalance;// Optional element 'tt:WhiteBalance'  白平衡
    //tt__ImagingSettingsExtension20* Extension;// Optional element 'tt:Extension'
};
class _timg__GetImagingSettingsResponse :public BaseImagingResponse
{
public:
	tt__ImagingSettings20 ImagingSettings;// Required element 'timg:ImagingSettings' 
	std::string Serialization();
};

class _timg__SetImagingSettingsResponse :public BaseImagingResponse
{
public:
    std::string Serialization();
};
class _timg__MoveResponse :public BaseImagingResponse
{
public:
    std::string Serialization();
};
class _timg__StopResponse :public BaseImagingResponse
{
public:
    std::string Serialization();
};
class tt__AbsoluteFocusOptions
{
public:
    std::shared_ptr<tt__FloatRange> Position;// Required element 'tt:Position'
    std::shared_ptr<tt__FloatRange> Speed;// Optional element 'tt:Speed'
};
class tt__RelativeFocusOptions20
{
public:
    std::shared_ptr<tt__FloatRange> Distance; // Required element 'tt:Distance'
    std::shared_ptr<tt__FloatRange> Speed;// Optional element 'tt:Speed'
};
class tt__ContinuousFocusOptions
{
public:
    std::shared_ptr<tt__FloatRange> Speed;  // Required element 'tt:Speed'
};
class tt__MoveOptions20
{
public:
    std::shared_ptr<tt__AbsoluteFocusOptions> Absolute;// Optional element 'tt:Absolute'
    std::shared_ptr<tt__RelativeFocusOptions20> Relative;// Optional element 'tt:Relative'
    std::shared_ptr<tt__ContinuousFocusOptions> Continuous;// Optional element 'tt:Continuous'
};
class _timg__GetMoveOptionsResponse :public BaseImagingResponse
{
public:
    tt__MoveOptions20 MoveOptions;// Required element 'timg:MoveOptions'
    std::string Serialization();
};
class tt__BacklightCompensationOptions20
{
public:
    // Required element 'tt:Mode'
    std::vector<std::string> Mode;//OFF ON
    std::shared_ptr<tt__FloatRange> Level;// Optional element 'tt:Level'
};
class tt__ExposureOptions20
{
public:
    std::vector<std::string> Mode;// AUTO MANUAL Required element 'tt:Mode'
    std::vector<enum tt__ExposurePriority> Priority;// Optional element 'tt:Priority'
    std::shared_ptr<tt__FloatRange> MinExposureTime;// Optional element 'tt:MinExposureTime'
    std::shared_ptr<tt__FloatRange> MaxExposureTime;// Optional element 'tt:MaxExposureTime'
    std::shared_ptr<tt__FloatRange> MinGain;// Optional element 'tt:MinGain' 
    std::shared_ptr<tt__FloatRange> MaxGain;// Optional element 'tt:MaxGain'
    std::shared_ptr<tt__FloatRange> MinIris;// Optional element 'tt:MinIris'
    std::shared_ptr<tt__FloatRange> MaxIris;// Optional element 'tt:MaxIris'
    std::shared_ptr<tt__FloatRange> ExposureTime;// Optional element 'tt:ExposureTime'
    std::shared_ptr<tt__FloatRange> Gain;// Optional element 'tt:Gain'
    std::shared_ptr<tt__FloatRange> Iris;// Optional element 'tt:Iris'
};
class tt__FocusOptions20
{
public:
    std::vector<std::string> AutoFocusModes;//AUTO MANUAL // Optional element 'tt:AutoFocusModes'
    std::shared_ptr<tt__FloatRange> DefaultSpeed;// Optional element 'tt:DefaultSpeed'
    std::shared_ptr<tt__FloatRange> NearLimit;// Optional element 'tt:NearLimit'
    std::shared_ptr<tt__FloatRange> FarLimit;// Optional element 'tt:FarLimit' 
    //tt__FocusOptions20Extension* Extension;// Optional element 'tt:Extension'
};
class tt__WideDynamicRangeOptions20
{
public:
    std::vector<std::string> Mode;//OFF ON Required element 'tt:Mode' 
    std::shared_ptr<tt__FloatRange> Level;// Optional element 'tt:Level'
};
class tt__WhiteBalanceOptions20
{
public:
    std::vector<std::string> Mode;//AUTO MANUAL Required element 'tt:Mode'
    std::shared_ptr<tt__FloatRange> YrGain;// Optional element 'tt:YrGain'
    std::shared_ptr<tt__FloatRange> YbGain;// Optional element 'tt:YbGain'
    //tt__WhiteBalanceOptions20Extension* Extension;// Optional element 'tt:Extension'
};
class tt__ImagingOptions20
{
public:
    std::shared_ptr<tt__BacklightCompensationOptions20> BacklightCompensation;// Optional element 'tt:BacklightCompensation'
    std::shared_ptr<tt__FloatRange> Brightness;// Optional element 'tt:Brightness'
    std::shared_ptr<tt__FloatRange> ColorSaturation;// Optional element 'tt:ColorSaturation'
    std::shared_ptr<tt__FloatRange> Contrast;// Optional element 'tt:Contrast'
    std::shared_ptr<tt__ExposureOptions20> Exposure;// Optional element 'tt:Exposure'
    std::shared_ptr<tt__FocusOptions20> Focus;// Optional element 'tt:Focus'
    std::vector<enum tt__IrCutFilterMode> IrCutFilterModes;// Optional element 'tt:IrCutFilterModes'
    std::shared_ptr<tt__FloatRange> Sharpness;// Optional element 'tt:Sharpness'
    std::shared_ptr<tt__WideDynamicRangeOptions20> WideDynamicRange;// Optional element 'tt:WideDynamicRange'
    std::shared_ptr<tt__WhiteBalanceOptions20> WhiteBalance;// Optional element 'tt:WhiteBalance'
    //tt__ImagingOptions20Extension* Extension;// Optional element 'tt:Extension'
};
class _timg__GetOptionsResponse :public BaseImagingResponse
{
public:
    // Required element 'timg:ImagingOptions'
    tt__ImagingOptions20 ImagingOptions;
    std::string Serialization();
};
class soapImaging
{
public:
	soapImaging() = default;
	~soapImaging() {};
private:
	int OnGetImagingSettings(void* req, std::string& body);

	int OnSetImagingSettings(void* req, std::string& body);

	int OnMove(void* req, std::string& body);

	int OnStop(void* req, std::string& body);

	int OnGetMoveOptions(void* req, std::string& body);

    int OnGetOptions(void* req, std::string& body);
private:
    friend SoapDispatcher;
};

