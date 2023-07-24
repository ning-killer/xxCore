#pragma once
#include <string>
#include <memory>
#include <vector>
#include "tinyxml2.h"
#include "soapSchema.h"

class SoapDispatcher;
class tt__SearchScope;

class BaseSearchResponse
{
public:
	tinyxml2::XMLElement* InitHeader(const std::string& tag);
	virtual ~BaseSearchResponse();
protected:
	tinyxml2::XMLDocument _doc;
};
class tt__RecordingSummary
{
public:
	time_t DataFrom;// Required element 'tt:DataFrom'
	time_t DataUntil;// Required element 'tt:DataUntil'
	int NumberRecordings;// Required element 'tt:NumberRecordings'
};
class _tse__GetRecordingSummaryResponse:public BaseSearchResponse
{
public:
	std::shared_ptr<tt__RecordingSummary> Summary;// Required element 'tse:Summary'
	std::string Serialization();
};
enum tt__RecordingStatus {
	tt__RecordingStatus__Initiated = 0,
	tt__RecordingStatus__Recording = 1,
	tt__RecordingStatus__Stopped = 2,
	tt__RecordingStatus__Removing = 3,
	tt__RecordingStatus__Removed = 4,
	tt__RecordingStatus__Unknown = 5
};
enum tt__SearchState {
	tt__SearchState__Queued = 0,
	tt__SearchState__Searching = 1,
	tt__SearchState__Completed = 2,
	tt__SearchState__Unknown = 3
};
class _tse__GetRecordingInformation
{
public:
	std::string RecordingToken;// Required element 'tse:RecordingToken' 
};
class tt__RecordingSourceInformation
{
public:
	std::string SourceId;// Required element 'tt:SourceId'
	std::string Name;// Required element 'tt:Name'
	std::string Location;// Required element 'tt:Location'
	std::string Description;// Required element 'tt:Description'
	std::string Address;// Required element 'tt:Address'
};
class tt__TrackInformation
{
public:
	std::string TrackToken;// Required element 'tt:TrackToken'
	std::string TrackType;//Video Audio Metadata Extended Required element 'tt:TrackType'
	std::string Description;// Required element 'tt:Description'
	time_t DataFrom;// Required element 'tt:DataFrom'
	time_t DataTo;// Required element 'tt:DataTo'
};
class tt__RecordingInformation
{
public:
	std::string RecordingToken;// Required element 'tt:RecordingToken' 
	std::shared_ptr<tt__RecordingSourceInformation> Source;// Required element 'tt:Source'
	std::shared_ptr<time_t> EarliestRecording;// Optional element 'tt:EarliestRecording'
	std::shared_ptr<time_t> LatestRecording;// Optional element 'tt:LatestRecording'
	std::string Content;// Required element 'tt:Content'
	std::vector<std::shared_ptr<tt__TrackInformation>> Track;// Optional element 'tt:Track'
	enum tt__RecordingStatus RecordingStatus;// Required element 'tt:RecordingStatus'
};
class _tse__GetRecordingInformationResponse :public BaseSearchResponse
{
public:
	std::shared_ptr<tt__RecordingInformation> RecordingInformation;// Required element 'tse:RecordingInformation'
	std::string Serialization();
};

class tt__FindRecordingResultList
{
public:
	enum tt__SearchState SearchState;// Required element 'tt:SearchState'
	std::vector<std::shared_ptr<tt__RecordingInformation>> RecordingInformation;// Optional element 'tt:RecordingInformation'
};
class _tse__GetRecordingSearchResultsResponse :public BaseSearchResponse
{
public:
	tt__FindRecordingResultList ResultList;// Required element 'tse:ResultList' 
	std::string Serialization();
};
class tt__Vector2D
{
public:
	float x;// required attribute 'x'
	float y;// required attribute 'y'
	std::shared_ptr<std::string> space;// optional attribute 'space'
};
class tt__Vector1D
{
public:
	float x;// required attribute 'x'
	std::shared_ptr<std::string> space;// optional attribute 'space'
};
class tt__PTZVector
{
public:
	std::shared_ptr<tt__Vector2D> PanTilt;// Optional element 'tt:PanTilt'
	std::shared_ptr<tt__Vector1D> Zoom;// Optional element 'tt:Zoom'
};
class tt__FindPTZPositionResult
{
public:
	std::string RecordingToken;// Required element 'tt:RecordingToken'
	std::string TrackToken;// Required element 'tt:TrackToken'
	time_t Time;// Required element 'tt:Time'
	tt__PTZVector Position;// Required element 'tt:Position' 
};
class tt__FindPTZPositionResultList
{
public:
	enum tt__SearchState SearchState;// Required element 'tt:SearchState'
	std::vector<std::shared_ptr<tt__FindPTZPositionResult>> Result;// Optional element 'tt:Result'
};
class _tse__GetPTZPositionSearchResultsResponse :public BaseSearchResponse
{
public:
	tt__FindPTZPositionResultList ResultList;// Required element 'tse:ResultList'
	std::string Serialization();
};
class _tse__FindPTZPositionResponse :public BaseSearchResponse
{
public:
	std::string SearchToken;// Required element 'tse:SearchToken'
	std::string Serialization();
};
class _tse__EndSearchResponse :public BaseSearchResponse
{
public:
	time_t Endpoint;// Required element 'tse:Endpoint' 
	std::string Serialization();
};

class _tse__FindEvents
{
public:
	// Required element 'tse:StartPoint' of XML schema type 'xsd:dateTime'
	time_t StartPoint;
	// Optional element 'tse:EndPoint' of XML schema type 'xsd:dateTime'
	time_t EndPoint = 0;
	// Required element 'tse:Scope' of XML schema type 'tt:SearchScope'
	std::shared_ptr<tt__SearchScope> Scope;
	// Required element 'tse:SearchFilter' of XML schema type 'tt:EventFilter'
	//tt__EventFilter* SearchFilter;
	std::string SearchFilter;
	/// Required element 'tse:IncludeStartState' of XML schema type 'xsd:boolean'
	bool IncludeStartState;
	// Optional element 'tse:MaxMatches' of XML schema type 'xsd:int'
	std::shared_ptr<int> MaxMatches;
	// Required element 'tse:KeepAliveTime' of XML schema type 'xsd:duration'
	std::string KeepAliveTime;

	void UnSerialize(std::string str);
};
class tt__FindEventResult
{
public:
	std::string RecordingToken;// Required element 'tt:RecordingToken'
	std::string TrackToken;// Required element 'tt:TrackToken'
	time_t Time;// Required element 'tt:Time'
	wsnt__NotificationMessageHolderType Event;// Required element 'tt:Event'
	bool StartStateEvent;// Required element 'tt:StartStateEvent'
};
class tt__FindEventResultList
{
public:
	enum tt__SearchState SearchState;// Required element 'tt:SearchState'
	std::vector<std::shared_ptr<tt__FindEventResult>> Result;// Optional element 'tt:Result'
};
class _tse__GetEventSearchResultsResponse :public BaseSearchResponse
{
public:
	tt__FindEventResultList ResultList;// Required element 'tse:ResultList'
	 std::string Serialization();
};

class tt__SourceReference
{
public:
	std::string Token;
	//std::vector<char*> __any;
	// Optional attribute 'Type' of XML schema type 'xsd:anyURI'
	std::string Type; //< initialized with default value = "http://www.onvif.org/ver10/schema/Receiver"
};

class tt__SearchScope
{
public:
	// Optional element 'tt:IncludedSources' of XML schema type 'tt:SourceReference'
	std::vector<std::shared_ptr<tt__SourceReference>> IncludedSources;
	// Optional element 'tt:IncludedRecordings' of XML schema type 'tt:RecordingReference'
	std::vector<std::string> IncludedRecordings;
	// Optional element 'tt:RecordingInformationFilter' of XML schema type 'tt:XPathExpression'
	std::string RecordingInformationFilter;
	// Optional element 'tt:Extension' of XML schema type 'tt:SearchScopeExtension'
	//tt__SearchScopeExtension* Extension;
	// Optional attribute '-anyAttribute' of XML schema type 'xsd:anyType'
	char* __anyAttribute;

	void UnSerialize(tinyxml2::XMLElement* x);
};

class _tse__FindRecordings
{
public:
	std::shared_ptr<tt__SearchScope> Scope;
	std::shared_ptr<int> MaxMatches; // Optional element 'tse:MaxMatches'
	std::string KeepAliveTime;// Required element 'tse:KeepAliveTime' 'xsd:duration'

	void UnSerialize(std::string str);
};

class soapSearch
{
public:
	soapSearch() = default;
	~soapSearch() {};
private:
	int OnGetRecordingInformation(void* req, std::string& body);

	int OnGetRecordingSearchResults(void* req, std::string& body);

	int OnGetRecordingSummary(void* req, std::string& body);

	int OnFindRecordings(void* req, std::string& body);

	int OnEndSearch(void* req, std::string& body);

	int OnGetPTZPositionSearchResults(void* req, std::string& body);

	int OnFindPTZPosition(void* req, std::string& body);

	int OnFindEvents(void* req, std::string& body);

	int OnGetEventSearchResults(void* req, std::string& body);
private:
	friend SoapDispatcher;
};

