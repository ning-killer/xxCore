#include "soapMedia.h"
#include "soapdefine.h"
#include <iostream>
#include <sstream>
#include "core/EmxParam.hpp"
#include "core/msg/MsgRpc.hpp"
#include "EmxMedia.hpp"

#define VSCMToken  "00000"
#define VSCSToken  "00001"
#define ASCToken   "00002"
#define VECMToken  "00003"
#define VECSToken  "00004"
#define AECToken   "00005"

BaseMediaResponse::~BaseMediaResponse()
{
}

tinyxml2::XMLElement* BaseMediaResponse::InitHeader(const std::string& tag)
{
	auto declare = _doc.NewDeclaration();
	_doc.InsertFirstChild(declare);

	/*Envelope*/
	auto envelopeNode = _doc.NewElement("s:Envelope");
	_doc.InsertEndChild(envelopeNode);
	envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
	envelopeNode->SetAttribute("xmlns:sc", "http://www.w3.org/2003/05/soap-encoding");
	envelopeNode->SetAttribute("xmlns:tt", "http://www.onvif.org/ver10/schema");
	envelopeNode->SetAttribute("xmlns:trt", "http://www.onvif.org/ver10/media/wsdl");
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

std::string _trt__GetProfilesResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetProfilesResponse");
	for (size_t i = 0; i < Profiles.size(); i++)
	{
		auto profilesNode =  _doc.NewElement("trt:Profiles");
		profilesNode->SetAttribute("token", Profiles[i]->token.c_str());
		profilesNode->SetAttribute("fixed", Profiles[i]->fixed);
		respNode->InsertEndChild(profilesNode);
		auto nameNode = _doc.NewElement("tt:Name");
		nameNode->SetText(Profiles[i]->Name.c_str());
		profilesNode->InsertEndChild(nameNode);
		/*VideoSourceConfiguration*/
		if (Profiles[i]->VideoSourceConfiguration)
		{
			auto vConfNode = _doc.NewElement("tt:VideoSourceConfiguration");
			vConfNode->SetAttribute("token", Profiles[i]->VideoSourceConfiguration->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->VideoSourceConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->VideoSourceConfiguration->UseCount);
			auto srcTokenNode = _doc.NewElement("tt:SourceToken");
			srcTokenNode->SetText(Profiles[i]->VideoSourceConfiguration->SourceToken.c_str());
			auto boundsNode = _doc.NewElement("tt:Bounds");
			boundsNode->SetAttribute("x", Profiles[i]->VideoSourceConfiguration->Bounds.x);
			boundsNode->SetAttribute("y", Profiles[i]->VideoSourceConfiguration->Bounds.y);
			boundsNode->SetAttribute("width", Profiles[i]->VideoSourceConfiguration->Bounds.width);
			boundsNode->SetAttribute("height", Profiles[i]->VideoSourceConfiguration->Bounds.height);
			vConfNode->InsertEndChild(nNode);
			vConfNode->InsertEndChild(useCountNode);
			vConfNode->InsertEndChild(srcTokenNode);
			vConfNode->InsertEndChild(boundsNode);
			profilesNode->InsertEndChild(vConfNode);
		}
		/*AudioSourceConfiguration*/
		if (Profiles[i]->AudioSourceConfiguration)
		{
			auto aConfNode = _doc.NewElement("tt:AudioSourceConfiguration");
			aConfNode->SetAttribute("token", Profiles[i]->AudioSourceConfiguration->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->AudioSourceConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->AudioSourceConfiguration->UseCount);
			auto srcTokenNode = _doc.NewElement("tt:SourceToken");
			srcTokenNode->SetText(Profiles[i]->AudioSourceConfiguration->SourceToken.c_str());
			aConfNode->InsertEndChild(nNode);
			aConfNode->InsertEndChild(useCountNode);
			aConfNode->InsertEndChild(srcTokenNode);
			profilesNode->InsertEndChild(aConfNode);
		}
		/*VideoEncoderConfiguration*/
		if (Profiles[i]->VideoEncoderConfiguration)
		{
			auto vecNode = _doc.NewElement("tt:VideoEncoderConfiguration");
			vecNode->SetAttribute("token", Profiles[i]->VideoEncoderConfiguration->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->VideoEncoderConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->VideoEncoderConfiguration->UseCount);
			auto encodingNode = _doc.NewElement("tt:Encoding");
			encodingNode->SetText(Profiles[i]->VideoEncoderConfiguration->Encoding.c_str());

			auto resolutionNode = _doc.NewElement("tt:Resolution");
			auto wNode = _doc.NewElement("tt:Width");
			wNode->SetText(Profiles[i]->VideoEncoderConfiguration->Width);
			auto hNode = _doc.NewElement("tt:Height");
			hNode->SetText(Profiles[i]->VideoEncoderConfiguration->Height);
			resolutionNode->InsertEndChild(wNode);
			resolutionNode->InsertEndChild(hNode);

			auto qualityNode = _doc.NewElement("tt:Quality");
            qualityNode->SetText(Profiles[i]->VideoEncoderConfiguration->Quality);
			//qualityNode->SetText(std::to_string(Profiles[i]->VideoEncoderConfiguration->Quality).c_str());
			vecNode->InsertEndChild(nNode);
			vecNode->InsertEndChild(useCountNode);
			vecNode->InsertEndChild(encodingNode);
			vecNode->InsertEndChild(resolutionNode);
			vecNode->InsertEndChild(qualityNode);

			if (Profiles[i]->VideoEncoderConfiguration->RateControl)
			{
				auto rcNode = _doc.NewElement("tt:RateControl");
				auto frlNode = _doc.NewElement("tt:FrameRateLimit");
				frlNode->SetText(Profiles[i]->VideoEncoderConfiguration->RateControl->FrameRateLimit);
				auto eiNode = _doc.NewElement("tt:EncodingInterval");
				eiNode->SetText(Profiles[i]->VideoEncoderConfiguration->RateControl->EncodingInterval);
				auto blNode = _doc.NewElement("tt:BitrateLimit");
				blNode->SetText(Profiles[i]->VideoEncoderConfiguration->RateControl->BitrateLimit);
				rcNode->InsertEndChild(frlNode);
				rcNode->InsertEndChild(eiNode);
				rcNode->InsertEndChild(blNode);
				vecNode->InsertEndChild(rcNode);
			}
			if (Profiles[i]->VideoEncoderConfiguration->H264)
			{
				auto h264Node = _doc.NewElement("tt:H264");
				auto govLengthNode = _doc.NewElement("tt:GovLength");
				govLengthNode->SetText(Profiles[i]->VideoEncoderConfiguration->H264->GovLength);
				auto hpNode = _doc.NewElement("tt:H264Profile");
				switch (Profiles[i]->VideoEncoderConfiguration->H264->H264Profile)
				{
				case tt__H264Profile__Baseline:
					hpNode->SetText("Baseline");
					break;
				case tt__H264Profile__Main:
					hpNode->SetText("Main");
					break;
				case tt__H264Profile__Extended:
					hpNode->SetText("Extended");
					break;
				case tt__H264Profile__High:
					hpNode->SetText("High");
					break;
				}
				h264Node->InsertEndChild(govLengthNode);
				h264Node->InsertEndChild(hpNode);
				vecNode->InsertEndChild(h264Node);
			}

			auto multicastNode = _doc.NewElement("tt:Multicast");
			auto addressNode = _doc.NewElement("tt:Address");
			auto typeNode = _doc.NewElement("tt:Type");
			typeNode->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.Address->Type.c_str());
			addressNode->InsertEndChild(typeNode);
			if (Profiles[i]->VideoEncoderConfiguration->Multicast.Address->IPv4Address)
			{
				auto ipv4Node = _doc.NewElement("tt:IPv4Address");
				ipv4Node->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.Address->IPv4Address->c_str());
				addressNode->InsertEndChild(ipv4Node);
			}
			if (Profiles[i]->VideoEncoderConfiguration->Multicast.Address->IPv6Address)
			{
				auto ipv6Node = _doc.NewElement("tt:IPv4Address");
				ipv6Node->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.Address->IPv6Address->c_str());
				addressNode->InsertEndChild(ipv6Node);
			}
			auto portNode = _doc.NewElement("tt:Port");
			portNode->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.Port);
			auto ttlNode = _doc.NewElement("tt:TTL");
			ttlNode->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.TTL);
			auto autoStartNode = _doc.NewElement("tt:AutoStart");
			autoStartNode->SetText(Profiles[i]->VideoEncoderConfiguration->Multicast.AutoStart);
			multicastNode->InsertEndChild(addressNode);
			multicastNode->InsertEndChild(portNode);
			multicastNode->InsertEndChild(ttlNode);
			multicastNode->InsertEndChild(autoStartNode);
			vecNode->InsertEndChild(multicastNode);

			auto ptNode = _doc.NewElement("tt:SessionTimeout");
			ptNode->SetText(Profiles[i]->VideoEncoderConfiguration->SessionTimeout.c_str());
			vecNode->InsertEndChild(ptNode);
			profilesNode->InsertEndChild(vecNode);
		}
		/*AudioEncoderConfiguration*/
		if (Profiles[i]->AudioEncoderConfiguration)
		{
			auto aecNode = _doc.NewElement("tt:AudioEncoderConfiguration");
			aecNode->SetAttribute("token", Profiles[i]->AudioEncoderConfiguration->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->AudioEncoderConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->AudioEncoderConfiguration->UseCount);
			auto encodingNode = _doc.NewElement("tt:Encoding");
			encodingNode->SetText(Profiles[i]->AudioEncoderConfiguration->Encoding.c_str());
			auto bitrateNode = _doc.NewElement("tt:Bitrate");
			bitrateNode->SetText(Profiles[i]->AudioEncoderConfiguration->Bitrate);
			auto sampleRateNode = _doc.NewElement("tt:SampleRate");
			sampleRateNode->SetText(Profiles[i]->AudioEncoderConfiguration->SampleRate);
			aecNode->InsertEndChild(nNode);
			aecNode->InsertEndChild(useCountNode);
			aecNode->InsertEndChild(encodingNode);
			aecNode->InsertEndChild(bitrateNode);
			aecNode->InsertEndChild(sampleRateNode);

			auto multicastNode = _doc.NewElement("tt:Multicast");
			auto addressNode = _doc.NewElement("tt:Address");
			auto typeNode = _doc.NewElement("tt:Type");
			typeNode->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.Address->Type.c_str());
			addressNode->InsertEndChild(typeNode);
			if (Profiles[i]->AudioEncoderConfiguration->Multicast.Address->IPv4Address)
			{
				auto ipv4Node = _doc.NewElement("tt:IPv4Address");
				ipv4Node->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.Address->IPv4Address->c_str());
				addressNode->InsertEndChild(ipv4Node);
			}
			if (Profiles[i]->AudioEncoderConfiguration->Multicast.Address->IPv6Address)
			{
				auto ipv6Node = _doc.NewElement("tt:IPv4Address");
				ipv6Node->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.Address->IPv6Address->c_str());
				addressNode->InsertEndChild(ipv6Node);
			}
			auto portNode = _doc.NewElement("tt:Port");
			portNode->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.Port);
			auto ttlNode = _doc.NewElement("tt:TTL");
			ttlNode->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.TTL);
			auto autoStartNode = _doc.NewElement("tt:AutoStart");
			autoStartNode->SetText(Profiles[i]->AudioEncoderConfiguration->Multicast.AutoStart);
			multicastNode->InsertEndChild(addressNode);
			multicastNode->InsertEndChild(portNode);
			multicastNode->InsertEndChild(ttlNode);
			multicastNode->InsertEndChild(autoStartNode);
			aecNode->InsertEndChild(multicastNode);

			auto ptNode = _doc.NewElement("tt:SessionTimeout");
			ptNode->SetText(Profiles[i]->AudioEncoderConfiguration->SessionTimeout.c_str());
			aecNode->InsertEndChild(ptNode);
			profilesNode->InsertEndChild(aecNode);
		}
		if (Profiles[i]->VideoAnalyticsConfiguration)
		{
			auto aNode = _doc.NewElement("tt:VideoAnalyticsConfiguration");
			aNode->SetAttribute("token", Profiles[i]->VideoAnalyticsConfiguration->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->VideoAnalyticsConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->VideoAnalyticsConfiguration->UseCount);
			aNode->InsertEndChild(nNode);
			aNode->InsertEndChild(useCountNode);

			auto bNode = _doc.NewElement("tt:AnalyticsEngineConfiguration");
			aNode->InsertEndChild(bNode);
			for (size_t j = 0; j < Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule.size(); j++)
			{
				auto modeNode = _doc.NewElement("tt:AnalyticsModule");
				modeNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Name.c_str());
				modeNode->SetAttribute("Type", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Type.c_str());

				auto paramNode = _doc.NewElement("tt:Parameters");
				modeNode->InsertEndChild(paramNode);
				for (size_t k = 0; k < Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.SimpleItem.size(); k++)
				{
					auto itemNode = _doc.NewElement("tt:SimpleItem");
					itemNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.SimpleItem[k].Name.c_str());
					itemNode->SetAttribute("Value", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.SimpleItem[k].Value.c_str());
					paramNode->InsertEndChild(itemNode);
				}
				for (size_t k = 0; k < Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem.size(); k++)
				{
					auto itemNode = _doc.NewElement("tt:ElementItem");
					itemNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Name.c_str());
					auto layoutNode = _doc.NewElement("tt:CellLayout");
					layoutNode->SetAttribute("Columns", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Columns.c_str());
					layoutNode->SetAttribute("Rows", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Rows.c_str());
					auto transformationNode = _doc.NewElement("tt:Transformation");
					auto translateNode = _doc.NewElement("tt:Translate");
					translateNode->SetAttribute("x", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Transformation.Translate.x);
					translateNode->SetAttribute("y", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Transformation.Translate.y);
					auto scaleNode = _doc.NewElement("tt:Scale");	
					scaleNode->SetAttribute("x", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Transformation.Scale.x);
					scaleNode->SetAttribute("y", Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout.Transformation.Scale.y);

					transformationNode->InsertEndChild(translateNode);
					transformationNode->InsertEndChild(scaleNode);
					layoutNode->InsertEndChild(transformationNode);
					itemNode->InsertEndChild(layoutNode);
					Profiles[i]->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule[j]->Parameters.ElementItem[k].Layout;
					paramNode->InsertEndChild(itemNode);
				}
				bNode->InsertEndChild(modeNode);
			}

			auto cNode = _doc.NewElement("tt:RuleEngineConfiguration");
			aNode->InsertEndChild(cNode);
			for (size_t j = 0; j < Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule.size(); j++)
			{
				auto ruleNode = _doc.NewElement("tt:Rule");
				ruleNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Name.c_str());
				ruleNode->SetAttribute("Type", Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Type.c_str());
				auto paramNode = _doc.NewElement("tt:Parameters");
				ruleNode->InsertEndChild(paramNode);
				for (size_t k = 0; k < Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Parameters.SimpleItem.size(); k++)
				{
					auto itemNode = _doc.NewElement("tt:SimpleItem");
					itemNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Parameters.SimpleItem[k].Name.c_str());
					itemNode->SetAttribute("Value", Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Parameters.SimpleItem[k].Value.c_str());
					paramNode->InsertEndChild(itemNode);
				}
				for (size_t k = 0; k < Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Parameters.ElementItem.size(); k++)
				{
					auto itemNode = _doc.NewElement("tt:ElementItem");
					itemNode->SetAttribute("Name", Profiles[i]->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule[j]->Parameters.ElementItem[k].Name.c_str());
					paramNode->InsertEndChild(itemNode);
				}
				cNode->InsertEndChild(ruleNode);
			}
			profilesNode->InsertEndChild(aNode);
		}
		/*MetadataConfiguration*/
		if (Profiles[i]->MetadataConfiguration)
		{
			auto metaDataNode = _doc.NewElement("tt:MetadataConfiguration");
			metaDataNode->SetAttribute("token", Profiles[i]->MetadataConfiguration->token.c_str());
			metaDataNode->SetAttribute("CompressionType", "");
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(Profiles[i]->MetadataConfiguration->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(Profiles[i]->MetadataConfiguration->UseCount);
			metaDataNode->InsertEndChild(nNode);
			metaDataNode->InsertEndChild(useCountNode);

			if (Profiles[i]->MetadataConfiguration->PTZStatus)
			{
				auto ptzStatusNode = _doc.NewElement("tt:PTZStatus");
				auto statusNode = _doc.NewElement("tt:Status");
				statusNode->SetText(Profiles[i]->MetadataConfiguration->PTZStatus->Status);
				auto positionNode = _doc.NewElement("tt:Position");
				positionNode->SetText(Profiles[i]->MetadataConfiguration->PTZStatus->Position);
				ptzStatusNode->InsertEndChild(statusNode);
				ptzStatusNode->InsertEndChild(positionNode);
				metaDataNode->InsertEndChild(ptzStatusNode);
			}
			if (Profiles[i]->MetadataConfiguration->Events)
			{
				auto eventNode = _doc.NewElement("tt:Events");
				auto filterNode = _doc.NewElement("tt:Filter");
				auto topicNode = _doc.NewElement("wsnt:TopicExpression");
				topicNode->SetAttribute("Dialect", Profiles[i]->MetadataConfiguration->Events->Filter.TopicExpression.Dialect.c_str());
				topicNode->SetAttribute("xmlns:tns1", "http://www.onvif.org/ver10/topics");
				topicNode->SetAttribute("xmlns:wsnt", "http://docs.oasis-open.org/wsn/b-2");
				topicNode->SetText(Profiles[i]->MetadataConfiguration->Events->Filter.TopicExpression.v.c_str());

				filterNode->InsertEndChild(topicNode);
				eventNode->InsertEndChild(filterNode);
				metaDataNode->InsertEndChild(eventNode);
			}
			auto analyticsNode = _doc.NewElement("tt:Analytics");
			analyticsNode->SetText(Profiles[i]->MetadataConfiguration->Analytics);
			metaDataNode->InsertEndChild(analyticsNode);

			auto multicastNode = _doc.NewElement("tt:Multicast");
			auto addressNode = _doc.NewElement("tt:Address");
			auto typeNode = _doc.NewElement("tt:Type");
			typeNode->SetText(Profiles[i]->MetadataConfiguration->Multicast.Address->Type.c_str());
			addressNode->InsertEndChild(typeNode);
			if (Profiles[i]->MetadataConfiguration->Multicast.Address->IPv4Address)
			{
				auto ipv4Node = _doc.NewElement("tt:IPv4Address");
				ipv4Node->SetText(Profiles[i]->MetadataConfiguration->Multicast.Address->IPv4Address->c_str());
				addressNode->InsertEndChild(ipv4Node);
			}
			if (Profiles[i]->MetadataConfiguration->Multicast.Address->IPv6Address)
			{
				auto ipv6Node = _doc.NewElement("tt:IPv4Address");
				ipv6Node->SetText(Profiles[i]->MetadataConfiguration->Multicast.Address->IPv6Address->c_str());
				addressNode->InsertEndChild(ipv6Node);
			}
			auto portNode = _doc.NewElement("tt:Port");
			portNode->SetText(Profiles[i]->MetadataConfiguration->Multicast.Port);
			auto ttlNode = _doc.NewElement("tt:TTL");
			ttlNode->SetText(Profiles[i]->MetadataConfiguration->Multicast.TTL);
			auto autoStartNode = _doc.NewElement("tt:AutoStart");
			autoStartNode->SetText(Profiles[i]->MetadataConfiguration->Multicast.AutoStart);
			multicastNode->InsertEndChild(addressNode);
			multicastNode->InsertEndChild(portNode);
			multicastNode->InsertEndChild(ttlNode);
			multicastNode->InsertEndChild(autoStartNode);
			metaDataNode->InsertEndChild(multicastNode);

			auto ptNode = _doc.NewElement("tt:SessionTimeout");
			ptNode->SetText(Profiles[i]->MetadataConfiguration->SessionTimeout.c_str());
			metaDataNode->InsertEndChild(ptNode);
			profilesNode->InsertEndChild(metaDataNode);
		}
		if (Profiles[i]->PTZConfiguration)
		{
			auto obj = Profiles[i]->PTZConfiguration;
			auto ptzNode = _doc.NewElement("tt:PTZConfiguration");
			ptzNode->SetAttribute("token", obj->token.c_str());
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(obj->Name.c_str());
			auto useCountNode = _doc.NewElement("tt:UseCount");
			useCountNode->SetText(obj->UseCount);
			auto tokenNode = _doc.NewElement("tt:NodeToken");
			tokenNode->SetText(obj->NodeToken.c_str());
			ptzNode->InsertEndChild(nNode);
			ptzNode->InsertEndChild(useCountNode);
			ptzNode->InsertEndChild(tokenNode);
			if (obj->DefaultAbsolutePantTiltPositionSpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultAbsolutePantTiltPositionSpace");
				aNode->SetText(obj->DefaultAbsolutePantTiltPositionSpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultAbsoluteZoomPositionSpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultAbsoluteZoomPositionSpace");
				aNode->SetText(obj->DefaultAbsoluteZoomPositionSpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultRelativePanTiltTranslationSpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultRelativePanTiltTranslationSpace");
				aNode->SetText(obj->DefaultRelativePanTiltTranslationSpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultRelativeZoomTranslationSpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultRelativeZoomTranslationSpace");
				aNode->SetText(obj->DefaultRelativeZoomTranslationSpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultContinuousPanTiltVelocitySpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultContinuousPanTiltVelocitySpace");
				aNode->SetText(obj->DefaultContinuousPanTiltVelocitySpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultContinuousZoomVelocitySpace)
			{
				auto aNode = _doc.NewElement("tt:DefaultContinuousZoomVelocitySpace");
				aNode->SetText(obj->DefaultContinuousZoomVelocitySpace->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultPTZSpeed)
			{
				auto aNode = _doc.NewElement("tt:DefaultPTZSpeed");
				if (obj->DefaultPTZSpeed->PanTilt)
				{
					auto bNode = _doc.NewElement("tt:PanTilt");
					bNode->SetAttribute("x", obj->DefaultPTZSpeed->PanTilt->x);
					bNode->SetAttribute("y", obj->DefaultPTZSpeed->PanTilt->y);
					if (obj->DefaultPTZSpeed->PanTilt->space)
						bNode->SetAttribute("space", obj->DefaultPTZSpeed->PanTilt->space->c_str());

					aNode->InsertEndChild(bNode);
				}
				if (obj->DefaultPTZSpeed->Zoom)
				{
					auto bNode = _doc.NewElement("tt:Zoom");
					bNode->SetAttribute("x", obj->DefaultPTZSpeed->Zoom->x);
					if (obj->DefaultPTZSpeed->Zoom->space)
						bNode->SetAttribute("space", obj->DefaultPTZSpeed->Zoom->space->c_str());

					aNode->InsertEndChild(bNode);
				}
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->DefaultPTZTimeout)
			{
				auto aNode = _doc.NewElement("tt:DefaultPTZTimeout");
				aNode->SetText(obj->DefaultPTZTimeout->c_str());
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->PanTiltLimits)
			{
				auto aNode = _doc.NewElement("tt:PanTiltLimits");

				auto rangeNode = _doc.NewElement("tt:Range");
				auto uriNode = _doc.NewElement("tt:URI");
				uriNode->SetText(obj->PanTiltLimits->Range.URI.c_str());

				auto xrangeNode = _doc.NewElement("tt:XRange");
				auto xMinNode = _doc.NewElement("tt:Min");
				xMinNode->SetText(obj->PanTiltLimits->Range.xMin);
				auto xMaxNode = _doc.NewElement("tt:Max");
				xMaxNode->SetText(obj->PanTiltLimits->Range.xMax);
				xrangeNode->InsertEndChild(xMinNode);
				xrangeNode->InsertEndChild(xMaxNode);

				auto yrangeNode = _doc.NewElement("tt:YRange");
				auto yMinNode = _doc.NewElement("tt:Min");
				yMinNode->SetText(obj->PanTiltLimits->Range.yMin);
				auto yMaxNode = _doc.NewElement("tt:Max");
				yMaxNode->SetText(obj->PanTiltLimits->Range.yMax);
				yrangeNode->InsertEndChild(yMinNode);
				yrangeNode->InsertEndChild(yMaxNode);
					
				rangeNode->InsertEndChild(uriNode);
				rangeNode->InsertEndChild(xrangeNode);
				rangeNode->InsertEndChild(yrangeNode);
				aNode->InsertEndChild(rangeNode);
				ptzNode->InsertEndChild(aNode);
			}
			if (obj->ZoomLimits)
			{
				auto aNode = _doc.NewElement("tt:ZoomLimits");
				auto rangeNode = _doc.NewElement("tt:Range");
				auto uriNode = _doc.NewElement("tt:URI");
				uriNode->SetText(obj->ZoomLimits->Range.URI.c_str());

				auto xrangeNode = _doc.NewElement("tt:XRange");
				auto xMinNode = _doc.NewElement("tt:Min");
				xMinNode->SetText(obj->ZoomLimits->Range.Min);
				auto xMaxNode = _doc.NewElement("tt:Max");
				xMaxNode->SetText(obj->ZoomLimits->Range.Max);
				xrangeNode->InsertEndChild(xMinNode);
				xrangeNode->InsertEndChild(xMaxNode);

				rangeNode->InsertEndChild(uriNode);
				rangeNode->InsertEndChild(xrangeNode);
				aNode->InsertEndChild(rangeNode);
				ptzNode->InsertEndChild(aNode);
			}
			profilesNode->InsertEndChild(ptzNode);
		}
	}

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetStreamUriResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetStreamUriResponse");
	auto mediaUriNode = _doc.NewElement("trt:MediaUri");
	respNode->InsertEndChild(mediaUriNode);
	auto uriNode = _doc.NewElement("tt:Uri");
	uriNode->SetText(MediaUri.Uri.c_str());
	auto iacNode = _doc.NewElement("tt:InvalidAfterConnect");
	iacNode->SetText(MediaUri.InvalidAfterConnect);
	auto iarNode = _doc.NewElement("tt:InvalidAfterReboot");
	iarNode->SetText(MediaUri.InvalidAfterReboot);
	auto timeoutNode = _doc.NewElement("tt:Timeout");
	timeoutNode->SetText(MediaUri.Timeout.c_str());

	mediaUriNode->InsertEndChild(uriNode);
	mediaUriNode->InsertEndChild(iacNode);
	mediaUriNode->InsertEndChild(iarNode);
	mediaUriNode->InsertEndChild(timeoutNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetVideoSourcesResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetVideoSourcesResponse");
	for (size_t i = 0; i < VideoSources.size(); i++)
	{
		auto a = _doc.NewElement("trt:VideoSources");
		a->SetAttribute("token", VideoSources[i]->token.c_str());
		auto f = _doc.NewElement("tt:Framerate");
		f->SetText(std::to_string(VideoSources[i]->Framerate).c_str());
		auto r = _doc.NewElement("tt:Resolution");
		auto w = _doc.NewElement("tt:Width");
		w->SetText(VideoSources[i]->Width);
		auto h = _doc.NewElement("tt:Height");
		h->SetText(VideoSources[i]->Heigth);
		r->InsertEndChild(w);
		r->InsertEndChild(h);
		a->InsertEndChild(f);
		a->InsertEndChild(r);
		/*if (VideoSources[i]->Imaging)
		{

		}*/
		respNode->InsertEndChild(a);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetAudioSourcesResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetAudioSourcesResponse");
	for (size_t i = 0; i < AudioSources.size(); i++)
	{
		auto a = _doc.NewElement("trt:AudioSources");
		a->SetAttribute("token", AudioSources[i]->token.c_str());
		auto c = _doc.NewElement("tt:Channels");
		c->SetText(AudioSources[i]->Channels);
		a->InsertEndChild(c);
		respNode->InsertEndChild(a);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _trt__GetProfileResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetProfileResponse");
	auto profilesNode = _doc.NewElement("trt:Profile");
	profilesNode->SetAttribute("token", Profile->token.c_str());
	profilesNode->SetAttribute("fixed", Profile->fixed);
	respNode->InsertEndChild(profilesNode);
	auto nameNode = _doc.NewElement("tt:Name");
	nameNode->SetText(Profile->Name.c_str());
	profilesNode->InsertEndChild(nameNode);
	/*VideoSourceConfiguration*/
	if (Profile->VideoSourceConfiguration)
	{
		auto vConfNode = _doc.NewElement("tt:VideoSourceConfiguration");
		vConfNode->SetAttribute("token", Profile->VideoSourceConfiguration->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(Profile->VideoSourceConfiguration->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(Profile->VideoSourceConfiguration->UseCount);
		auto srcTokenNode = _doc.NewElement("tt:SourceToken");
		srcTokenNode->SetText(Profile->VideoSourceConfiguration->SourceToken.c_str());
		auto boundsNode = _doc.NewElement("tt:Bounds");
		boundsNode->SetAttribute("x", Profile->VideoSourceConfiguration->Bounds.x);
		boundsNode->SetAttribute("y", Profile->VideoSourceConfiguration->Bounds.y);
		boundsNode->SetAttribute("width", Profile->VideoSourceConfiguration->Bounds.width);
		boundsNode->SetAttribute("height", Profile->VideoSourceConfiguration->Bounds.height);
		vConfNode->InsertEndChild(nNode);
		vConfNode->InsertEndChild(useCountNode);
		vConfNode->InsertEndChild(srcTokenNode);
		vConfNode->InsertEndChild(boundsNode);
		profilesNode->InsertEndChild(vConfNode);
	}
	/*AudioSourceConfiguration*/
	if (Profile->AudioSourceConfiguration)
	{
		auto aConfNode = _doc.NewElement("tt:AudioSourceConfiguration");
		aConfNode->SetAttribute("token", Profile->AudioSourceConfiguration->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(Profile->AudioSourceConfiguration->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(Profile->AudioSourceConfiguration->UseCount);
		auto srcTokenNode = _doc.NewElement("tt:SourceToken");
		srcTokenNode->SetText(Profile->AudioSourceConfiguration->SourceToken.c_str());
		aConfNode->InsertEndChild(nNode);
		aConfNode->InsertEndChild(useCountNode);
		aConfNode->InsertEndChild(srcTokenNode);
		profilesNode->InsertEndChild(aConfNode);
	}
	/*VideoEncoderConfiguration*/
	if (Profile->VideoEncoderConfiguration)
	{
		auto vecNode = _doc.NewElement("tt:VideoEncoderConfiguration");
		vecNode->SetAttribute("token", Profile->VideoEncoderConfiguration->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(Profile->VideoEncoderConfiguration->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(Profile->VideoEncoderConfiguration->UseCount);
		auto encodingNode = _doc.NewElement("tt:Encoding");
		encodingNode->SetText(Profile->VideoEncoderConfiguration->Encoding.c_str());

		auto resolutionNode = _doc.NewElement("tt:Resolution");
		auto wNode = _doc.NewElement("tt:Width");
		wNode->SetText(Profile->VideoEncoderConfiguration->Width);
		auto hNode = _doc.NewElement("tt:Height");
		hNode->SetText(Profile->VideoEncoderConfiguration->Height);
		resolutionNode->InsertEndChild(wNode);
		resolutionNode->InsertEndChild(hNode);

		auto qualityNode = _doc.NewElement("tt:Quality");
        //qualityNode->SetText(std::to_string(Profile->VideoEncoderConfiguration->Quality).c_str());
        qualityNode->SetText(Profile->VideoEncoderConfiguration->Quality);
		vecNode->InsertEndChild(nNode);
		vecNode->InsertEndChild(useCountNode);
		vecNode->InsertEndChild(encodingNode);
		vecNode->InsertEndChild(resolutionNode);
		vecNode->InsertEndChild(qualityNode);

		if (Profile->VideoEncoderConfiguration->RateControl)
		{
			auto rcNode = _doc.NewElement("tt:RateControl");
			auto frlNode = _doc.NewElement("tt:FrameRateLimit");
			frlNode->SetText(Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit);
			auto eiNode = _doc.NewElement("tt:EncodingInterval");
			eiNode->SetText(Profile->VideoEncoderConfiguration->RateControl->EncodingInterval);
			auto blNode = _doc.NewElement("tt:BitrateLimit");
			blNode->SetText(Profile->VideoEncoderConfiguration->RateControl->BitrateLimit);
			rcNode->InsertEndChild(frlNode);
			rcNode->InsertEndChild(eiNode);
			rcNode->InsertEndChild(blNode);
			vecNode->InsertEndChild(rcNode);
		}
		if (Profile->VideoEncoderConfiguration->H264)
		{
			auto h264Node = _doc.NewElement("tt:H264");
			auto govLengthNode = _doc.NewElement("tt:GovLength");
			govLengthNode->SetText(Profile->VideoEncoderConfiguration->H264->GovLength);
			auto hpNode = _doc.NewElement("tt:H264Profile");
			switch (Profile->VideoEncoderConfiguration->H264->H264Profile)
			{
			case tt__H264Profile__Baseline:
				hpNode->SetText("Baseline");
				break;
			case tt__H264Profile__Main:
				hpNode->SetText("Main");
				break;
			case tt__H264Profile__Extended:
				hpNode->SetText("Extended");
				break;
			case tt__H264Profile__High:
				hpNode->SetText("High");
				break;
			}
			h264Node->InsertEndChild(govLengthNode);
			h264Node->InsertEndChild(hpNode);
			vecNode->InsertEndChild(h264Node);
		}

		auto multicastNode = _doc.NewElement("tt:Multicast");
		auto addressNode = _doc.NewElement("tt:Address");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(Profile->VideoEncoderConfiguration->Multicast.Address->Type.c_str());
		addressNode->InsertEndChild(typeNode);
		if (Profile->VideoEncoderConfiguration->Multicast.Address->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(Profile->VideoEncoderConfiguration->Multicast.Address->IPv4Address->c_str());
			addressNode->InsertEndChild(ipv4Node);
		}
		if (Profile->VideoEncoderConfiguration->Multicast.Address->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv4Address");
			ipv6Node->SetText(Profile->VideoEncoderConfiguration->Multicast.Address->IPv6Address->c_str());
			addressNode->InsertEndChild(ipv6Node);
		}
		auto portNode = _doc.NewElement("tt:Port");
		portNode->SetText(Profile->VideoEncoderConfiguration->Multicast.Port);
		auto ttlNode = _doc.NewElement("tt:TTL");
		ttlNode->SetText(Profile->VideoEncoderConfiguration->Multicast.TTL);
		auto autoStartNode = _doc.NewElement("tt:AutoStart");
		autoStartNode->SetText(Profile->VideoEncoderConfiguration->Multicast.AutoStart);
		multicastNode->InsertEndChild(addressNode);
		multicastNode->InsertEndChild(portNode);
		multicastNode->InsertEndChild(ttlNode);
		multicastNode->InsertEndChild(autoStartNode);
		vecNode->InsertEndChild(multicastNode);

		auto ptNode = _doc.NewElement("tt:SessionTimeout");
		ptNode->SetText(Profile->VideoEncoderConfiguration->SessionTimeout.c_str());
		vecNode->InsertEndChild(ptNode);
		profilesNode->InsertEndChild(vecNode);
	}
	/*AudioEncoderConfiguration*/
	if (Profile->AudioEncoderConfiguration)
	{
		auto aecNode = _doc.NewElement("tt:AudioEncoderConfiguration");
		aecNode->SetAttribute("token", Profile->AudioEncoderConfiguration->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(Profile->AudioEncoderConfiguration->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(Profile->AudioEncoderConfiguration->UseCount);
		auto encodingNode = _doc.NewElement("tt:Encoding");
		encodingNode->SetText(Profile->AudioEncoderConfiguration->Encoding.c_str());
		auto bitrateNode = _doc.NewElement("tt:Bitrate");
		bitrateNode->SetText(Profile->AudioEncoderConfiguration->Bitrate);
		auto sampleRateNode = _doc.NewElement("tt:SampleRate");
		sampleRateNode->SetText(Profile->AudioEncoderConfiguration->SampleRate);
		aecNode->InsertEndChild(nNode);
		aecNode->InsertEndChild(useCountNode);
		aecNode->InsertEndChild(encodingNode);
		aecNode->InsertEndChild(bitrateNode);
		aecNode->InsertEndChild(sampleRateNode);

		auto multicastNode = _doc.NewElement("tt:Multicast");
		auto addressNode = _doc.NewElement("tt:Address");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(Profile->AudioEncoderConfiguration->Multicast.Address->Type.c_str());
		addressNode->InsertEndChild(typeNode);
		if (Profile->AudioEncoderConfiguration->Multicast.Address->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(Profile->AudioEncoderConfiguration->Multicast.Address->IPv4Address->c_str());
			addressNode->InsertEndChild(ipv4Node);
		}
		if (Profile->AudioEncoderConfiguration->Multicast.Address->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv4Address");
			ipv6Node->SetText(Profile->AudioEncoderConfiguration->Multicast.Address->IPv6Address->c_str());
			addressNode->InsertEndChild(ipv6Node);
		}
		auto portNode = _doc.NewElement("tt:Port");
		portNode->SetText(Profile->AudioEncoderConfiguration->Multicast.Port);
		auto ttlNode = _doc.NewElement("tt:TTL");
		ttlNode->SetText(Profile->AudioEncoderConfiguration->Multicast.TTL);
		auto autoStartNode = _doc.NewElement("tt:AutoStart");
		autoStartNode->SetText(Profile->AudioEncoderConfiguration->Multicast.AutoStart);
		multicastNode->InsertEndChild(addressNode);
		multicastNode->InsertEndChild(portNode);
		multicastNode->InsertEndChild(ttlNode);
		multicastNode->InsertEndChild(autoStartNode);
		aecNode->InsertEndChild(multicastNode);

		auto ptNode = _doc.NewElement("tt:SessionTimeout");
		ptNode->SetText(Profile->AudioEncoderConfiguration->SessionTimeout.c_str());
		aecNode->InsertEndChild(ptNode);
		profilesNode->InsertEndChild(aecNode);
	}
	/*MetadataConfiguration*/
	if (Profile->MetadataConfiguration)
	{
		auto metaDataNode = _doc.NewElement("tt:MetadataConfiguration");
		metaDataNode->SetAttribute("token", Profile->MetadataConfiguration->token.c_str());
		metaDataNode->SetAttribute("CompressionType", "");
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(Profile->MetadataConfiguration->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(Profile->MetadataConfiguration->UseCount);
		metaDataNode->InsertEndChild(nNode);
		metaDataNode->InsertEndChild(useCountNode);

		if (Profile->MetadataConfiguration->PTZStatus)
		{
			auto ptzStatusNode = _doc.NewElement("tt:PTZStatus");
			auto statusNode = _doc.NewElement("tt:Status");
			statusNode->SetText(Profile->MetadataConfiguration->PTZStatus->Status);
			auto positionNode = _doc.NewElement("tt:Position");
			positionNode->SetText(Profile->MetadataConfiguration->PTZStatus->Position);
			ptzStatusNode->InsertEndChild(statusNode);
			ptzStatusNode->InsertEndChild(positionNode);
			metaDataNode->InsertEndChild(ptzStatusNode);
		}
		auto analyticsNode = _doc.NewElement("tt:Analytics");
		analyticsNode->SetText(Profile->MetadataConfiguration->Analytics);
		metaDataNode->InsertEndChild(analyticsNode);

		auto multicastNode = _doc.NewElement("tt:Multicast");
		auto addressNode = _doc.NewElement("tt:Address");
		auto typeNode = _doc.NewElement("tt:Type");
		typeNode->SetText(Profile->MetadataConfiguration->Multicast.Address->Type.c_str());
		addressNode->InsertEndChild(typeNode);
		if (Profile->MetadataConfiguration->Multicast.Address->IPv4Address)
		{
			auto ipv4Node = _doc.NewElement("tt:IPv4Address");
			ipv4Node->SetText(Profile->MetadataConfiguration->Multicast.Address->IPv4Address->c_str());
			addressNode->InsertEndChild(ipv4Node);
		}
		if (Profile->MetadataConfiguration->Multicast.Address->IPv6Address)
		{
			auto ipv6Node = _doc.NewElement("tt:IPv4Address");
			ipv6Node->SetText(Profile->MetadataConfiguration->Multicast.Address->IPv6Address->c_str());
			addressNode->InsertEndChild(ipv6Node);
		}
		auto portNode = _doc.NewElement("tt:Port");
		portNode->SetText(Profile->MetadataConfiguration->Multicast.Port);
		auto ttlNode = _doc.NewElement("tt:TTL");
		ttlNode->SetText(Profile->MetadataConfiguration->Multicast.TTL);
		auto autoStartNode = _doc.NewElement("tt:AutoStart");
		autoStartNode->SetText(Profile->MetadataConfiguration->Multicast.AutoStart);
		multicastNode->InsertEndChild(addressNode);
		multicastNode->InsertEndChild(portNode);
		multicastNode->InsertEndChild(ttlNode);
		multicastNode->InsertEndChild(autoStartNode);
		metaDataNode->InsertEndChild(multicastNode);

		auto ptNode = _doc.NewElement("tt:SessionTimeout");
		ptNode->SetText(Profile->MetadataConfiguration->SessionTimeout.c_str());
		metaDataNode->InsertEndChild(ptNode);
		profilesNode->InsertEndChild(metaDataNode);
	}

	if (Profile->PTZConfiguration)
	{
		auto obj = Profile->PTZConfiguration;
		auto ptzNode = _doc.NewElement("tt:PTZConfiguration");
		ptzNode->SetAttribute("token", obj->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(obj->Name.c_str());
		auto useCountNode = _doc.NewElement("tt:UseCount");
		useCountNode->SetText(obj->UseCount);
		auto tokenNode = _doc.NewElement("tt:NodeToken");
		tokenNode->SetText(obj->NodeToken.c_str());
		ptzNode->InsertEndChild(nNode);
		ptzNode->InsertEndChild(useCountNode);
		ptzNode->InsertEndChild(tokenNode);
		if (obj->DefaultAbsolutePantTiltPositionSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultAbsolutePantTiltPositionSpace");
			aNode->SetText(obj->DefaultAbsolutePantTiltPositionSpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultAbsoluteZoomPositionSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultAbsoluteZoomPositionSpace");
			aNode->SetText(obj->DefaultAbsoluteZoomPositionSpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultRelativePanTiltTranslationSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultRelativePanTiltTranslationSpace");
			aNode->SetText(obj->DefaultRelativePanTiltTranslationSpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultRelativeZoomTranslationSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultRelativeZoomTranslationSpace");
			aNode->SetText(obj->DefaultRelativeZoomTranslationSpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultContinuousPanTiltVelocitySpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultContinuousPanTiltVelocitySpace");
			aNode->SetText(obj->DefaultContinuousPanTiltVelocitySpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultContinuousZoomVelocitySpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultContinuousZoomVelocitySpace");
			aNode->SetText(obj->DefaultContinuousZoomVelocitySpace->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultPTZSpeed)
		{
			auto aNode = _doc.NewElement("tt:DefaultPTZSpeed");
			if (obj->DefaultPTZSpeed->PanTilt)
			{
				auto bNode = _doc.NewElement("tt:PanTilt");
				bNode->SetAttribute("x", obj->DefaultPTZSpeed->PanTilt->x);
				bNode->SetAttribute("y", obj->DefaultPTZSpeed->PanTilt->y);
				if (obj->DefaultPTZSpeed->PanTilt->space)
					bNode->SetAttribute("space", obj->DefaultPTZSpeed->PanTilt->space->c_str());

				aNode->InsertEndChild(bNode);
			}
			if (obj->DefaultPTZSpeed->Zoom)
			{
				auto bNode = _doc.NewElement("tt:Zoom");
				bNode->SetAttribute("x", obj->DefaultPTZSpeed->Zoom->x);
				if (obj->DefaultPTZSpeed->Zoom->space)
					bNode->SetAttribute("space", obj->DefaultPTZSpeed->Zoom->space->c_str());

				aNode->InsertEndChild(bNode);
			}
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->DefaultPTZTimeout)
		{
			auto aNode = _doc.NewElement("tt:DefaultPTZTimeout");
			aNode->SetText(obj->DefaultPTZTimeout->c_str());
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->PanTiltLimits)
		{
			auto aNode = _doc.NewElement("tt:PanTiltLimits");

			auto rangeNode = _doc.NewElement("tt:Range");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(obj->PanTiltLimits->Range.URI.c_str());

			auto xrangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			xMinNode->SetText(obj->PanTiltLimits->Range.xMin);
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMaxNode->SetText(obj->PanTiltLimits->Range.xMax);
			xrangeNode->InsertEndChild(xMinNode);
			xrangeNode->InsertEndChild(xMaxNode);

			auto yrangeNode = _doc.NewElement("tt:YRange");
			auto yMinNode = _doc.NewElement("tt:Min");
			yMinNode->SetText(obj->PanTiltLimits->Range.yMin);
			auto yMaxNode = _doc.NewElement("tt:Max");
			yMaxNode->SetText(obj->PanTiltLimits->Range.yMax);
			yrangeNode->InsertEndChild(yMinNode);
			yrangeNode->InsertEndChild(yMaxNode);

			rangeNode->InsertEndChild(uriNode);
			rangeNode->InsertEndChild(xrangeNode);
			rangeNode->InsertEndChild(yrangeNode);
			aNode->InsertEndChild(rangeNode);
			ptzNode->InsertEndChild(aNode);
		}
		if (obj->ZoomLimits)
		{
			auto aNode = _doc.NewElement("tt:ZoomLimits");
			auto rangeNode = _doc.NewElement("tt:Range");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(obj->ZoomLimits->Range.URI.c_str());

			auto xrangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			xMinNode->SetText(obj->ZoomLimits->Range.Min);
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMaxNode->SetText(obj->ZoomLimits->Range.Max);
			xrangeNode->InsertEndChild(xMinNode);
			xrangeNode->InsertEndChild(xMaxNode);

			rangeNode->InsertEndChild(uriNode);
			rangeNode->InsertEndChild(xrangeNode);
			aNode->InsertEndChild(rangeNode);
			ptzNode->InsertEndChild(aNode);
		}
		profilesNode->InsertEndChild(ptzNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetVideoSourceConfigurationResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetVideoSourceConfigurationResponse");
	auto cfgNode = _doc.NewElement("trt:Configuration");
	cfgNode->SetAttribute("token", Configuration.token.c_str());
	respNode->InsertEndChild(cfgNode);
	auto nNode = _doc.NewElement("tt:Name");
	nNode->SetText(Configuration.Name.c_str());
	auto useCountNode = _doc.NewElement("tt:UseCount");
	useCountNode->SetText(Configuration.UseCount);
	auto stNode = _doc.NewElement("tt:SourceToken");
	stNode->SetText(Configuration.SourceToken.c_str());
	auto boundsNode = _doc.NewElement("tt:Bounds");
	boundsNode->SetAttribute("x", Configuration.Bounds.x);
	boundsNode->SetAttribute("y", Configuration.Bounds.y);
	boundsNode->SetAttribute("width", Configuration.Bounds.width);
	boundsNode->SetAttribute("height", Configuration.Bounds.height);
	cfgNode->InsertEndChild(nNode);
	cfgNode->InsertEndChild(useCountNode);
	cfgNode->InsertEndChild(stNode);
	cfgNode->InsertEndChild(boundsNode);
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetVideoEncoderConfigurationOptionsResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetVideoEncoderConfigurationOptionsResponse");
	auto optionNode = _doc.NewElement("trt:Options");
	optionNode->SetAttribute("token", Options.token.c_str());
	respNode->InsertEndChild(optionNode);
	auto qrNode = _doc.NewElement("tt:QualityRange");
	auto qMinNode = _doc.NewElement("tt:Min");
	qMinNode->SetText(Options.QualityRange.Min);
	auto qMaxNode = _doc.NewElement("tt:Max");
	qMaxNode->SetText(Options.QualityRange.Max);
	qrNode->InsertEndChild(qMinNode);
	qrNode->InsertEndChild(qMaxNode);
	optionNode->InsertEndChild(qrNode);
	if (Options.H264)
	{
		auto h264Node = _doc.NewElement("tt:H264");
		for (size_t i = 0; i < Options.H264->ResolutionsAvailable.size(); i++)
		{
			auto raNode = _doc.NewElement("tt:ResolutionsAvailable");
			auto wNode = _doc.NewElement("tt:Width");
			wNode->SetText(Options.H264->ResolutionsAvailable[i]->Width);
			auto hNode = _doc.NewElement("tt:Height");
			hNode->SetText(Options.H264->ResolutionsAvailable[i]->Height);
			raNode->InsertEndChild(wNode);
			raNode->InsertEndChild(hNode);
			h264Node->InsertEndChild(raNode);
		}

		auto grNode = _doc.NewElement("tt:GovLengthRange");
		auto gMinNode = _doc.NewElement("tt:Min");
		gMinNode->SetText(Options.H264->GovLengthRange.Min);
		auto gMaxNode = _doc.NewElement("tt:Max");
		gMaxNode->SetText(Options.H264->GovLengthRange.Max);
		grNode->InsertEndChild(gMinNode);
		grNode->InsertEndChild(gMaxNode);
		h264Node->InsertEndChild(grNode);

		auto frNode = _doc.NewElement("tt:FrameRateRange");
		auto fMinNode = _doc.NewElement("tt:Min");
		fMinNode->SetText(Options.H264->FrameRateRange.Min);
		auto fMaxNode = _doc.NewElement("tt:Max");
		fMaxNode->SetText(Options.H264->FrameRateRange.Max);
		frNode->InsertEndChild(fMinNode);
		frNode->InsertEndChild(fMaxNode);
		h264Node->InsertEndChild(frNode);

		auto eiNode = _doc.NewElement("tt:EncodingIntervalRange");
		auto eMinNode = _doc.NewElement("tt:Min");
		eMinNode->SetText(Options.H264->EncodingIntervalRange.Min);
		auto eMaxNode = _doc.NewElement("tt:Max");
		eMaxNode->SetText(Options.H264->EncodingIntervalRange.Max);
		eiNode->InsertEndChild(eMinNode);
		eiNode->InsertEndChild(eMaxNode);
		h264Node->InsertEndChild(eiNode);

		for (size_t i = 0; i < Options.H264->H264ProfilesSupported.size(); i++)
		{
			auto hstions = _doc.NewElement("tt:H264ProfilesSupported");
			switch (Options.H264->H264ProfilesSupported[i])
			{
			case tt__H264Profile__Baseline:
				hstions->SetText("Baseline");
				break;
			case tt__H264Profile__Main:
				hstions->SetText("Main");
				break;
			case tt__H264Profile__High:
				hstions->SetText("High");
				break;
			default:
                break;
            }
			h264Node->InsertEndChild(hstions);
		}
		optionNode->InsertEndChild(h264Node);
	}
	
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _trt__GetServiceCapabilitiesResponse::Serialization()
{
	auto respNode = InitHeader("trt:GetServiceCapabilitiesResponse");
	auto capabilityNode = _doc.NewElement("trt:Capabilities");
	capabilityNode->SetAttribute("SnapshotUri", Capabilities.SnapshotUri);
	capabilityNode->SetAttribute("Rotation", Capabilities.Rotation);
	capabilityNode->SetAttribute("VideoSourceMode", Capabilities.VideoSourceMode);
	capabilityNode->SetAttribute("OSD", Capabilities.OSD);
	capabilityNode->SetAttribute("TemporaryOSDText", Capabilities.TemporaryOSDText);
	capabilityNode->SetAttribute("EXICompression", Capabilities.EXICompression);
	respNode->InsertEndChild(capabilityNode);
	auto pcNode = _doc.NewElement("trt:ProfileCapabilities");
	pcNode->SetAttribute("MaximumNumberOfProfiles", Capabilities.MaximumNumberOfProfiles);
	auto scNode = _doc.NewElement("trt:StreamingCapabilities");
	scNode->SetAttribute("RTPMulticast", Capabilities.StreamingCapabilities.RTPMulticast);
	scNode->SetAttribute("RTP_TCP", Capabilities.StreamingCapabilities.RTP_USCORETCP);
	scNode->SetAttribute("RTP_RTSP_TCP", Capabilities.StreamingCapabilities.RTP_USCORERTSP_USCORETCP);
	scNode->SetAttribute("NonAggregateControl", Capabilities.StreamingCapabilities.NonAggregateControl);
	scNode->SetAttribute("NoRTSPStreaming", Capabilities.StreamingCapabilities.NoRTSPStreaming);
	capabilityNode->InsertEndChild(pcNode);
	capabilityNode->InsertEndChild(scNode);
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _trt__AddPTZConfigurationResponse::Serialization()
{
//	auto respNode = InitHeader("trt:AddPTZConfigurationResponse");
	InitHeader("trt:AddPTZConfigurationResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
soapMedia::soapMedia()
{
}

soapMedia::~soapMedia()
{
}

int soapMedia::OnGetServiceCapabilities(void* req, std::string& body)
{
	_trt__GetServiceCapabilitiesResponse resp;
	resp.Capabilities.MaximumNumberOfProfiles = 2;
	resp.Capabilities.EXICompression = false;
	resp.Capabilities.SnapshotUri = true;
	resp.Capabilities.OSD = false;
	resp.Capabilities.Rotation = false;
	resp.Capabilities.TemporaryOSDText = false;
	resp.Capabilities.VideoSourceMode = false;
	resp.Capabilities.StreamingCapabilities.NonAggregateControl = false;
	resp.Capabilities.StreamingCapabilities.NoRTSPStreaming = false;
	resp.Capabilities.StreamingCapabilities.RTPMulticast = false;
	resp.Capabilities.StreamingCapabilities.RTP_USCORERTSP_USCORETCP = true;
	resp.Capabilities.StreamingCapabilities.RTP_USCORETCP = true;
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetProfiles(void* req, std::string& body)
{
    Emx::MediaClientVenc venc(0);
    Emx::MediaVenc::Param vencParam = {};
    venc.GetParam(vencParam);

	_trt__GetProfilesResponse resp;
	std::shared_ptr<tt__Profile> item1(new tt__Profile);
	item1->token = "Profile_01";
	item1->fixed = true;
	item1->Name = "MainStream";
	item1->VideoSourceConfiguration = std::make_shared<tt__VideoSourceConfiguration>();
	item1->VideoSourceConfiguration->token = VSCMToken;
	item1->VideoSourceConfiguration->SourceToken = VSCMToken;
	item1->VideoSourceConfiguration->UseCount = 2;
	item1->VideoSourceConfiguration->Name = "MainStream_VideoSourceConfiguration";
	item1->VideoSourceConfiguration->Bounds.x = 0;
	item1->VideoSourceConfiguration->Bounds.y = 0;
    item1->VideoSourceConfiguration->Bounds.width = (int)vencParam.width;
    item1->VideoSourceConfiguration->Bounds.height = (int)vencParam.height;
	
	item1->AudioSourceConfiguration = std::make_shared<tt__AudioSourceConfiguration>();
	item1->AudioSourceConfiguration->token = ASCToken;
	item1->AudioSourceConfiguration->SourceToken = ASCToken;
	item1->AudioSourceConfiguration->UseCount = 2;
	item1->AudioSourceConfiguration->Name = "MainStream_AudioSourceConfiguration";

	item1->VideoEncoderConfiguration = std::make_shared<tt__VideoEncoderConfiguration>();
	item1->VideoEncoderConfiguration->token = VECMToken;
	item1->VideoEncoderConfiguration->UseCount = 1;
	item1->VideoEncoderConfiguration->Name = "MainStream_VideoEncoderConfiguration";
    item1->VideoEncoderConfiguration->Encoding = (vencParam.codec == Emx::VideoCodecE::H264) ? "H264":"H265";
    item1->VideoEncoderConfiguration->Width = (int)vencParam.width;
    item1->VideoEncoderConfiguration->Height = (int)vencParam.height;
	item1->VideoEncoderConfiguration->Quality = 4.0f;
	item1->VideoEncoderConfiguration->RateControl = std::make_shared<tt__VideoRateControl>();
    item1->VideoEncoderConfiguration->RateControl->EncodingInterval = (int)vencParam.iFrameInv;
    item1->VideoEncoderConfiguration->RateControl->FrameRateLimit = (int)vencParam.fps;
    item1->VideoEncoderConfiguration->RateControl->BitrateLimit = (int)vencParam.bitRate;
	item1->VideoEncoderConfiguration->H264 = std::make_shared<tt__H264Configuration>();
    item1->VideoEncoderConfiguration->H264->GovLength = (int)(vencParam.fps*vencParam.iFrameInv);
	item1->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
	item1->VideoEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item1->VideoEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item1->VideoEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item1->VideoEncoderConfiguration->Multicast.AutoStart = false;
	item1->VideoEncoderConfiguration->Multicast.Port = 40000;
	item1->VideoEncoderConfiguration->Multicast.TTL = 64;
	item1->VideoEncoderConfiguration->SessionTimeout = "PT60S";

    Emx::MediaClientAenc aenc(0);
    Emx::MediaAenc::Param AencParam = {};
    aenc.GetParam(AencParam);

	item1->AudioEncoderConfiguration = std::make_shared<tt__AudioEncoderConfiguration>();
    if (AencParam.codec == Emx::AudioCodecE::AAC){
        item1->AudioEncoderConfiguration->Encoding = "AAC";
    }else if (AencParam.codec == Emx::AudioCodecE::G711A){
        item1->AudioEncoderConfiguration->Encoding = "G711";
    }else if (AencParam.codec == Emx::AudioCodecE::G711U){
        item1->AudioEncoderConfiguration->Encoding = "G711";
    }
	item1->AudioEncoderConfiguration->token = AECToken;
	item1->AudioEncoderConfiguration->Name = "MainStream_AudioEncoderConfiguration";
	item1->AudioEncoderConfiguration->UseCount = 1;
    item1->AudioEncoderConfiguration->Bitrate = (int)AencParam.bitRate;
    item1->AudioEncoderConfiguration->SampleRate = (int)AencParam.sampleRate;
	item1->AudioEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item1->AudioEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item1->AudioEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item1->AudioEncoderConfiguration->Multicast.Port = 40002;
	item1->AudioEncoderConfiguration->Multicast.TTL = 64;
	item1->AudioEncoderConfiguration->Multicast.AutoStart = false;
	item1->AudioEncoderConfiguration->SessionTimeout = "PT60S";

	//item1->VideoAnalyticsConfiguration = std::make_shared<tt__VideoAnalyticsConfiguration>();
	//item1->VideoAnalyticsConfiguration->Name = "AnalyticsConfigure";
	//item1->VideoAnalyticsConfiguration->token = "00000";
	//item1->VideoAnalyticsConfiguration->UseCount = 2;
	//std::shared_ptr<tt__Config> cfg1(new tt__Config);
	//cfg1->Name = "MyCellMotion";
	//cfg1->Type = "tt:CellMotionEngine";
	//_tt__ItemList_SimpleItem simpleItem;
	//simpleItem.Name = "Sensitivity";
	//simpleItem.Value = "60"; //Range is 0 to 100
	//cfg1->Parameters.SimpleItem.emplace_back(simpleItem);
	//_tt__ItemList_ElementItem elementItem;
	//elementItem.Name = "Layout";
	//elementItem.Layout.Columns = "22";
	//elementItem.Layout.Rows = "18";
	//elementItem.Layout.Transformation.Scale.x = 0.00625f;
	//elementItem.Layout.Transformation.Scale.y = 0.00834f;
	//elementItem.Layout.Transformation.Translate.x = -1.f;
	//elementItem.Layout.Transformation.Translate.y = -1.f;
	//cfg1->Parameters.ElementItem.emplace_back(elementItem);
	//item1->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration.AnalyticsModule.emplace_back(std::move(cfg1));

	//std::shared_ptr<tt__Config> cfg2(new tt__Config);
	//cfg2->Name = "MyMotionDetectorRule";
	//cfg2->Type = "tt:CellMotionDetector";
	//_tt__ItemList_SimpleItem simpleItem1;
	//simpleItem1.Name = "MinCount";
	//simpleItem1.Value = "5";
	//_tt__ItemList_SimpleItem simpleItem2;
	//simpleItem2.Name = "AlarmOnDelay";
	//simpleItem2.Value = "1000";
	//_tt__ItemList_SimpleItem simpleItem3;
	//simpleItem3.Name = "AlarmOffDelay";
	//simpleItem3.Value = "1000";//milliseconds
	//_tt__ItemList_SimpleItem simpleItem4;
	//simpleItem4.Name = "ActiveCells";
	//simpleItem4.Value = "zwA=";//Packbit+base64
	//cfg2->Parameters.SimpleItem.emplace_back(simpleItem1);
	//cfg2->Parameters.SimpleItem.emplace_back(simpleItem2);
	//cfg2->Parameters.SimpleItem.emplace_back(simpleItem3);
	//cfg2->Parameters.SimpleItem.emplace_back(simpleItem4);
	//item1->VideoAnalyticsConfiguration->RuleEngineConfiguration.Rule.emplace_back(std::move(cfg2));

	/*item1->MetadataConfiguration = std::make_shared<tt__MetadataConfiguration>();
	item1->MetadataConfiguration->token = "00000";
	item1->MetadataConfiguration->UseCount = 2;
	item1->MetadataConfiguration->Name = "MainStream_MetadataConfiguration";
	item1->MetadataConfiguration->Analytics = false; 
	item1->MetadataConfiguration->PTZStatus = std::make_shared<tt__PTZFilter>();
	item1->MetadataConfiguration->PTZStatus->Status = false;
	item1->MetadataConfiguration->PTZStatus->Position = false;
	item1->MetadataConfiguration->Events = std::make_shared<tt__EventSubscription>();
	item1->MetadataConfiguration->Events->Filter.TopicExpression.Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
	item1->MetadataConfiguration->Events->Filter.TopicExpression.v = "tns1:RuleEngine/CellMotionDetector/Motion";
	item1->MetadataConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item1->MetadataConfiguration->Multicast.Address->Type = "IPv4";
	item1->MetadataConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.2.0.0");
	item1->MetadataConfiguration->Multicast.Port = 40020;
	item1->MetadataConfiguration->Multicast.TTL = 64;
	item1->MetadataConfiguration->Multicast.AutoStart = false;
	item1->MetadataConfiguration->SessionTimeout = "PT60S";*/

	item1->PTZConfiguration.reset(new tt__PTZConfiguration);
	item1->PTZConfiguration->token = "PTZToken";
	item1->PTZConfiguration->Name = "PTZ";
	item1->PTZConfiguration->UseCount = 2;
	item1->PTZConfiguration->NodeToken = "PTZNODETOKEN";
	item1->PTZConfiguration->DefaultAbsolutePantTiltPositionSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace"));
	item1->PTZConfiguration->DefaultAbsoluteZoomPositionSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace"));
	item1->PTZConfiguration->DefaultRelativePanTiltTranslationSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace"));
	item1->PTZConfiguration->DefaultRelativeZoomTranslationSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace"));
	item1->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace"));
	item1->PTZConfiguration->DefaultContinuousZoomVelocitySpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace"));
	item1->PTZConfiguration->DefaultPTZSpeed.reset(new tt__PTZSpeed);
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt.reset(new tt__Vector2D);
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 0.1f;
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 0.1f;
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->space.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace"));
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom.reset(new tt__Vector1D);
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom->x = 1.0f;
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom->space.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace"));
	item1->PTZConfiguration->DefaultPTZTimeout.reset(new std::string("PT300S"));
	item1->PTZConfiguration->PanTiltLimits.reset(new tt__PanTiltLimits);
	item1->PTZConfiguration->PanTiltLimits->Range.URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	item1->PTZConfiguration->PanTiltLimits->Range.xMin = -1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.xMax = 1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.yMax = 1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.yMin = -1.0f;
	item1->PTZConfiguration->ZoomLimits.reset(new tt__ZoomLimits);
	item1->PTZConfiguration->ZoomLimits->Range.Max = 1.0f;
	item1->PTZConfiguration->ZoomLimits->Range.Min = 0.0f;
	item1->PTZConfiguration->ZoomLimits->Range.URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
    resp.Profiles.emplace_back(std::move(item1));

    int chn = 0;
    venc.GetChnNum(chn);
    if (chn != 1)
    {
        Emx::MediaClientVenc venc1(1);
        venc1.GetParam(vencParam);

	std::shared_ptr<tt__Profile> item2(new tt__Profile);
	item2->token = "Profile_02";
	item2->fixed = true;
	item2->Name = "SubStream";
	item2->VideoSourceConfiguration = std::make_shared<tt__VideoSourceConfiguration>();
    item2->VideoSourceConfiguration->token = VSCSToken;
    item2->VideoSourceConfiguration->SourceToken = VSCSToken;
	item2->VideoSourceConfiguration->UseCount = 2;
	item2->VideoSourceConfiguration->Name = "SubStream_VideoSourceConfiguration";
	item2->VideoSourceConfiguration->Bounds.x = 0;
	item2->VideoSourceConfiguration->Bounds.y = 0;
    item2->VideoSourceConfiguration->Bounds.width = vencParam.width;
    item2->VideoSourceConfiguration->Bounds.height = vencParam.height;

	item2->AudioSourceConfiguration = std::make_shared<tt__AudioSourceConfiguration>();
	item2->AudioSourceConfiguration->token = ASCToken;
	item2->AudioSourceConfiguration->SourceToken = ASCToken;
	item2->AudioSourceConfiguration->UseCount = 2;
	item2->AudioSourceConfiguration->Name = "SubStream_AudioSourceConfiguration";

	item2->VideoEncoderConfiguration = std::make_shared<tt__VideoEncoderConfiguration>();
	item2->VideoEncoderConfiguration->token = VECSToken;
	item2->VideoEncoderConfiguration->UseCount = 1;
	item2->VideoEncoderConfiguration->Name = "SubStream_VideoEncoderConfiguration";
    item2->VideoEncoderConfiguration->Encoding = (vencParam.codec == Emx::VideoCodecE::H264) ? "H264":"H265";
    item2->VideoEncoderConfiguration->Width = (int)vencParam.width;
    item2->VideoEncoderConfiguration->Height = (int)vencParam.height;
	item2->VideoEncoderConfiguration->Quality = 4.0f;
	item2->VideoEncoderConfiguration->RateControl = std::make_shared<tt__VideoRateControl>();
    item2->VideoEncoderConfiguration->RateControl->EncodingInterval = (int)vencParam.iFrameInv;
    item2->VideoEncoderConfiguration->RateControl->FrameRateLimit = (int)vencParam.fps;
    item2->VideoEncoderConfiguration->RateControl->BitrateLimit = (int)vencParam.bitRate;
	item2->VideoEncoderConfiguration->H264 = std::make_shared<tt__H264Configuration>();
    item2->VideoEncoderConfiguration->H264->GovLength = (int)vencParam.iFrameInv*(int)vencParam.fps;
	item2->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
	item2->VideoEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item2->VideoEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item2->VideoEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item2->VideoEncoderConfiguration->Multicast.AutoStart = false;
	item2->VideoEncoderConfiguration->Multicast.Port = 40004;
	item2->VideoEncoderConfiguration->Multicast.TTL = 64;
	item2->VideoEncoderConfiguration->SessionTimeout = "PT60S";

	item2->AudioEncoderConfiguration = std::make_shared<tt__AudioEncoderConfiguration>();
    if (AencParam.codec == Emx::AudioCodecE::AAC){
        item2->AudioEncoderConfiguration->Encoding = "AAC";
    }else if (AencParam.codec == Emx::AudioCodecE::G711A){
        item2->AudioEncoderConfiguration->Encoding = "G711";
    }else if (AencParam.codec == Emx::AudioCodecE::G711U){
        item2->AudioEncoderConfiguration->Encoding = "G711";
    }
	item2->AudioEncoderConfiguration->token = AECToken;
	item2->AudioEncoderConfiguration->Name = "SubStream_AudioEncoderConfiguration";
	item2->AudioEncoderConfiguration->UseCount = 1;
    item2->AudioEncoderConfiguration->Bitrate = (int)AencParam.bitRate;;
    item2->AudioEncoderConfiguration->SampleRate = (int)AencParam.sampleRate;;
	item2->AudioEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item2->AudioEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item2->AudioEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item2->AudioEncoderConfiguration->Multicast.Port = 40006;
	item2->AudioEncoderConfiguration->Multicast.TTL = 64;
	item2->AudioEncoderConfiguration->Multicast.AutoStart = false;
	item2->AudioEncoderConfiguration->SessionTimeout = "PT60S";
    item2->PTZConfiguration = item1->PTZConfiguration;
    resp.Profiles.emplace_back(std::move(item2));
    }


	/*item2->VideoAnalyticsConfiguration = item1->VideoAnalyticsConfiguration;
	item2->MetadataConfiguration = std::make_shared<tt__MetadataConfiguration>();
	item2->MetadataConfiguration->token = "00000";
	item2->MetadataConfiguration->UseCount = 2;
	item2->MetadataConfiguration->Name = "SubStream_MetadataConfiguration";
	item2->MetadataConfiguration->Analytics = false;
	item2->MetadataConfiguration->PTZStatus = std::make_shared<tt__PTZFilter>();
	item2->MetadataConfiguration->PTZStatus->Status = false;
	item2->MetadataConfiguration->PTZStatus->Position = false;
	item2->MetadataConfiguration->Events = std::make_shared<tt__EventSubscription>();
	item2->MetadataConfiguration->Events->Filter.TopicExpression.Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
	item2->MetadataConfiguration->Events->Filter.TopicExpression.v = "tns1:RuleEngine/CellMotionDetector/Motion";
	item2->MetadataConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item2->MetadataConfiguration->Multicast.Address->Type = "IPv4";
	item2->MetadataConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.2.0.0");
	item2->MetadataConfiguration->Multicast.Port = 40020;
	item2->MetadataConfiguration->Multicast.TTL = 64;
	item2->MetadataConfiguration->Multicast.AutoStart = false;
	item2->MetadataConfiguration->SessionTimeout = "PT60S";*/



    //
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetStreamUri(void* req, std::string& body)
{
	std::string profileToken = "Profile_01";
	std::string streamType;
	std::string protocol;
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto dataNode = child->FirstChildElement();
		for (auto i = dataNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "StreamSetup"))
			{
				for (auto j = i->FirstChildElement(); j; j=j->NextSiblingElement())
				{
					if (strstr(j->Value(), "Stream"))
						streamType = j->GetText();
					else if (strstr(j->Value(), "Transport"))
					{
						if (strstr(j->FirstChildElement()->Value(), "Protocol"))
							protocol = j->FirstChildElement()->GetText();
					}
				}
			}
			else if (strstr(i->Value(), "ProfileToken")){
                profileToken = i->GetText();
			}

		}
	}

	_trt__GetStreamUriResponse resp;
	resp.MediaUri.InvalidAfterConnect = false;
	resp.MediaUri.InvalidAfterReboot = false;
	resp.MediaUri.Timeout = "PT0S";
	if(profileToken == "Profile_02")
	    resp.MediaUri.Uri = EPSoap::Instance().RtspSubStreamUri;
	else
        resp.MediaUri.Uri = EPSoap::Instance().RtspMainStreamUri;

	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetOSD(void* req, std::string& body)
{
	return 200;
}

int soapMedia::OnSetOSD(void* req, std::string& body)
{
	return 200;
}

int soapMedia::OnGetOSDOptions(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string cfgToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getCfgNode = child->FirstChildElement();
		if (getCfgNode && strstr(getCfgNode->Value(), "GetOSDOptions"))
		{
			for (auto i = getCfgNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "ConfigurationToken"))
					cfgToken = i->GetText();
			}
		}
	}

	body.clear();
	std::ostringstream os;
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	os << "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">";
	os << "<s:Header/><s:Body><trt:GetOSDOptionsResponse><trt:OSDOptions>";
	os << "<tt:MaximumNumberOfOSDs Total=\"5\" Image=\"1\" PlainText=\"1\" Date=\"1\" Time=\"1\" DateAndTime=\"1\">6</tt:MaximumNumberOfOSDs>";
	os << "<tt:Type>Text</tt:Type>";
	os << "<tt:PositionOption>LowerRight</tt:PositionOption>";

	os << "</trt:OSDOptions></trt:GetOSDOptionsResponse></s:Body></s:Envelope>";
	body = os.str();
	return 200;
}

int soapMedia::OnGetVideoSources(void* req, std::string& body)
{
	_trt__GetVideoSourcesResponse resp;
    int chn;
    Emx::MediaClientVenc::GetChnNum(chn);
    for (int i = 0; i < chn; ++i) {
        std::shared_ptr<tt__VideoSource> vs(new tt__VideoSource);
        Emx::MediaClientVenc venc(i);
        Emx::MediaVenc::Param vencParam = {};
        venc.GetParam(vencParam);

        if (0 == i)
            vs->token = VSCMToken;
        else
            vs->token = VSCSToken;
        vs->Framerate = (float)vencParam.fps;
        vs->Heigth = (int)vencParam.height;;
        vs->Width = (int)vencParam.width;;
        resp.VideoSources.emplace_back(std::move(vs));
    }
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetAudioSources(void* req, std::string& body)
{
	_trt__GetAudioSourcesResponse resp;
	std::shared_ptr<tt__AudioSource> av(new tt__AudioSource);
	av->token = ASCToken;
	av->Channels = 1;
	resp.AudioSources.emplace_back(std::move(av));
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetProfile(void* req, std::string& body)
{
	std::string profileToken = "Profile_01";
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto tokenNode = child->FirstChildElement()->FirstChildElement();
		if (tokenNode && strstr(tokenNode->Value(), "ProfileToken"))
		{
			profileToken = tokenNode->GetText();
			std::cout << "req profile token " << profileToken << std::endl;
		}
	}

	_trt__GetProfileResponse resp;
	std::shared_ptr<tt__Profile> item1(new tt__Profile);

    int chn = 0;
    if (profileToken == "Profile_01"){
        chn = 0;
        item1->Name = "MainStream";
    }
    else if (profileToken == "Profile_02"){
        chn = 1;
        item1->Name = "SubStream";
    }
    Emx::MediaClientVenc venc(chn);
    Emx::MediaVenc::Param vencParam = {};
    venc.GetParam(vencParam);

	item1->token = profileToken;
	item1->fixed = true;
	item1->VideoSourceConfiguration = std::make_shared<tt__VideoSourceConfiguration>();
    if (chn){
        item1->VideoSourceConfiguration->token = VSCSToken;
        item1->VideoSourceConfiguration->SourceToken = VSCSToken;
    }
    else{
        item1->VideoSourceConfiguration->token = VSCMToken;
        item1->VideoSourceConfiguration->SourceToken = VSCMToken;
    }
	item1->VideoSourceConfiguration->UseCount = 2;
	item1->VideoSourceConfiguration->Name = "MainStream_VideoSourceConfiguration";
	item1->VideoSourceConfiguration->Bounds.x = 0;
	item1->VideoSourceConfiguration->Bounds.y = 0;
    item1->VideoSourceConfiguration->Bounds.width = (int)vencParam.width;
    item1->VideoSourceConfiguration->Bounds.height = (int)vencParam.height;


	item1->AudioSourceConfiguration = std::make_shared<tt__AudioSourceConfiguration>();
    item1->AudioSourceConfiguration->token = ASCToken;
    item1->AudioSourceConfiguration->SourceToken = ASCToken;
	item1->AudioSourceConfiguration->UseCount = 2;
	item1->AudioSourceConfiguration->Name = "MainStream_AudioSourceConfiguration";

	item1->VideoEncoderConfiguration = std::make_shared<tt__VideoEncoderConfiguration>();
    if(chn)
        item1->VideoEncoderConfiguration->token = VECSToken;
    else
        item1->VideoEncoderConfiguration->token = VECMToken;
	item1->VideoEncoderConfiguration->UseCount = 1;
	item1->VideoEncoderConfiguration->Name = "MainStream_VideoEncoderConfiguration";
    item1->VideoEncoderConfiguration->Encoding = (vencParam.codec == Emx::VideoCodecE::H264) ? "H264":"H265";
    item1->VideoEncoderConfiguration->Width = (int)vencParam.width;
    item1->VideoEncoderConfiguration->Height = (int)vencParam.height;
	item1->VideoEncoderConfiguration->Quality = 4.0f;
	item1->VideoEncoderConfiguration->RateControl = std::make_shared<tt__VideoRateControl>();
    item1->VideoEncoderConfiguration->RateControl->EncodingInterval = (int)vencParam.iFrameInv;
    item1->VideoEncoderConfiguration->RateControl->FrameRateLimit = (int)vencParam.fps;
    item1->VideoEncoderConfiguration->RateControl->BitrateLimit = (int)vencParam.bitRate;
	item1->VideoEncoderConfiguration->H264 = std::make_shared<tt__H264Configuration>();
	item1->VideoEncoderConfiguration->H264->GovLength = 30;
	item1->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
    item1->VideoEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();/*Multicast 多播配置，暂没有用到*/
	item1->VideoEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item1->VideoEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item1->VideoEncoderConfiguration->Multicast.AutoStart = false;
	item1->VideoEncoderConfiguration->Multicast.Port = 40000;
	item1->VideoEncoderConfiguration->Multicast.TTL = 64;
	item1->VideoEncoderConfiguration->SessionTimeout = "PT60S";

    Emx::MediaClientAenc aenc(0);
    Emx::MediaAenc::Param AencParam = {};
    aenc.GetParam(AencParam);


	item1->AudioEncoderConfiguration = std::make_shared<tt__AudioEncoderConfiguration>();

    if (AencParam.codec == Emx::AudioCodecE::AAC){
        item1->AudioEncoderConfiguration->Encoding = "AAC";
    }else if (AencParam.codec == Emx::AudioCodecE::G711A){
        item1->AudioEncoderConfiguration->Encoding = "G711";
    }else if (AencParam.codec == Emx::AudioCodecE::G711U){
        item1->AudioEncoderConfiguration->Encoding = "G711";
    }
    item1->AudioEncoderConfiguration->token = AECToken;
	item1->AudioEncoderConfiguration->Name = "MainStream_AudioEncoderConfiguration";
	item1->AudioEncoderConfiguration->UseCount = 1;
    item1->AudioEncoderConfiguration->Bitrate = (int)AencParam.bitRate;
    item1->AudioEncoderConfiguration->SampleRate = (int)AencParam.sampleRate;
	item1->AudioEncoderConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item1->AudioEncoderConfiguration->Multicast.Address->Type = "IPv4";
	item1->AudioEncoderConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
	item1->AudioEncoderConfiguration->Multicast.Port = 40002;
	item1->AudioEncoderConfiguration->Multicast.TTL = 64;
	item1->AudioEncoderConfiguration->Multicast.AutoStart = false;
	item1->AudioEncoderConfiguration->SessionTimeout = "PT60S";

	/*item1->MetadataConfiguration = std::make_shared<tt__MetadataConfiguration>();
	item1->MetadataConfiguration->token = "00000";
	item1->MetadataConfiguration->UseCount = 2;
	item1->MetadataConfiguration->Name = "MainStream_MetadataConfiguration";
	item1->MetadataConfiguration->Analytics = false;
	item1->MetadataConfiguration->PTZStatus = std::make_shared<tt__PTZFilter>();
	item1->MetadataConfiguration->PTZStatus->Status = true;
	item1->MetadataConfiguration->PTZStatus->Position = true;
	item1->MetadataConfiguration->Multicast.Address = std::make_shared<tt__IPAddress>();
	item1->MetadataConfiguration->Multicast.Address->Type = "IPv4";
	item1->MetadataConfiguration->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.2.0.0");
	item1->MetadataConfiguration->Multicast.Port = 40020;
	item1->MetadataConfiguration->Multicast.TTL = 64;
	item1->MetadataConfiguration->Multicast.AutoStart = false;
	item1->MetadataConfiguration->SessionTimeout = "PT60S";*/

	item1->PTZConfiguration.reset(new tt__PTZConfiguration);
	item1->PTZConfiguration->token = "PTZToken";
	item1->PTZConfiguration->Name = "PTZ";
	item1->PTZConfiguration->UseCount = 2;
	item1->PTZConfiguration->NodeToken = "PTZNODETOKEN";
	item1->PTZConfiguration->DefaultAbsolutePantTiltPositionSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace"));
	item1->PTZConfiguration->DefaultAbsoluteZoomPositionSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace"));
	item1->PTZConfiguration->DefaultRelativePanTiltTranslationSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace"));
	item1->PTZConfiguration->DefaultRelativeZoomTranslationSpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace"));
	item1->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace"));
	item1->PTZConfiguration->DefaultContinuousZoomVelocitySpace.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace"));
	item1->PTZConfiguration->DefaultPTZSpeed.reset(new tt__PTZSpeed);
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt.reset(new tt__Vector2D);
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 0.1f;
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 0.1f;
	item1->PTZConfiguration->DefaultPTZSpeed->PanTilt->space.reset(new std::string("http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace"));
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom.reset(new tt__Vector1D);
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom->x = 1.0f;
	item1->PTZConfiguration->DefaultPTZSpeed->Zoom->space.reset(new std::string("http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace"));
	item1->PTZConfiguration->DefaultPTZTimeout.reset(new std::string("PT300S"));
	item1->PTZConfiguration->PanTiltLimits.reset(new tt__PanTiltLimits);
	item1->PTZConfiguration->PanTiltLimits->Range.URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	item1->PTZConfiguration->PanTiltLimits->Range.xMin = -1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.xMax = 1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.yMax = 1.0f;
	item1->PTZConfiguration->PanTiltLimits->Range.yMin = -1.0f;
	item1->PTZConfiguration->ZoomLimits.reset(new tt__ZoomLimits);
	item1->PTZConfiguration->ZoomLimits->Range.Max = 1.0f;
	item1->PTZConfiguration->ZoomLimits->Range.Min = 0.0f;
	item1->PTZConfiguration->ZoomLimits->Range.URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";

	resp.Profile = item1;

	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetSnapshotUri(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:GetSnapshotUriResponse><trt:MediaUri><tt:Uri>");
	body.append(EPSoap::Instance().snapshotUri);
	body.append("</tt:Uri><tt:InvalidAfterConnect>false</tt:InvalidAfterConnect><tt:InvalidAfterReboot>"
		"false</tt:InvalidAfterReboot><tt:Timeout>PT60S</tt:Timeout></trt:MediaUri></trt:GetSnapshotUriResponse></s:Body></s:Envelope>");

	return 200;
}

int soapMedia::OnGetVideoSourceConfiguration(void* req, std::string& body)
{
	_trt__GetVideoSourceConfigurationResponse resp;
	resp.Configuration.token = VSCMToken;
	resp.Configuration.SourceToken = VSCMToken;
	resp.Configuration.UseCount = 2;
	resp.Configuration.Name = "MainStream_VideoSourceConfiguration";
	resp.Configuration.Bounds.x = 0;
	resp.Configuration.Bounds.y = 0;
	resp.Configuration.Bounds.width = 2048;
	resp.Configuration.Bounds.height = 1536;
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnGetVideoEncoderConfigurationOptions(void* req, std::string& body)
{
    int num;
    auto e = Emx::MediaClientVenc::GetChnNum(num);
    if (e != Emx::ErrCodeE::Success) {
        return -1;
    }
    Json::Value range;
    for (int i = 0; i < num; i++) {
        Emx::MediaClientVenc venc(i);
        e = venc.GetParamRange(range);
        if (e != Emx::ErrCodeE::Success) {
            return -1;
        }
    }

    _trt__GetVideoEncoderConfigurationOptionsResponse resp;
    resp.Options.QualityRange.Min = 1;
    resp.Options.QualityRange.Max = 6;
    resp.Options.H264 = std::make_shared<tt__H264Options>();
    resp.Options.H264->EncodingIntervalRange.Max = 1;
    resp.Options.H264->EncodingIntervalRange.Min = 1;
    resp.Options.H264->FrameRateRange.Max = range["fps"]["max"].asInt();
    resp.Options.H264->FrameRateRange.Min = range["fps"]["min"].asInt();
    resp.Options.H264->GovLengthRange.Min = 1;
    resp.Options.H264->GovLengthRange.Max = 90;
    resp.Options.H264->H264ProfilesSupported.push_back(tt__H264Profile__High);
    resp.Options.H264->H264ProfilesSupported.push_back(tt__H264Profile__Main);
    resp.Options.H264->H264ProfilesSupported.push_back(tt__H264Profile__Baseline);

    for (auto &resolution : range["resolution"]) {
        std::shared_ptr<tt__VideoResolution> r1(new tt__VideoResolution);
        r1->Width = resolution["w"].asInt();
        r1->Height = resolution["h"].asInt();
        resp.Options.H264->ResolutionsAvailable.emplace_back(std::move(r1));
        resp.Options.token = VECMToken;
    }
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnAddPTZConfiguration(void* req, std::string& body)
{
	_trt__AddPTZConfigurationResponse resp;
	body = std::move(resp.Serialization());
	return 200;
}

int soapMedia::OnSetVideoEncoderConfiguration(void* req, std::string& body)
{
	tt__VideoEncoderConfiguration a;

	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto setNode = child->FirstChildElement()->FirstChildElement();
		if (setNode && strstr(setNode->Value(), "Configuration"))
		{
			for (auto i = setNode->FirstChildElement(); i; i=i->NextSiblingElement())
			{
				if (strstr(i->Value(), "Encoding"))
					a.Encoding = i->GetText();
				else if (strstr(i->Value(), "Resolution"))
				{
					for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
					{
						if (strstr(j->Value(), "Width"))
							a.Width = j->IntText();
						else if (strstr(j->Value(), "Height"))
							a.Height = j->IntText();
					}
				}
				else if (strstr(i->Value(), "Quality"))
					a.Quality = i->FloatText();
				else if (strstr(i->Value(), "RateControl"))
				{
					a.RateControl = std::make_shared<tt__VideoRateControl>();
					for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
					{
						if (strstr(j->Value(), "FrameRateLimit"))
							a.RateControl->FrameRateLimit = j->IntText();
						else if (strstr(j->Value(), "EncodingInterval"))
							a.RateControl->EncodingInterval = j->IntText();
						else if (strstr(j->Value(), "BitrateLimit"))
							a.RateControl->BitrateLimit = j->IntText();
					}
				}
				else if (strstr(i->Value(), "H264"))
				{
					a.H264 = std::make_shared<tt__H264Configuration>();
					for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
					{
						if (strstr(j->Value(), "GovLength"))
							a.H264->GovLength = j->IntText();
						else if (strstr(j->Value(), "H264Profile"))
							a.H264->H264Profile = (tt__H264Profile)j->IntText();
					}
				}
				else if (strstr(i->Value(), "Multicast"))
				{
					/*TODO*/
				}
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:SetVideoEncoderConfigurationResponse/></s:Body></s:Envelope>");
	return 200;
}

int soapMedia::OnGetVideoEncoderConfiguration(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string cfgToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getCfgNode = child->FirstChildElement();
		if (getCfgNode && strstr(getCfgNode->Value(), "GetVideoEncoderConfiguration"))
		{
			for (auto i = getCfgNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "ConfigurationToken"))
					cfgToken = i->GetText();
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:GetVideoEncoderConfigurationResponse><trt:Configuration GuaranteedFrameRate=\"false\" token = \"");
	body.append(cfgToken);
	body.append("\">");
	if (cfgToken ==VECMToken)
		body.append("<tt:Name>MainStream_VideoEncoderConfiguration</tt:Name>");
	else if (cfgToken == VECMToken)
		body.append("<tt:Name>SubStream_VideoEncoderConfiguration</tt:Name>");
	body.append("<tt:UseCount>0</tt:UseCount>");
	body.append("<tt:Encoding>H264</tt:Encoding>");
	body.append("<tt:Resolution>");
	body.append("<tt:Width>1920</tt:Width><tt:Height>1080</tt:Height></tt:Resolution>");
	body.append("<tt:Quality>3.0</tt:Quality>");
	body.append("<tt:RateControl>");
	body.append("<tt:FrameRateLimit>15</tt:FrameRateLimit><tt:EncodingInterval>30</tt:EncodingInterval><tt:BitrateLimit>1536</tt:BitrateLimit>");
	body.append("</tt:RateControl>");
	body.append("<tt:H264><tt:GovLength>30</tt:GovLength><tt:H264Profile>Baseline</tt:H264Profile></tt:H264>");
	body.append("<tt:SessionTimeout>PT0S</tt:SessionTimeout>");
	body.append("</trt:Configuration></trt:GetVideoEncoderConfigurationResponse></s:Body></s:Envelope>");

	return 200;
}

int soapMedia::OnGetAudioEncoderConfigurationOptions(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string cfgToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getCfgNode = child->FirstChildElement();
		if (getCfgNode && strstr(getCfgNode->Value(), "GetAudioEncoderConfigurationOptions"))
		{
			for (auto i = getCfgNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "ConfigurationToken"))
					cfgToken = i->GetText();
			}
		}
	}

	body.clear();
	std::ostringstream os;
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	os << "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">";
	os << "<s:Header/><s:Body><trt:GetAudioEncoderConfigurationOptionsResponse><trt:Options>";
	os << "<tt:Options>";
	os << "<tt:Encoding>G711</tt:Encoding>";
	os << "<tt:BitrateList>";
	os << "<tt:Items>16</tt:Items></tt:BitrateList>";
	os << "<tt:SampleRateList>";
	os << "<tt:Items>8</tt:Items></tt:SampleRateList></tt:Options>";
	os << "</trt:Options></trt:GetAudioEncoderConfigurationOptionsResponse></s:Body></s:Envelope>";
	body = os.str();
	return 200;
}

int soapMedia::OnGetAudioEncoderConfiguration(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string cfgToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getCfgNode = child->FirstChildElement();
		if (getCfgNode && strstr(getCfgNode->Value(), "GetAudioEncoderConfiguration"))
		{
			for (auto i = getCfgNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "ConfigurationToken"))
					cfgToken = i->GetText();
			}
		}
	}

	body.clear();
	std::ostringstream os;
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	os << "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">";
	os << "<s:Header/><s:Body><trt:GetAudioEncoderConfigurationResponse><trt:Configuration token=\"";
	os << cfgToken << "\">";
	if (cfgToken == "0000")
		os << "<tt:Name>MainStream_AudioEncoderConfiguration</tt:Name>";
	else if (cfgToken == "0001")
		os << "<tt:Name>SubStream_AudioEncoderConfiguration</tt:Name>";
	os << "<tt:UseCount>0</tt:UseCount>";
	os << "<tt:Encoding>G711</tt:Encoding>";
	os << "<tt:Bitrate>16</tt:Bitrate>";
	os << "<tt:SampleRate>8</tt:SampleRate>";
	os << "</trt:Configuration></trt:GetAudioEncoderConfigurationResponse></s:Body></s:Envelope>";
	body = os.str();
	return 200;
}

int soapMedia::OnGetSnapshot(void* req, std::string& body)
{
	FILE* pf;
#ifdef _WIN32
	fopen_s(&pf, "s.jpg", "rb");
#else
	pf = fopen("s.jpg", "rb");
#endif 
	fseek(pf, 0, SEEK_END);
	auto len = ftell(pf);
	rewind(pf);
	char* p = new char[len];
	fread(p, 1, len, pf);
	fclose(pf);

	body.assign(p, len);
	delete[] p;
	return 200;
}

int soapMedia::OnGetMetadataConfigurationOptions(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:GetMetadataConfigurationOptionsResponse><trt:Options><tt:PTZStatusFilterOptions><tt:PanTiltStatusSupported>false</tt:PanTiltStatusSupported>");
	body.append("<tt:ZoomStatusSupported>false</tt:ZoomStatusSupported>");
	body.append("<tt:PanTiltPositionSupported>false</tt:PanTiltPositionSupported>");
	body.append("<tt:ZoomPositionSupported>false</tt:ZoomPositionSupported>");
	body.append("</tt:PTZStatusFilterOptions></trt:Options>");
	body.append("</trt:GetMetadataConfigurationOptionsResponse></s:Body></s:Envelope>");
	return 200;
}

int soapMedia::OnGetMetadataConfiguration(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:GetMetadataConfigurationResponse><trt:Configuration token=\"main\">");
	body.append("<tt:Name>MainStream_MetadataConfiguration</tt:Name><tt:UseCount>2</tt:UseCount><tt:PTZStatus>");
	body.append("<tt:Status>false</tt:Status><tt:Position>false</tt:Position>");
	body.append("</tt:PTZStatus><tt:Analytics>false</tt:Analytics><tt:Multicast><tt:Address>");
	body.append("<tt:Type>IPv4</tt:Type><tt:IPv4Address>224.2.0.0</tt:IPv4Address></tt:Address>");
	body.append("<tt:Port>40020</tt:Port><tt:TTL>64</tt:TTL><tt:AutoStart>false</tt:AutoStart></tt:Multicast>");
	body.append("<tt:SessionTimeout>PT0S</tt:SessionTimeout></trt:Configuration></trt:GetMetadataConfigurationResponse></s:Body></s:Envelope>");
	return 200;
}
int soapMedia::OnSetSynchronizationPoint(void* req, std::string& body)
{
	//TODO ǿ��I֡
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">");
	body.append("<s:Header/><s:Body><trt:SetSynchronizationPointResponse>");
	body.append("</trt:SetSynchronizationPointResponse></s:Body></s:Envelope>");
	return 200;
}

std::string _trt__GetVideoEncoderConfigurationsResponse::Serialization() {

    auto respNode = InitHeader("trt:GetVideoEncoderConfigurationsResponse");

    for (int i = 0; i < (int)VideoEncoder.size(); ++i) {
        auto vConfNode = _doc.NewElement("trt:Configurations");
        vConfNode->SetAttribute("GuaranteedFrameRate", "false");
        vConfNode->SetAttribute("token", VideoEncoder[i]->token.c_str());
        auto nNode = _doc.NewElement("tt:Name");
        nNode->SetText(VideoEncoder[i]->Name.c_str());
        auto useCountNode = _doc.NewElement("tt:UseCount");
        useCountNode->SetText(VideoEncoder[i]->UseCount);
        auto encodingNode = _doc.NewElement("tt:Encoding");
        encodingNode->SetText(VideoEncoder[i]->Encoding.c_str());

        auto resolutionNode = _doc.NewElement("tt:Resolution");
        auto wNode = _doc.NewElement("tt:Width");
        wNode->SetText(VideoEncoder[i]->Width);
        auto hNode = _doc.NewElement("tt:Height");
        hNode->SetText(VideoEncoder[i]->Height);
        resolutionNode->InsertEndChild(wNode);
        resolutionNode->InsertEndChild(hNode);

        auto qualityNode = _doc.NewElement("tt:Quality");
        qualityNode->SetText(VideoEncoder[i]->Quality);
        respNode->InsertEndChild(vConfNode);
        respNode->InsertEndChild(nNode);
        respNode->InsertEndChild(useCountNode);
        respNode->InsertEndChild(encodingNode);
        respNode->InsertEndChild(resolutionNode);
        respNode->InsertEndChild(qualityNode);

        if (VideoEncoder[i]->RateControl)
        {
            auto rcNode = _doc.NewElement("tt:RateControl");
            auto frlNode = _doc.NewElement("tt:FrameRateLimit");
            frlNode->SetText(VideoEncoder[i]->RateControl->FrameRateLimit);
            auto eiNode = _doc.NewElement("tt:EncodingInterval");
            eiNode->SetText(VideoEncoder[i]->RateControl->EncodingInterval);
            auto blNode = _doc.NewElement("tt:BitrateLimit");
            blNode->SetText(VideoEncoder[i]->RateControl->BitrateLimit);
            rcNode->InsertEndChild(frlNode);
            rcNode->InsertEndChild(eiNode);
            rcNode->InsertEndChild(blNode);
            respNode->InsertEndChild(rcNode);
        }

        if (VideoEncoder[i]->H264)
        {
            auto h264Node = _doc.NewElement("tt:H264");
            auto govLengthNode = _doc.NewElement("tt:GovLength");
            govLengthNode->SetText(VideoEncoder[i]->H264->GovLength);
            auto hpNode = _doc.NewElement("tt:H264Profile");
            switch (VideoEncoder[i]->H264->H264Profile)
            {
                case tt__H264Profile__Baseline:
                    hpNode->SetText("Baseline");
                    break;
                case tt__H264Profile__Main:
                    hpNode->SetText("Main");
                    break;
                case tt__H264Profile__Extended:
                    hpNode->SetText("Extended");
                    break;
                case tt__H264Profile__High:
                    hpNode->SetText("High");
                    break;
            }
            h264Node->InsertEndChild(govLengthNode);
            h264Node->InsertEndChild(hpNode);
            respNode->InsertEndChild(h264Node);
        }

        auto multicastNode = _doc.NewElement("tt:Multicast");
        auto addressNode = _doc.NewElement("tt:Address");
        auto typeNode = _doc.NewElement("tt:Type");
        typeNode->SetText(VideoEncoder[i]->Multicast.Address->Type.c_str());
        addressNode->InsertEndChild(typeNode);
        if (VideoEncoder[i]->Multicast.Address->IPv4Address)
        {
            auto ipv4Node = _doc.NewElement("tt:IPv4Address");
            ipv4Node->SetText(VideoEncoder[i]->Multicast.Address->IPv4Address->c_str());
            addressNode->InsertEndChild(ipv4Node);
        }
        if (VideoEncoder[i]->Multicast.Address->IPv6Address)
        {
            auto ipv6Node = _doc.NewElement("tt:IPv4Address");
            ipv6Node->SetText(VideoEncoder[i]->Multicast.Address->IPv6Address->c_str());
            addressNode->InsertEndChild(ipv6Node);
        }
        auto portNode = _doc.NewElement("tt:Port");
        portNode->SetText(VideoEncoder[i]->Multicast.Port);
        auto ttlNode = _doc.NewElement("tt:TTL");
        ttlNode->SetText(VideoEncoder[i]->Multicast.TTL);
        auto autoStartNode = _doc.NewElement("tt:AutoStart");
        autoStartNode->SetText(VideoEncoder[i]->Multicast.AutoStart);
        multicastNode->InsertEndChild(addressNode);
        multicastNode->InsertEndChild(portNode);
        multicastNode->InsertEndChild(ttlNode);
        multicastNode->InsertEndChild(autoStartNode);
        respNode->InsertEndChild(multicastNode);

        auto ptNode = _doc.NewElement("tt:SessionTimeout");
        ptNode->SetText(VideoEncoder[i]->SessionTimeout.c_str());
        respNode->InsertEndChild(ptNode);
    }

    tinyxml2::XMLPrinter printer(0, true);
    _doc.Print(&printer);
    return std::string(printer.CStr(), printer.CStrSize() - 1);
}

int soapMedia::OnGetVideoEncoderConfigurations(void* req, std::string& body)
{
    _trt__GetVideoEncoderConfigurationsResponse Response;

    int chn = 0;
    Emx::MediaClientVenc::GetChnNum(chn);
    for (int i = 0; i < chn; ++i) {
        Emx::MediaClientVenc venc(i);
        Emx::MediaVenc::Param vencParam = {};
        venc.GetParam(vencParam);

        std::shared_ptr<tt__VideoEncoderConfiguration> item(new tt__VideoEncoderConfiguration);
        if(0 == 1)
            item->token = VECMToken;
        else
            item->token = VECSToken;
        item->UseCount = 2;
        item->Name = "MainStream_VideoEncoderConfigurations";
        item->Encoding = (vencParam.codec == Emx::VideoCodecE::H264) ? "H264":"H265";
        item->Width = (int)vencParam.width;
        item->Height = (int)vencParam.height;
        item->Quality = 4.0f;
        item->RateControl = std::make_shared<tt__VideoRateControl>();
        item->RateControl->EncodingInterval = (int)vencParam.iFrameInv;
        item->RateControl->FrameRateLimit = (int)vencParam.fps;
        item->RateControl->BitrateLimit = (int)vencParam.bitRate;
        item->H264 = std::make_shared<tt__H264Configuration>();
        item->H264->GovLength = (int)(vencParam.fps*vencParam.iFrameInv);
        item->H264->H264Profile = tt__H264Profile__Baseline;
        item->Multicast.Address = std::make_shared<tt__IPAddress>();
        item->Multicast.Address->Type = "IPv4";
        item->Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
        item->Multicast.AutoStart = false;
        item->Multicast.Port = 40000;
        item->Multicast.TTL = 64;
        item->SessionTimeout = "PT60S";
        Response.VideoEncoder.emplace_back(item);
    }
    body = std::move(Response.Serialization());

    return 200;
}

int soapMedia::OnGetAudioEncoderConfigurations(void* req, std::string& body)
{

    Emx::MediaClientAenc aenc(0);
    Emx::MediaAenc::Param AencParam = {};
    aenc.GetParam(AencParam);

    _trt__GetAudioEncoderConfigurationsResponse Response;

    if (AencParam.codec == Emx::AudioCodecE::AAC){
        Response.AudioEncoderConfigurations.Encoding = "AAC";
    }else if (AencParam.codec == Emx::AudioCodecE::G711A){
        Response.AudioEncoderConfigurations.Encoding = "G711";
    }else if (AencParam.codec == Emx::AudioCodecE::G711U){
        Response.AudioEncoderConfigurations.Encoding = "G711";
    }
    Response.AudioEncoderConfigurations.token = AECToken;
    Response.AudioEncoderConfigurations.Name = "MainStream_AudioEncoderConfiguration";
    Response.AudioEncoderConfigurations.UseCount = 1;
    Response.AudioEncoderConfigurations.Bitrate = (int)AencParam.bitRate;
    Response.AudioEncoderConfigurations.SampleRate = (int)AencParam.sampleRate;
    Response.AudioEncoderConfigurations.Multicast.Address = std::make_shared<tt__IPAddress>();
    Response.AudioEncoderConfigurations.Multicast.Address->Type = "IPv4";
    Response.AudioEncoderConfigurations.Multicast.Address->IPv4Address = std::make_shared<std::string>("224.1.0.0");
    Response.AudioEncoderConfigurations.Multicast.Port = 40002;
    Response.AudioEncoderConfigurations.Multicast.TTL = 64;
    Response.AudioEncoderConfigurations.Multicast.AutoStart = false;
    Response.AudioEncoderConfigurations.SessionTimeout = "PT60S";

    body = std::move(Response.Serialization());
    return 200;
}

std::string _trt__GetAudioEncoderConfigurationsResponse::Serialization() {

    auto resNode = InitHeader("trt:GetAudioEncoderConfigurationsResponse");
    auto vConfNode = _doc.NewElement("trt:Configurations");
    vConfNode->SetAttribute("token", AudioEncoderConfigurations.token.c_str());
    auto nNode = _doc.NewElement("tt:Name");
    nNode->SetText(AudioEncoderConfigurations.Name.c_str());
    auto useCountNode = _doc.NewElement("tt:UseCount");
    useCountNode->SetText(AudioEncoderConfigurations.UseCount);
    auto encodingNode = _doc.NewElement("tt:Encoding");
    encodingNode->SetText(AudioEncoderConfigurations.Encoding.c_str());
    auto bitrateNode = _doc.NewElement("tt:Bitrate");
    bitrateNode->SetText(AudioEncoderConfigurations.Bitrate);
    auto sampleRateNode = _doc.NewElement("tt:SampleRate");
    sampleRateNode->SetText(AudioEncoderConfigurations.SampleRate);
    resNode->InsertEndChild(vConfNode);
    resNode->InsertEndChild(nNode);
    resNode->InsertEndChild(useCountNode);
    resNode->InsertEndChild(encodingNode);
    resNode->InsertEndChild(bitrateNode);
    resNode->InsertEndChild(sampleRateNode);

    auto multicastNode = _doc.NewElement("tt:Multicast");
    auto addressNode = _doc.NewElement("tt:Address");
    auto typeNode = _doc.NewElement("tt:Type");
    typeNode->SetText(AudioEncoderConfigurations.Multicast.Address->Type.c_str());
    addressNode->InsertEndChild(typeNode);
    if (AudioEncoderConfigurations.Multicast.Address->IPv4Address)
    {
        auto ipv4Node = _doc.NewElement("tt:IPv4Address");
        ipv4Node->SetText(AudioEncoderConfigurations.Multicast.Address->IPv4Address->c_str());
        addressNode->InsertEndChild(ipv4Node);
    }
    if (AudioEncoderConfigurations.Multicast.Address->IPv6Address)
    {
        auto ipv6Node = _doc.NewElement("tt:IPv4Address");
        ipv6Node->SetText(AudioEncoderConfigurations.Multicast.Address->IPv6Address->c_str());
        addressNode->InsertEndChild(ipv6Node);
    }
    auto portNode = _doc.NewElement("tt:Port");
    portNode->SetText(AudioEncoderConfigurations.Multicast.Port);
    auto ttlNode = _doc.NewElement("tt:TTL");
    ttlNode->SetText(AudioEncoderConfigurations.Multicast.TTL);
    auto autoStartNode = _doc.NewElement("tt:AutoStart");
    autoStartNode->SetText(AudioEncoderConfigurations.Multicast.AutoStart);
    multicastNode->InsertEndChild(addressNode);
    multicastNode->InsertEndChild(portNode);
    multicastNode->InsertEndChild(ttlNode);
    multicastNode->InsertEndChild(autoStartNode);
    resNode->InsertEndChild(multicastNode);

    auto ptNode = _doc.NewElement("tt:SessionTimeout");
    ptNode->SetText(AudioEncoderConfigurations.SessionTimeout.c_str());
    resNode->InsertEndChild(ptNode);


    tinyxml2::XMLPrinter printer(0, true);
    _doc.Print(&printer);
    return std::string(printer.CStr(), printer.CStrSize() - 1);
}
