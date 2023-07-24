#include "soapSearch.h"
#include "soapUtil.h"
#include <time.h>

BaseSearchResponse::~BaseSearchResponse()
{
}

tinyxml2::XMLElement* BaseSearchResponse::InitHeader(const std::string& tag)
{
	auto declare = _doc.NewDeclaration();
	_doc.InsertFirstChild(declare);

	/*Envelope*/
	auto envelopeNode = _doc.NewElement("s:Envelope");
	_doc.InsertEndChild(envelopeNode);
	envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
	envelopeNode->SetAttribute("xmlns:sc", "http://www.w3.org/2003/05/soap-encoding");
	envelopeNode->SetAttribute("xmlns:tt", "http://www.onvif.org/ver10/schema");
	envelopeNode->SetAttribute("xmlns:tse", "http://www.onvif.org/ver10/search/wsdl");
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
std::string _tse__GetRecordingSummaryResponse::Serialization()
{
	auto respNode = InitHeader("tse:GetRecordingSummaryResponse");
	auto summaryNode = _doc.NewElement("tse:Summary");
	auto dateFromNode = _doc.NewElement("tt:DataFrom");
	auto dateUntilNode = _doc.NewElement("tt:DataUntil");
	auto nrNode = _doc.NewElement("tt:NumberRecordings");
	dateFromNode->SetText(SoapUtil::soap_createTime(Summary->DataFrom).c_str());
	dateUntilNode->SetText(SoapUtil::soap_createTime(Summary->DataUntil).c_str());
	nrNode->SetText(Summary->NumberRecordings);

	summaryNode->InsertEndChild(dateFromNode);
	summaryNode->InsertEndChild(dateUntilNode);
	summaryNode->InsertEndChild(nrNode);
	respNode->InsertEndChild(summaryNode);
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tse__GetRecordingInformationResponse::Serialization()
{
	auto respNode = InitHeader("tse:GetRecordingInformationResponse");
	auto infoNode = _doc.NewElement("tse:RecordingInformation");
	respNode->InsertEndChild(infoNode);
	auto tokenNode = _doc.NewElement("tt:RecordingToken");
	tokenNode->SetText(RecordingInformation->RecordingToken.c_str());
	infoNode->InsertEndChild(tokenNode);

	auto sourceNode = _doc.NewElement("tt:Source");
	auto sidNode = _doc.NewElement("tt:SourceId");
	auto nNode = _doc.NewElement("tt:Name");
	auto locationNode = _doc.NewElement("tt:Location");
	auto descNode = _doc.NewElement("tt:Description");
	auto addressNode = _doc.NewElement("tt:Address");
	sidNode->SetText(RecordingInformation->Source->SourceId.c_str());
	nNode->SetText(RecordingInformation->Source->Name.c_str());
	locationNode->SetText(RecordingInformation->Source->Location.c_str());
	descNode->SetText(RecordingInformation->Source->Description.c_str());
	addressNode->SetText(RecordingInformation->Source->Address.c_str());
	sourceNode->InsertEndChild(sidNode);
	sourceNode->InsertEndChild(nNode);
	sourceNode->InsertEndChild(locationNode);
	sourceNode->InsertEndChild(descNode);
	sourceNode->InsertEndChild(addressNode);
	infoNode->InsertEndChild(sourceNode);
	if (RecordingInformation->EarliestRecording)
	{
		auto erNode = _doc.NewElement("tt:EarliestRecording");
		erNode->SetText(SoapUtil::soap_createTime(*RecordingInformation->EarliestRecording).c_str());
		infoNode->InsertEndChild(erNode);
	}
	if (RecordingInformation->LatestRecording)
	{
		auto erNode = _doc.NewElement("tt:LatestRecording");
		erNode->SetText(SoapUtil::soap_createTime(*RecordingInformation->LatestRecording).c_str());
		infoNode->InsertEndChild(erNode);
	}
	auto cntNode = _doc.NewElement("tt:Content");
	cntNode->SetText(RecordingInformation->Content.c_str());
	infoNode->InsertEndChild(cntNode);
	for (size_t i = 0; i < RecordingInformation->Track.size(); i++)
	{
		auto trackNode = _doc.NewElement("tt:Track");
		auto ttNode = _doc.NewElement("tt:TrackToken");
		auto tTypeNode = _doc.NewElement("tt:TrackType");
		auto tDescNode = _doc.NewElement("tt:Description");
		auto tDateFromNode = _doc.NewElement("tt:DataFrom");
		auto tDataToNode = _doc.NewElement("tt:DataTo");
		ttNode->SetText(RecordingInformation->Track[i]->TrackToken.c_str());
		tTypeNode->SetText(RecordingInformation->Track[i]->TrackType.c_str());
		tDescNode->SetText(RecordingInformation->Track[i]->Description.c_str());
		tDateFromNode->SetText(SoapUtil::soap_createTime(RecordingInformation->Track[i]->DataFrom).c_str());
		tDataToNode->SetText(SoapUtil::soap_createTime(RecordingInformation->Track[i]->DataTo).c_str());
		trackNode->InsertEndChild(ttNode);
		trackNode->InsertEndChild(tTypeNode);
		trackNode->InsertEndChild(tDescNode);
		trackNode->InsertEndChild(tDateFromNode);
		trackNode->InsertEndChild(tDataToNode);
		infoNode->InsertEndChild(trackNode);
	}
	auto rsNode = _doc.NewElement("tt:RecordingStatus");
	switch (RecordingInformation->RecordingStatus)
	{
	case tt__RecordingStatus__Initiated:
		rsNode->SetText("Initiated");
		break;
	case tt__RecordingStatus__Recording:
		rsNode->SetText("Recording");
		break;
	case tt__RecordingStatus__Stopped:
		rsNode->SetText("Stopped");
		break;
	case tt__RecordingStatus__Removing:
		rsNode->SetText("Removing");
		break;
	case tt__RecordingStatus__Removed:
		rsNode->SetText("Removed");
		break;
	case tt__RecordingStatus__Unknown:
		rsNode->SetText("Unknown");
		break;
	}
	infoNode->InsertEndChild(rsNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tse__GetRecordingSearchResultsResponse::Serialization()
{
	auto respNode = InitHeader("tse:GetRecordingSearchResultsResponse");
	auto resListNode = _doc.NewElement("tse:ResultList");
	respNode->InsertEndChild(resListNode);
	auto searchStateNode = _doc.NewElement("tt:SearchState");
	switch (ResultList.SearchState)
	{
	case tt__SearchState__Queued:
		searchStateNode->SetText("Queued");
		break;
	case tt__SearchState__Searching:
		searchStateNode->SetText("Searching");
		break;
	case tt__SearchState__Completed:
		searchStateNode->SetText("Completed");
		break;
	case tt__SearchState__Unknown:
		searchStateNode->SetText("Unknown");
		break;
	}
	resListNode->InsertEndChild(searchStateNode);
	for (size_t i = 0; i < ResultList.RecordingInformation.size(); i++)
	{
		auto infoNode = _doc.NewElement("tt:RecordingInformation");
		auto tokenNode = _doc.NewElement("tt:RecordingToken");
		tokenNode->SetText(ResultList.RecordingInformation[i]->RecordingToken.c_str());
		infoNode->InsertEndChild(tokenNode);

		auto sourceNode = _doc.NewElement("tt:Source");
		auto sidNode = _doc.NewElement("tt:SourceId");
		auto nNode = _doc.NewElement("tt:Name");
		auto locationNode = _doc.NewElement("tt:Location");
		auto descNode = _doc.NewElement("tt:Description");
		auto addressNode = _doc.NewElement("tt:Address");
		sidNode->SetText(ResultList.RecordingInformation[i]->Source->SourceId.c_str());
		nNode->SetText(ResultList.RecordingInformation[i]->Source->Name.c_str());
		locationNode->SetText(ResultList.RecordingInformation[i]->Source->Location.c_str());
		descNode->SetText(ResultList.RecordingInformation[i]->Source->Description.c_str());
		addressNode->SetText(ResultList.RecordingInformation[i]->Source->Address.c_str());
		sourceNode->InsertEndChild(sidNode);
		sourceNode->InsertEndChild(nNode);
		sourceNode->InsertEndChild(locationNode);
		sourceNode->InsertEndChild(descNode);
		sourceNode->InsertEndChild(addressNode);
		infoNode->InsertEndChild(sourceNode);
		if (ResultList.RecordingInformation[i]->EarliestRecording)
		{
			auto erNode = _doc.NewElement("tt:EarliestRecording");
			erNode->SetText(SoapUtil::soap_createTime(*ResultList.RecordingInformation[i]->EarliestRecording).c_str());
			infoNode->InsertEndChild(erNode);
		}
		if (ResultList.RecordingInformation[i]->LatestRecording)
		{
			auto erNode = _doc.NewElement("tt:LatestRecording");
			erNode->SetText(SoapUtil::soap_createTime(*ResultList.RecordingInformation[i]->LatestRecording).c_str());
			infoNode->InsertEndChild(erNode);
		}
		auto cntNode = _doc.NewElement("tt:Content");
		cntNode->SetText(ResultList.RecordingInformation[i]->Content.c_str());
		infoNode->InsertEndChild(cntNode);
		for (size_t j = 0; j < ResultList.RecordingInformation[i]->Track.size(); j++)
		{
			auto trackNode = _doc.NewElement("tt:Track");
			auto ttNode = _doc.NewElement("tt:TrackToken");
			auto tTypeNode = _doc.NewElement("tt:TrackType");
			auto tDescNode = _doc.NewElement("tt:Description");
			auto tDateFromNode = _doc.NewElement("tt:DataFrom");
			auto tDataToNode = _doc.NewElement("tt:DataTo");
			ttNode->SetText(ResultList.RecordingInformation[i]->Track[j]->TrackToken.c_str());
			tTypeNode->SetText(ResultList.RecordingInformation[i]->Track[j]->TrackType.c_str());
			tDescNode->SetText(ResultList.RecordingInformation[i]->Track[j]->Description.c_str());
			tDateFromNode->SetText(SoapUtil::soap_createTime(ResultList.RecordingInformation[i]->Track[j]->DataFrom).c_str());
			tDataToNode->SetText(SoapUtil::soap_createTime(ResultList.RecordingInformation[i]->Track[j]->DataTo).c_str());
			trackNode->InsertEndChild(ttNode);
			trackNode->InsertEndChild(tTypeNode);
			trackNode->InsertEndChild(tDescNode);
			trackNode->InsertEndChild(tDateFromNode);
			trackNode->InsertEndChild(tDataToNode);
			infoNode->InsertEndChild(trackNode);
		}
		auto rsNode = _doc.NewElement("tt:RecordingStatus");
		switch (ResultList.RecordingInformation[i]->RecordingStatus)
		{
		case tt__RecordingStatus__Initiated:
			rsNode->SetText("Initiated");
			break;
		case tt__RecordingStatus__Recording:
			rsNode->SetText("Recording");
			break;
		case tt__RecordingStatus__Stopped:
			rsNode->SetText("Stopped");
			break;
		case tt__RecordingStatus__Removing:
			rsNode->SetText("Removing");
			break;
		case tt__RecordingStatus__Removed:
			rsNode->SetText("Removed");
			break;
		case tt__RecordingStatus__Unknown:
			rsNode->SetText("Unknown");
			break;
		}
		infoNode->InsertEndChild(rsNode);

		resListNode->InsertEndChild(infoNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tse__GetPTZPositionSearchResultsResponse::Serialization()
{
//	auto respNode = InitHeader("tse:GetPTZPositionSearchResultsResponse");
	InitHeader("tse:GetPTZPositionSearchResultsResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tse__FindPTZPositionResponse::Serialization()
{
//	auto respNode = InitHeader("tse:FindPTZPositionResponse");
	InitHeader("tse:FindPTZPositionResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tse__EndSearchResponse::Serialization()
{
	auto respNode = InitHeader("tse:EndSearchResponse");
	auto endPointNode = _doc.NewElement("tse:Endpoint");
	endPointNode->SetText(SoapUtil::soap_createTime(Endpoint).c_str());
	respNode->InsertEndChild(endPointNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

void _tse__FindEvents::UnSerialize(std::string str)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(str.c_str(), str.size());

	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		for (auto i = envelopeNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Name(), "Body"))
			{
				auto frNode = i->FirstChildElement();//tse:FindRecordings
				for (auto j = frNode->FirstChildElement(); j; j = j->NextSiblingElement())
				{
					auto n = j->Name();
					if (strstr(n, "Scope"))
					{
						Scope.reset(new tt__SearchScope);
						Scope->UnSerialize(j);
					}
					else if (strstr(n, "MaxMatches"))
					{
						MaxMatches.reset(new int(j->IntText()));
					}
					else if (strstr(n, "KeepAliveTime"))
					{
						KeepAliveTime = j->GetText();
					}
					else if (strstr(n, "StartPoint"))
					{
						tm tt;
						strptime(j->GetText(), "%Y-%m-%dT%H:%M:%SZ", &tt);
						tt.tm_hour += 8;//zone +8
						
						StartPoint = mktime(&tt);
					}
					else if (strstr(n, "EndPoint"))
					{
						tm tt;
						strptime(j->GetText(), "%Y-%m-%dT%H:%M:%SZ", &tt);
						tt.tm_hour += 8;// zone + 8
						EndPoint = mktime(&tt);
					}
					else if (strstr(n, "SearchFilter"))
						SearchFilter = j->GetText();
				}
			}
			else if (strstr(i->Name(), "Header"))
			{
			}
		}
	}
}

void tt__SearchScope::UnSerialize(tinyxml2::XMLElement* x)
{
	for (auto i = x->FirstChildElement(); i; i = i->NextSiblingElement())
	{
		if (strstr(i->Name(), "IncludedSources"))
		{
			std::shared_ptr<tt__SourceReference> item(new tt__SourceReference);
			auto attr = i->FindAttribute("Type");
			if (attr)
				item->Type = attr->Value();

			auto tokeNode = i->FirstChildElement();
			if (tokeNode && strstr(tokeNode->Name(), "Token"))
				item->Token = tokeNode->GetText();
			
			IncludedSources.push_back(item);
		}
		else if (strstr(i->Name(), "IncludedRecordings"))
		{
			IncludedRecordings.push_back(i->GetText());
		}
		else if (strstr(i->Name(), "RecordingInformationFilter"))
		{
			RecordingInformationFilter = i->GetText();
		}
	}
}

void _tse__FindRecordings::UnSerialize(std::string str)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(str.c_str(), str.size());

	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		for (auto i = envelopeNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Name(), "Body"))
			{
				auto frNode = i->FirstChildElement();//tse:FindRecordings
				for (auto j = frNode->FirstChildElement(); j; j = j->NextSiblingElement())
				{
					auto n = j->Name();
					if (strstr(n, "Scope"))
					{
						Scope.reset(new tt__SearchScope);
						Scope->UnSerialize(j);
					}
					else if(strstr(n, "MaxMatches"))
					{ 
						MaxMatches.reset(new int(j->IntText()));
					}
					else if(strstr(n, "KeepAliveTime"))
					{
						KeepAliveTime = j->GetText();
					}
				}
			}
			else if(strstr(i->Name(), "Header"))
			{ }
		}
	}
}

std::string _tse__GetEventSearchResultsResponse::Serialization()
{
	auto respNode = InitHeader("tse:GetEventSearchResultsResponse");
	auto resListNode = _doc.NewElement("tse:ResultList");
	respNode->InsertEndChild(resListNode);

	auto searchStateNode = _doc.NewElement("tt:SearchState");
	switch (ResultList.SearchState)
	{
	case tt__SearchState__Queued:
		searchStateNode->SetText("Queued");
		break;
	case tt__SearchState__Searching:
		searchStateNode->SetText("Searching");
		break;
	case tt__SearchState__Completed:
		searchStateNode->SetText("Completed");
		break;
	case tt__SearchState__Unknown:
		searchStateNode->SetText("Unknown");
		break;
	}
	resListNode->InsertEndChild(searchStateNode);
	for (auto& item : ResultList.Result)
	{
		auto resNode = _doc.NewElement("tt:Result");
		auto rTokenNode = _doc.NewElement("tt:RecordingToken");
		auto trackNode = _doc.NewElement("tt:TrackToken");
		auto timeNode = _doc.NewElement("tt:Time");
		auto eventNode = _doc.NewElement("tt:Event");
		auto stateEventNode = _doc.NewElement("tt:StartStateEvent");
		rTokenNode->SetText(item->RecordingToken.c_str());
		trackNode->SetText(item->TrackToken.c_str());
		timeNode->SetText(SoapUtil::soap_createTime(item->Time).c_str());

		if (item->Event.SubscriptionReference)
		{
			auto a = _doc.NewElement("wsnt:SubscriptionReference");
			auto b = _doc.NewElement("wsa5:Address");
			b->SetText(item->Event.SubscriptionReference->Address.c_str());
			a->InsertEndChild(b);
			if (item->Event.SubscriptionReference->ReferenceParameters)
			{
				auto x = _doc.NewElement("wsa5:ReferenceParameters");
				auto y = _doc.NewElement("chan:ChannelInstance");
				y->SetText(item->Event.SubscriptionReference->ReferenceParameters->chan__ChannelInstance);
				x->InsertEndChild(y);
				a->InsertEndChild(x);
			}
			if (item->Event.SubscriptionReference->Metadata)
			{
				auto x = _doc.NewElement("wsa5:Metadata");
				a->InsertEndChild(x);
			}

			eventNode->InsertEndChild(a);
		}
		if (item->Event.Topic)
		{
			auto a = _doc.NewElement("wsnt:Topic");
			a->SetAttribute("Dialect", item->Event.Topic->Dialect.c_str());
			eventNode->InsertEndChild(a);
		}
		if (item->Event.ProducerReference)
		{
			auto a = _doc.NewElement("wsnt:ProducerReference");
			auto b = _doc.NewElement("wsa5:Address");
			b->SetText(item->Event.ProducerReference->Address.c_str());
			a->InsertEndChild(b);
			if (item->Event.ProducerReference->ReferenceParameters)
			{
				auto x = _doc.NewElement("wsa5:ReferenceParameters");
				auto y = _doc.NewElement("chan:ChannelInstance");
				y->SetText(item->Event.ProducerReference->ReferenceParameters->chan__ChannelInstance);
				x->InsertEndChild(y);
				a->InsertEndChild(x);
			}
			if (item->Event.ProducerReference->Metadata)
			{
				auto x = _doc.NewElement("wsa5:Metadata");
				a->InsertEndChild(x);
			}

			eventNode->InsertEndChild(a);
		}
		if (1)
		{
			auto a = _doc.NewElement("wsnt:Message");
			eventNode->InsertEndChild(a);
		}

		if (item->StartStateEvent)
			stateEventNode->SetText("true");
		else
			stateEventNode->SetText("false");
		resNode->InsertEndChild(rTokenNode);
		resNode->InsertEndChild(trackNode);
		resNode->InsertEndChild(timeNode);
		resNode->InsertEndChild(eventNode);
		resNode->InsertEndChild(stateEventNode);
		resListNode->InsertEndChild(resNode);
	}

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

int soapSearch::OnGetRecordingInformation(void* req, std::string& body)
{
	_tse__GetRecordingInformationResponse resp;
	resp.RecordingInformation = std::make_shared<tt__RecordingInformation>();
	resp.RecordingInformation->RecordingToken = "Record_0";
	resp.RecordingInformation->Source = std::make_shared<tt__RecordingSourceInformation>();	
	resp.RecordingInformation->Source->SourceId = "source_0";
	resp.RecordingInformation->Source->Name = "ipc";
	resp.RecordingInformation->Source->Location = "location";
	resp.RecordingInformation->Source->Description = "desc";
	resp.RecordingInformation->Source->Address= "address";
	resp.RecordingInformation->Content = "local record";
	resp.RecordingInformation->EarliestRecording = std::make_shared<time_t>(time(NULL));
	resp.RecordingInformation->LatestRecording = std::make_shared<time_t>(time(NULL));
	std::shared_ptr<tt__TrackInformation> videoTrack(new tt__TrackInformation);
	videoTrack->DataFrom = time(NULL);
	videoTrack->DataTo = time(NULL);
	videoTrack->TrackToken = "video_0";
	videoTrack->TrackType = "Video";
	videoTrack->Description = "video track";
	std::shared_ptr<tt__TrackInformation> audioTrack(new tt__TrackInformation);
	audioTrack->DataFrom = time(NULL);
	audioTrack->DataTo = time(NULL);
	audioTrack->TrackToken = "audio_0";
	audioTrack->TrackType = "Audio";
	audioTrack->Description = "audio track";
	resp.RecordingInformation->Track.emplace_back(std::move(videoTrack));
	resp.RecordingInformation->Track.emplace_back(std::move(audioTrack));
	resp.RecordingInformation->RecordingStatus = tt__RecordingStatus__Stopped;
	body = std::move(resp.Serialization());
	return 200;
}

int soapSearch::OnGetRecordingSearchResults(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string SearchToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getResNode = child->FirstChildElement();
		if (getResNode && strstr(getResNode->Value(), "GetRecordingSearchResults"))
		{
			for (auto i = getResNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "SearchToken"))
					SearchToken = i->GetText();
			}
		}
	}
	printf("searchToken %s\n", SearchToken.c_str());

	auto t = time(NULL);
	_tse__GetRecordingSearchResultsResponse resp;
	resp.ResultList.SearchState = tt__SearchState__Completed;
	std::shared_ptr<tt__RecordingInformation> info(new tt__RecordingInformation);
	info->Content = "local record";
	info->RecordingStatus = tt__RecordingStatus__Recording;
	info->RecordingToken = "Record_0";
	info->Source = std::make_shared<tt__RecordingSourceInformation>();
	info->Source->SourceId = "source_0";
	info->Source->Name = "ipc";
	info->Source->Location = "location";
	info->Source->Description = "desc";
	info->Source->Address = "http://www.onvif.org/ver10/schema/Profile";
	info->EarliestRecording = std::make_shared<time_t>(t);
	info->LatestRecording = std::make_shared<time_t>(t);
	std::shared_ptr<tt__TrackInformation> videoTrack(new tt__TrackInformation);
	videoTrack->DataFrom = t;//录像头时间
	videoTrack->DataTo = t;//录像尾时间
	videoTrack->TrackToken = "video_0";
	videoTrack->TrackType = "Video";
	videoTrack->Description = "video track";
	std::shared_ptr<tt__TrackInformation> audioTrack(new tt__TrackInformation);
	audioTrack->DataFrom = t;//录像头时间
	audioTrack->DataTo = t;//录像尾时间
	audioTrack->TrackToken = "audio_0";
	audioTrack->TrackType = "Audio";
	audioTrack->Description = "audio track";
	info->Track.emplace_back(std::move(videoTrack));
	info->Track.emplace_back(std::move(audioTrack));
	resp.ResultList.RecordingInformation.emplace_back(std::move(info));
	body = std::move(resp.Serialization());
	return 200;
}

int soapSearch::OnGetRecordingSummary(void* req, std::string& body)
{
	_tse__GetRecordingSummaryResponse resp;
	resp.Summary = std::make_shared<tt__RecordingSummary>();
	resp.Summary->DataFrom = time(NULL) - 12 * 60 * 60;//录像头时间
	resp.Summary->DataUntil = time(NULL);//录像尾时间
	resp.Summary->NumberRecordings = 1;
	body = std::move(resp.Serialization());
	return 200;
}

int soapSearch::OnFindRecordings(void* req, std::string& body)
{
	_tse__FindRecordings frd;
	frd.UnSerialize(body);
	
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tse=\"http://www.onvif.org/ver10/search/wsdl\">");
	body.append("<s:Header/><s:Body><tse:FindRecordingsResponse><tse:SearchToken>");
	body.append("search_0");//token
	body.append("</tse:SearchToken></tse:FindRecordingsResponse></s:Body></s:Envelope>\n");
	return 200;
}

int soapSearch::OnEndSearch(void* req, std::string& body)
{
	_tse__EndSearchResponse resp;
	resp.Endpoint = time(NULL);
	body = std::move(resp.Serialization());
	return 200;
}

int soapSearch::OnGetPTZPositionSearchResults(void* req, std::string& body)
{
	return 200;
}

int soapSearch::OnFindPTZPosition(void* req, std::string& body)
{
	return 200;
}

int soapSearch::OnFindEvents(void* req, std::string& body)
{
	_tse__FindEvents fe;
	fe.UnSerialize(body);

	printf("%ld %s\n", fe.StartPoint, fe.SearchFilter.c_str());

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tse=\"http://www.onvif.org/ver10/search/wsdl\">");
	body.append("<s:Header/><s:Body><tse:FindEventsResponse><tse:SearchToken>");
	body.append("search_0");//token
	body.append("</tse:SearchToken></tse:FindEventsResponse></s:Body></s:Envelope>\n");
	return 200;
}

int soapSearch::OnGetEventSearchResults(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string SearchToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getResNode = child->FirstChildElement();
		if (getResNode && strstr(getResNode->Value(), "GetEventSearchResults"))
		{
			for (auto i = getResNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "SearchToken"))
					SearchToken = i->GetText();
			}
		}
	}
	printf("searchToken %s\n", SearchToken.c_str());

	_tse__GetEventSearchResultsResponse resp;
	resp.ResultList.SearchState = tt__SearchState__Completed;
	
	/*std::shared_ptr<tt__FindEventResult> item1(new tt__FindEventResult);
	item1->RecordingToken = "Record_0";
	item1->TrackToken = "track_0";
	item1->Time = time(NULL);
	item1->StartStateEvent = false;
	item1->Event.ProducerReference.reset(new wsa5__EndpointReferenceType);
	item1->Event.ProducerReference->Address = "address";
	item1->Event.ProducerReference->ReferenceParameters.reset(new wsa5__ReferenceParametersType);
	item1->Event.ProducerReference->ReferenceParameters->chan__ChannelInstance = 2;
	item1->Event.ProducerReference->Metadata.reset(new wsa5__MetadataType);

	item1->Event.SubscriptionReference.reset(new wsa5__EndpointReferenceType);
	item1->Event.SubscriptionReference->Address = "address";
	item1->Event.SubscriptionReference->ReferenceParameters.reset(new wsa5__ReferenceParametersType);
	item1->Event.SubscriptionReference->ReferenceParameters->chan__ChannelInstance = 2;
	item1->Event.SubscriptionReference->Metadata.reset(new wsa5__MetadataType);

	item1->Event.Topic.reset(new wsnt__TopicExpressionType);
	item1->Event.Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
	resp.ResultList.Result.push_back(item1);*/

	body = std::move(resp.Serialization());
	return 200;
}