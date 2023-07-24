#include "soapPTZ.h"
#include "soapUtil.h"
#include <time.h>
//#include <te_ptz.h>
//#include <te_env.h>

tinyxml2::XMLElement* BasePTZResponse::InitHeader(const std::string& tag)
{
	auto declare = _doc.NewDeclaration();
	_doc.InsertFirstChild(declare);

	/*Envelope*/
	auto envelopeNode = _doc.NewElement("s:Envelope");
	_doc.InsertEndChild(envelopeNode);
	envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
	envelopeNode->SetAttribute("xmlns:sc", "http://www.w3.org/2003/05/soap-encoding");
	envelopeNode->SetAttribute("xmlns:tt", "http://www.onvif.org/ver10/schema");
	envelopeNode->SetAttribute("xmlns:tptz", "http://www.onvif.org/ver20/ptz/wsdl");
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
BasePTZResponse::~BasePTZResponse()
{
}

std::string _tptz__GetConfigurationsResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetConfigurationsResponse");
	for (size_t i = 0; i < PTZConfiguration.size(); i++)
	{
		auto ptzCfgNode = _doc.NewElement("tptz:PTZConfiguration");
		if (PTZConfiguration[i]->MoveRamp)
			ptzCfgNode->SetAttribute("MoveRamp", *PTZConfiguration[i]->MoveRamp);
		if (PTZConfiguration[i]->PresetRamp)
			ptzCfgNode->SetAttribute("PresetRamp", *PTZConfiguration[i]->PresetRamp);
		if (PTZConfiguration[i]->PresetTourRamp)
			ptzCfgNode->SetAttribute("PresetTourRamp", *PTZConfiguration[i]->PresetTourRamp);
		ptzCfgNode->SetAttribute("token", PTZConfiguration[i]->token.c_str());
		auto nNode = _doc.NewElement("tt:Name");
		auto useCountNode = _doc.NewElement("tt:UseCount");
		auto nodeTokenNode = _doc.NewElement("tt:NodeToken");
		nNode->SetText(PTZConfiguration[i]->Name.c_str());
		useCountNode->SetText(PTZConfiguration[i]->UseCount);
		nodeTokenNode->SetText(PTZConfiguration[i]->NodeToken.c_str());
		ptzCfgNode->InsertEndChild(nNode);
		ptzCfgNode->InsertEndChild(useCountNode);
		ptzCfgNode->InsertEndChild(nodeTokenNode);
		
		if (PTZConfiguration[i]->DefaultAbsolutePantTiltPositionSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultAbsolutePantTiltPositionSpace");
			aNode->SetText(PTZConfiguration[i]->DefaultAbsolutePantTiltPositionSpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultAbsoluteZoomPositionSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultAbsoluteZoomPositionSpace");
			aNode->SetText(PTZConfiguration[i]->DefaultAbsoluteZoomPositionSpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultRelativePanTiltTranslationSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultRelativePanTiltTranslationSpace");
			aNode->SetText(PTZConfiguration[i]->DefaultRelativePanTiltTranslationSpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultRelativeZoomTranslationSpace)
		{
			auto aNode = _doc.NewElement("tt:DefaultRelativeZoomTranslationSpace");
			aNode->SetText(PTZConfiguration[i]->DefaultRelativeZoomTranslationSpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultContinuousPanTiltVelocitySpace) 
		{
			auto aNode = _doc.NewElement("tt:DefaultContinuousPanTiltVelocitySpace");
			aNode->SetText(PTZConfiguration[i]->DefaultContinuousPanTiltVelocitySpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultContinuousZoomVelocitySpace) 
		{
			auto aNode = _doc.NewElement("tt:DefaultContinuousZoomVelocitySpace");
			aNode->SetText(PTZConfiguration[i]->DefaultContinuousZoomVelocitySpace->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultPTZSpeed)
		{
			auto aNode = _doc.NewElement("tt:DefaultPTZSpeed");
			
			if(PTZConfiguration[i]->DefaultPTZSpeed->PanTilt)
			{ 
				auto panTiltNode = _doc.NewElement("tt:PanTilt");
				panTiltNode->SetAttribute("x", PTZConfiguration[i]->DefaultPTZSpeed->PanTilt->x);
				panTiltNode->SetAttribute("y", PTZConfiguration[i]->DefaultPTZSpeed->PanTilt->y);
				if (PTZConfiguration[i]->DefaultPTZSpeed->PanTilt->space)
					panTiltNode->SetAttribute("space", PTZConfiguration[i]->DefaultPTZSpeed->PanTilt->space->c_str());
				
				aNode->InsertEndChild(panTiltNode);
			}
			if (PTZConfiguration[i]->DefaultPTZSpeed->Zoom)
			{
				auto zoomNode = _doc.NewElement("tt:Zoom");
				zoomNode->SetAttribute("x", PTZConfiguration[i]->DefaultPTZSpeed->Zoom->x);
				if (PTZConfiguration[i]->DefaultPTZSpeed->Zoom->space)
					zoomNode->SetAttribute("space", PTZConfiguration[i]->DefaultPTZSpeed->Zoom->space->c_str());
				aNode->InsertEndChild(zoomNode);
			}
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->DefaultPTZTimeout)
		{
			auto aNode = _doc.NewElement("tt:DefaultPTZTimeout");
			aNode->SetText(PTZConfiguration[i]->DefaultPTZTimeout->c_str());
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->PanTiltLimits)
		{
			auto aNode = _doc.NewElement("tt:PanTiltLimits");
			auto rangeNode = _doc.NewElement("tt:Range");
			aNode->InsertEndChild(rangeNode);
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZConfiguration[i]->PanTiltLimits->Range.URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMinNode->SetText(PTZConfiguration[i]->PanTiltLimits->Range.xMin);
			xMaxNode->SetText(PTZConfiguration[i]->PanTiltLimits->Range.xMax);
			xRangeNode->InsertEndChild(xMinNode);
			xRangeNode->InsertEndChild(xMaxNode);
			auto yRangeNode = _doc.NewElement("tt:YRange");
			auto yMinNode = _doc.NewElement("tt:Min");
			auto yMaxNode = _doc.NewElement("tt:Max");
			yMinNode->SetText(PTZConfiguration[i]->PanTiltLimits->Range.yMin);
			yMaxNode->SetText(PTZConfiguration[i]->PanTiltLimits->Range.yMax);
			yRangeNode->InsertEndChild(yMinNode);
			yRangeNode->InsertEndChild(yMaxNode);

			rangeNode->InsertEndChild(uriNode);
			rangeNode->InsertEndChild(xRangeNode);
			rangeNode->InsertEndChild(yRangeNode);
			ptzCfgNode->InsertEndChild(aNode);
		}
		if (PTZConfiguration[i]->ZoomLimits)
		{
			auto aNode = _doc.NewElement("tt:ZoomLimits");
			auto rangeNode = _doc.NewElement("tt:Range");
			aNode->InsertEndChild(rangeNode);
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZConfiguration[i]->ZoomLimits->Range.URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZConfiguration[i]->ZoomLimits->Range.Min);
			maxNode->SetText(PTZConfiguration[i]->ZoomLimits->Range.Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);

			rangeNode->InsertEndChild(uriNode);
			rangeNode->InsertEndChild(xRangeNode);
			ptzCfgNode->InsertEndChild(aNode);
		}
		respNode->InsertEndChild(ptzCfgNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tptz__GetPresetsResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetPresetsResponse");
	for (auto& item : Preset)
	{
		auto itemNode = _doc.NewElement("tptz:Preset");
		itemNode->SetAttribute("token", item->token.c_str());
		if (!item->Name.empty())
		{
			auto a = _doc.NewElement("tt:Name");
			a->SetText(item->Name.c_str());
			itemNode->InsertEndChild(a);
		}
		if (item->PTZPosition)
		{
			auto a = _doc.NewElement("tt:PTZPosition");
			if (item->PTZPosition->PanTilt)
			{
				auto b = _doc.NewElement("tt:PanTilt");
				b->SetAttribute("x", item->PTZPosition->PanTilt->x);
				b->SetAttribute("y", item->PTZPosition->PanTilt->y);
				a->InsertEndChild(b);
			}
			if (item->PTZPosition->Zoom)
			{
				auto b = _doc.NewElement("tt:Zoom");
				b->SetAttribute("x", item->PTZPosition->Zoom->x);
				a->InsertEndChild(b);
			}
			itemNode->InsertEndChild(a);
		}
		respNode->InsertEndChild(itemNode);
	}

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tptz__GotoPresetResponse::Serialization()
{
//	auto respNode = InitHeader("tptz:GotoPresetResponse");
	InitHeader("tptz:GotoPresetResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tptz__GetConfigurationResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetConfigurationsResponse");
	
	auto ptzCfgNode = _doc.NewElement("tptz:PTZConfiguration");
	

	ptzCfgNode->SetAttribute("MoveRamp", 1);
	ptzCfgNode->SetAttribute("PresetRamp", 1);
	ptzCfgNode->SetAttribute("PresetTourRamp", 1);
	ptzCfgNode->SetAttribute("token", 1);
	respNode->InsertEndChild(ptzCfgNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tptz__GetNodesResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetNodesResponse");
	for (size_t i = 0; i < PTZNode.size(); i++)
	{
		auto pnNode = _doc.NewElement("tptz:PTZNode");
		respNode->InsertEndChild(pnNode);
		if (PTZNode[i]->FixedHomePosition)
			pnNode->SetAttribute("FixedHomePosition", *PTZNode[i]->FixedHomePosition);
		if (PTZNode[i]->GeoMove)
			pnNode->SetAttribute("GeoMove", *PTZNode[i]->GeoMove);
		pnNode->SetAttribute("token", *PTZNode[i]->token.c_str());
		if (PTZNode[i]->Name)
		{
			auto nNode = _doc.NewElement("tt:Name");
			nNode->SetText(PTZNode[i]->Name->c_str());
			pnNode->InsertEndChild(nNode);
		}
		
		auto ptzSpaceNode = _doc.NewElement("tt:SupportedPTZSpaces");
		pnNode->InsertEndChild(ptzSpaceNode);
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:AbsolutePanTiltPositionSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->xMin);
			xMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->xMax);
			xRangeNode->InsertEndChild(xMinNode);
			xRangeNode->InsertEndChild(xMaxNode);
			auto yRangeNode = _doc.NewElement("tt:YRange");
			auto yMinNode = _doc.NewElement("tt:Min");
			auto yMaxNode = _doc.NewElement("tt:Max");
			yMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->yMin);
			yMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->yMax);
			yRangeNode->InsertEndChild(yMinNode);
			yRangeNode->InsertEndChild(yMaxNode);

			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			absNode->InsertEndChild(yRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.AbsoluteZoomPositionSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:AbsoluteZoomPositionSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->Min);
			maxNode->SetText(PTZNode[i]->SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);
			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:RelativePanTiltTranslationSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->xMin);
			xMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->xMax);
			xRangeNode->InsertEndChild(xMinNode);
			xRangeNode->InsertEndChild(xMaxNode);
			auto yRangeNode = _doc.NewElement("tt:YRange");
			auto yMinNode = _doc.NewElement("tt:Min");
			auto yMaxNode = _doc.NewElement("tt:Max");
			yMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->yMin);
			yMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->yMax);
			yRangeNode->InsertEndChild(yMinNode);
			yRangeNode->InsertEndChild(yMaxNode);

			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			absNode->InsertEndChild(yRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.RelativeZoomTranslationSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:RelativeZoomTranslationSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->Min);
			maxNode->SetText(PTZNode[i]->SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);
			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:ContinuousPanTiltVelocitySpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto xMinNode = _doc.NewElement("tt:Min");
			auto xMaxNode = _doc.NewElement("tt:Max");
			xMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->xMin);
			xMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->xMax);
			xRangeNode->InsertEndChild(xMinNode);
			xRangeNode->InsertEndChild(xMaxNode);
			auto yRangeNode = _doc.NewElement("tt:YRange");
			auto yMinNode = _doc.NewElement("tt:Min");
			auto yMaxNode = _doc.NewElement("tt:Max");
			yMinNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->yMin);
			yMaxNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->yMax);
			yRangeNode->InsertEndChild(yMinNode);
			yRangeNode->InsertEndChild(yMaxNode);

			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			absNode->InsertEndChild(yRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.ContinuousZoomVelocitySpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:ContinuousZoomVelocitySpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->Min);
			maxNode->SetText(PTZNode[i]->SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);
			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.PanTiltSpeedSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:PanTiltSpeedSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.PanTiltSpeedSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZNode[i]->SupportedPTZSpaces.PanTiltSpeedSpace[j]->Min);
			maxNode->SetText(PTZNode[i]->SupportedPTZSpaces.PanTiltSpeedSpace[j]->Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);
			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		for (size_t j = 0; j < PTZNode[i]->SupportedPTZSpaces.ZoomSpeedSpace.size(); j++)
		{
			auto absNode = _doc.NewElement("tt:ZoomSpeedSpace");
			auto uriNode = _doc.NewElement("tt:URI");
			uriNode->SetText(PTZNode[i]->SupportedPTZSpaces.ZoomSpeedSpace[j]->URI.c_str());
			auto xRangeNode = _doc.NewElement("tt:XRange");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(PTZNode[i]->SupportedPTZSpaces.ZoomSpeedSpace[j]->Min);
			maxNode->SetText(PTZNode[i]->SupportedPTZSpaces.ZoomSpeedSpace[j]->Max);
			xRangeNode->InsertEndChild(minNode);
			xRangeNode->InsertEndChild(maxNode);
			absNode->InsertEndChild(uriNode);
			absNode->InsertEndChild(xRangeNode);
			ptzSpaceNode->InsertEndChild(absNode);
		}
		auto mnNode = _doc.NewElement("tt:MaximumNumberOfPresets");
		mnNode->SetText(PTZNode[i]->MaximumNumberOfPresets);
		pnNode->InsertEndChild(mnNode);
		auto hsNode = _doc.NewElement("tt:HomeSupported");
		hsNode->SetText(PTZNode[i]->HomeSupported);
		pnNode->InsertEndChild(hsNode);
		for (size_t j = 0; i < PTZNode[i]->AuxiliaryCommands.size(); j++)
		{
			auto acNode = _doc.NewElement("tt:AuxiliaryCommands");
			acNode->SetText(PTZNode[i]->AuxiliaryCommands[j].c_str());
			pnNode->InsertEndChild(acNode);
		}
	}
	
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _tptz__GetNodeResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetNodeResponse");

	auto pnNode = _doc.NewElement("tptz:PTZNode");
	respNode->InsertEndChild(pnNode);
	if (PTZNode.FixedHomePosition)
		pnNode->SetAttribute("FixedHomePosition", *PTZNode.FixedHomePosition);
	if (PTZNode.GeoMove)
		pnNode->SetAttribute("GeoMove", *PTZNode.GeoMove);
	pnNode->SetAttribute("token", PTZNode.token.c_str());
	if (PTZNode.Name)
	{
		auto nNode = _doc.NewElement("tt:Name");
		nNode->SetText(PTZNode.Name->c_str());
		pnNode->InsertEndChild(nNode);
	}

	auto ptzSpaceNode = _doc.NewElement("tt:SupportedPTZSpaces");
	pnNode->InsertEndChild(ptzSpaceNode);
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:AbsolutePanTiltPositionSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto xMinNode = _doc.NewElement("tt:Min");
		auto xMaxNode = _doc.NewElement("tt:Max");
		xMinNode->SetText(PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->xMin);
		xMaxNode->SetText(PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->xMax);
		xRangeNode->InsertEndChild(xMinNode);
		xRangeNode->InsertEndChild(xMaxNode);
		auto yRangeNode = _doc.NewElement("tt:YRange");
		auto yMinNode = _doc.NewElement("tt:Min");
		auto yMaxNode = _doc.NewElement("tt:Max");
		yMinNode->SetText(PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->yMin);
		yMaxNode->SetText(PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace[j]->yMax);
		yRangeNode->InsertEndChild(yMinNode);
		yRangeNode->InsertEndChild(yMaxNode);

		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		absNode->InsertEndChild(yRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:AbsoluteZoomPositionSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->Min);
		maxNode->SetText(PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace[j]->Max);
		xRangeNode->InsertEndChild(minNode);
		xRangeNode->InsertEndChild(maxNode);
		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:RelativePanTiltTranslationSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto xMinNode = _doc.NewElement("tt:Min");
		auto xMaxNode = _doc.NewElement("tt:Max");
		xMinNode->SetText(PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->xMin);
		xMaxNode->SetText(PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->xMax);
		xRangeNode->InsertEndChild(xMinNode);
		xRangeNode->InsertEndChild(xMaxNode);
		auto yRangeNode = _doc.NewElement("tt:YRange");
		auto yMinNode = _doc.NewElement("tt:Min");
		auto yMaxNode = _doc.NewElement("tt:Max");
		yMinNode->SetText(PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->yMin);
		yMaxNode->SetText(PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace[j]->yMax);
		yRangeNode->InsertEndChild(yMinNode);
		yRangeNode->InsertEndChild(yMaxNode);

		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		absNode->InsertEndChild(yRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:RelativeZoomTranslationSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->Min);
		maxNode->SetText(PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace[j]->Max);
		xRangeNode->InsertEndChild(minNode);
		xRangeNode->InsertEndChild(maxNode);
		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:ContinuousPanTiltVelocitySpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto xMinNode = _doc.NewElement("tt:Min");
		auto xMaxNode = _doc.NewElement("tt:Max");
		xMinNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->xMin);
		xMaxNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->xMax);
		xRangeNode->InsertEndChild(xMinNode);
		xRangeNode->InsertEndChild(xMaxNode);
		auto yRangeNode = _doc.NewElement("tt:YRange");
		auto yMinNode = _doc.NewElement("tt:Min");
		auto yMaxNode = _doc.NewElement("tt:Max");
		yMinNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->yMin);
		yMaxNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace[j]->yMax);
		yRangeNode->InsertEndChild(yMinNode);
		yRangeNode->InsertEndChild(yMaxNode);

		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		absNode->InsertEndChild(yRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:ContinuousZoomVelocitySpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->Min);
		maxNode->SetText(PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace[j]->Max);
		xRangeNode->InsertEndChild(minNode);
		xRangeNode->InsertEndChild(maxNode);
		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:PanTiltSpeedSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace[j]->Min);
		maxNode->SetText(PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace[j]->Max);
		xRangeNode->InsertEndChild(minNode);
		xRangeNode->InsertEndChild(maxNode);
		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	for (size_t j = 0; j < PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.size(); j++)
	{
		auto absNode = _doc.NewElement("tt:ZoomSpeedSpace");
		auto uriNode = _doc.NewElement("tt:URI");
		uriNode->SetText(PTZNode.SupportedPTZSpaces.ZoomSpeedSpace[j]->URI.c_str());
		auto xRangeNode = _doc.NewElement("tt:XRange");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(PTZNode.SupportedPTZSpaces.ZoomSpeedSpace[j]->Min);
		maxNode->SetText(PTZNode.SupportedPTZSpaces.ZoomSpeedSpace[j]->Max);
		xRangeNode->InsertEndChild(minNode);
		xRangeNode->InsertEndChild(maxNode);
		absNode->InsertEndChild(uriNode);
		absNode->InsertEndChild(xRangeNode);
		ptzSpaceNode->InsertEndChild(absNode);
	}
	auto mnNode = _doc.NewElement("tt:MaximumNumberOfPresets");
	mnNode->SetText(PTZNode.MaximumNumberOfPresets);
	pnNode->InsertEndChild(mnNode);
	auto hsNode = _doc.NewElement("tt:HomeSupported");
	hsNode->SetText(PTZNode.HomeSupported);
	pnNode->InsertEndChild(hsNode);
	for (size_t i = 0; i < PTZNode.AuxiliaryCommands.size(); i++)
	{
		auto acNode = _doc.NewElement("tt:AuxiliaryCommands");
		acNode->SetText(PTZNode.AuxiliaryCommands[i].c_str());
		pnNode->InsertEndChild(acNode);
	}

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _tptz__GetStatusResponse::Serialization()
{
	auto respNode = InitHeader("tptz:GetStatusResponse");
	auto statusNode = _doc.NewElement("tptz:PTZStatus");
	respNode->InsertEndChild(statusNode);
	if (PTZStatus.Position)
	{
		auto a = _doc.NewElement("tt:Position");
		if (PTZStatus.Position->PanTilt)
		{
			auto b= _doc.NewElement("tt:PanTilt");
			b->SetAttribute("x", PTZStatus.Position->PanTilt->x);
			b->SetAttribute("y", PTZStatus.Position->PanTilt->y);
			if (PTZStatus.Position->PanTilt->space)
				b->SetAttribute("space", PTZStatus.Position->PanTilt->space->c_str());

			a->InsertEndChild(b);
		}
		if (PTZStatus.Position->Zoom)
		{
			auto b = _doc.NewElement("tt:Zoom");
			b->SetAttribute("x", PTZStatus.Position->Zoom->x);
			if (PTZStatus.Position->Zoom->space)
				b->SetAttribute("space", PTZStatus.Position->Zoom->space->c_str());

			a->InsertEndChild(b);
		}
		statusNode->InsertEndChild(a);
	}
	if (PTZStatus.MoveStatus)
	{
		auto a = _doc.NewElement("tt:MoveStatus");
		auto b = _doc.NewElement("tt:PanTilt");
		b->SetText(PTZStatus.MoveStatus->PanTilt.c_str());
		auto c = _doc.NewElement("tt:Zoom");
		c->SetText(PTZStatus.MoveStatus->Zoom.c_str());
		a->InsertEndChild(b);
		a->InsertEndChild(c);
		statusNode->InsertEndChild(a);
	}
	auto errorNode = _doc.NewElement("tt:Error");
	errorNode->SetText(PTZStatus.Error.c_str());
	statusNode->InsertEndChild(errorNode);

	auto utcNode = _doc.NewElement("tt:UtcTime");
	utcNode->SetText(SoapUtil::soap_createTime(PTZStatus.UtcTime).c_str());
	statusNode->InsertEndChild(utcNode);

	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
int soapPTZ::OnGetConfigurations(void* req, std::string& body)
{
	_tptz__GetConfigurationsResponse resp;
	std::shared_ptr<tt__PTZConfiguration> cfg(new tt__PTZConfiguration);
	cfg->token = "Profile_01";
	cfg->Name = "PTZConfiguration";
	cfg->UseCount = 1;
	cfg->NodeToken = "PTZNODETOKEN";
	cfg->MoveRamp = std::make_shared<int>(0);
	cfg->DefaultAbsolutePantTiltPositionSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/PositionGenericSpace");
	cfg->DefaultAbsoluteZoomPositionSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/PositionGenericSpace");
	cfg->DefaultRelativePanTiltTranslationSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/TranslationGenericSpace");
	cfg->DefaultRelativeZoomTranslationSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/TranslationGenericSpace");
	cfg->DefaultContinuousPanTiltVelocitySpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/VelocityGenericSpace");
	cfg->DefaultContinuousZoomVelocitySpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/VelocityGenericSpace");
	cfg->DefaultPTZSpeed = std::make_shared<tt__PTZSpeed>();
	cfg->DefaultPTZSpeed->PanTilt = std::make_shared<tt__Vector2D>();
	cfg->DefaultPTZSpeed->PanTilt->x = 1.f;
	cfg->DefaultPTZSpeed->PanTilt->y = 1.f;
	cfg->DefaultPTZSpeed->Zoom = std::make_shared<tt__Vector1D>();
	cfg->DefaultPTZSpeed->Zoom->x = 1.f;
	cfg->DefaultPTZTimeout = std::make_shared<std::string>("PT10S");
	cfg->PanTiltLimits = std::make_shared<tt__PanTiltLimits>();
	cfg->PanTiltLimits->Range.URI = "http://www.onvif.org/ver20/tptz/PanTiltSpaces/PositionGenericSpace";
	cfg->PanTiltLimits->Range.xMin = -1.f;
	cfg->PanTiltLimits->Range.xMax = 1.f;
	cfg->PanTiltLimits->Range.yMin = -1.f;
	cfg->PanTiltLimits->Range.yMax = 1.f;
	cfg->ZoomLimits = std::make_shared<tt__ZoomLimits>();
	cfg->ZoomLimits->Range.URI = "http://www.onvif.org/ver20/tptz/ZoomSpaces/PositionGenericSpace";
	cfg->ZoomLimits->Range.Min = 0.f;
	cfg->ZoomLimits->Range.Max = 1.f;
	resp.PTZConfiguration.emplace_back(std::move(cfg));
	body = std::move(resp.Serialization());
	return 0;
}

int soapPTZ::OnGetPresets(void* req, std::string& body)
{
	_tptz__GetPresetsResponse resp;
	std::shared_ptr<tt__PTZPreset> item1(new tt__PTZPreset);
	item1->Name = "preset-1";
	item1->token = "1";
	item1->PTZPosition.reset(new tt__PTZVector);
	item1->PTZPosition->PanTilt.reset(new tt__Vector2D);
	item1->PTZPosition->PanTilt->x = 0.0f;
	item1->PTZPosition->PanTilt->y = 0.1f;
	item1->PTZPosition->Zoom.reset(new tt__Vector1D);
	item1->PTZPosition->Zoom->x = 0.1f;

	resp.Preset.push_back(item1);
	body = std::move(resp.Serialization());
	return 0;
}

int soapPTZ::OnGotoPreset(void* req, std::string& body) 
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
	std::string presetToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto gotoPresetNode = child->FirstChildElement();
		for (auto i = gotoPresetNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
			else if (strstr(i->Value(), "PresetToken"))
				presetToken = i->GetText();
			else if (strstr(i->Value(), "Speed"))
			{
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:GotoPresetResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnGetConfiguration(void* req, std::string& body)
{
	_tptz__GetConfigurationResponse resp;
	resp.PTZConfiguration = std::make_shared<tt__PTZConfiguration>();
	resp.PTZConfiguration->token = "Profile_01";
	resp.PTZConfiguration->Name = "PTZConfiguration";
	resp.PTZConfiguration->UseCount = 1;
	resp.PTZConfiguration->NodeToken = "PTZNODETOKEN";
	resp.PTZConfiguration->MoveRamp = std::make_shared<int>(0);
	resp.PTZConfiguration->DefaultAbsolutePantTiltPositionSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/PositionGenericSpace");
	resp.PTZConfiguration->DefaultAbsoluteZoomPositionSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/PositionGenericSpace");
	resp.PTZConfiguration->DefaultRelativePanTiltTranslationSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/TranslationGenericSpace");
	resp.PTZConfiguration->DefaultRelativeZoomTranslationSpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/TranslationGenericSpace");
	resp.PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/PanTiltSpaces/VelocityGenericSpace");
	resp.PTZConfiguration->DefaultContinuousZoomVelocitySpace = std::make_shared<std::string>("http://www.onvif.org/ver20/tptz/ZoomSpaces/VelocityGenericSpace");
	resp.PTZConfiguration->DefaultPTZSpeed = std::make_shared<tt__PTZSpeed>();
	resp.PTZConfiguration->DefaultPTZSpeed->PanTilt = std::make_shared<tt__Vector2D>();
	resp.PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.f;
	resp.PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 1.f;
	resp.PTZConfiguration->DefaultPTZSpeed->Zoom = std::make_shared<tt__Vector1D>();
	resp.PTZConfiguration->DefaultPTZSpeed->Zoom->x = 1.f;
	resp.PTZConfiguration->DefaultPTZTimeout = std::make_shared<std::string>("PT10S");
	resp.PTZConfiguration->PanTiltLimits = std::make_shared<tt__PanTiltLimits>();
	resp.PTZConfiguration->PanTiltLimits->Range.URI = "http://www.onvif.org/ver20/tptz/PanTiltSpaces/PositionGenericSpace";
	resp.PTZConfiguration->PanTiltLimits->Range.xMin = -1.f;
	resp.PTZConfiguration->PanTiltLimits->Range.xMax = 1.f;
	resp.PTZConfiguration->PanTiltLimits->Range.yMin = -1.f;
	resp.PTZConfiguration->PanTiltLimits->Range.yMax = 1.f;
	resp.PTZConfiguration->ZoomLimits = std::make_shared<tt__ZoomLimits>();
	resp.PTZConfiguration->ZoomLimits->Range.URI = "http://www.onvif.org/ver20/tptz/ZoomSpaces/PositionGenericSpace";
	resp.PTZConfiguration->ZoomLimits->Range.Min = 0.f;
	resp.PTZConfiguration->ZoomLimits->Range.Max = 1.f;

	body = std::move(resp.Serialization());
	return 200;
}

int soapPTZ::OnGetNodes(void* req, std::string& body)
{
	_tptz__GetNodesResponse resp;
	std::shared_ptr<tt__PTZNode> nd(new tt__PTZNode);
	nd->Name = std::make_shared<std::string>("ptz");
	nd->token = "PTZNODETOKEN";
	nd->GeoMove = std::make_shared<bool>(false);
	nd->HomeSupported = true;
	nd->MaximumNumberOfPresets = 1;
	
	std::shared_ptr<tt__Space2DDescription> a(new tt__Space2DDescription);
	a->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	a->xMin = -1.f;
	a->xMax = 1.f;
	a->yMin = -1.f;
	a->yMax = 1.f;
	nd->SupportedPTZSpaces.AbsolutePanTiltPositionSpace.emplace_back(std::move(a));

	std::shared_ptr<tt__Space1DDescription> b(new tt__Space1DDescription);
	b->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	b->Min = -1.0f;
	b->Max = 1.0f;
	nd->SupportedPTZSpaces.AbsoluteZoomPositionSpace.emplace_back(std::move(b));

	std::shared_ptr<tt__Space2DDescription> c1(new tt__Space2DDescription);
	c1->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";//�������ռ䶨��ʵ��û�㶮-_-
	c1->xMin = -1.f;
	c1->xMax = 1.f;
	c1->yMin = -1.f;
	c1->yMax = 1.f;
	nd->SupportedPTZSpaces.RelativePanTiltTranslationSpace.emplace_back(std::move(c1));

	std::shared_ptr<tt__Space2DDescription> c(new tt__Space2DDescription);
	c->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationSpaceFov";
	c->xMin = -1.f;
	c->xMax = 1.f;
	c->yMin = -1.f;
	c->yMax = 1.f;
	nd->SupportedPTZSpaces.RelativePanTiltTranslationSpace.emplace_back(std::move(c));

	std::shared_ptr<tt__Space1DDescription> d(new tt__Space1DDescription);
	d->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace";
	d->Min = -1.0f;
	d->Max = 1.0f;
	nd->SupportedPTZSpaces.RelativeZoomTranslationSpace.emplace_back(std::move(d));

	std::shared_ptr<tt__Space2DDescription> e(new tt__Space2DDescription);
	e->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceFOV";
	e->xMin = -1.f;
	e->xMax = 1.f;
	e->yMin = -1.f;
	e->yMax = 1.f;
	nd->SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.emplace_back(std::move(e));

	std::shared_ptr<tt__Space1DDescription> f(new tt__Space1DDescription);
	f->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace";
	f->Min = -1.0f;
	f->Max = 1.0f;
	nd->SupportedPTZSpaces.ContinuousZoomVelocitySpace.emplace_back(std::move(f));

	std::shared_ptr<tt__Space1DDescription> g(new tt__Space1DDescription);
	g->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace";
	g->Min = 0.0f;
	g->Max = 1.0f;
	nd->SupportedPTZSpaces.PanTiltSpeedSpace.emplace_back(std::move(g));

	std::shared_ptr<tt__Space1DDescription> h(new tt__Space1DDescription);
	h->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	h->Min = 0.0f;
	h->Max = 1.0f;
	nd->SupportedPTZSpaces.ZoomSpeedSpace.emplace_back(std::move(h));
	
	resp.PTZNode.emplace_back(std::move(nd));
	body = std::move(resp.Serialization());
	return 200;
}

int soapPTZ::OnGetNode(void* req, std::string& body)
{
	_tptz__GetNodeResponse resp;
	resp.PTZNode.Name.reset(new std::string("ptz"));
	resp.PTZNode.token = "PTZNODETOKEN";
	resp.PTZNode.GeoMove = std::make_shared<bool>(false);
	resp.PTZNode.HomeSupported = true;
	resp.PTZNode.MaximumNumberOfPresets = 1;

	std::shared_ptr<tt__Space2DDescription> a(new tt__Space2DDescription);
	a->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	a->xMin = -1.f;
	a->xMax = 1.f;
	a->yMin = -1.f;
	a->yMax = 1.f;
	resp.PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.emplace_back(std::move(a));

	std::shared_ptr<tt__Space1DDescription> b(new tt__Space1DDescription);
	b->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	b->Min = -1.0f;
	b->Max = 1.0f;
	resp.PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.emplace_back(std::move(b));

	std::shared_ptr<tt__Space2DDescription> c1(new tt__Space2DDescription);
	c1->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";//�������ռ䶨��ʵ��û�㶮-_-
	c1->xMin = -1.f;
	c1->xMax = 1.f;
	c1->yMin = -1.f;
	c1->yMax = 1.f;
	resp.PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.emplace_back(std::move(c1));

	std::shared_ptr<tt__Space2DDescription> c(new tt__Space2DDescription);
	c->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationSpaceFov";
	c->xMin = -1.f;
	c->xMax = 1.f;
	c->yMin = -1.f;
	c->yMax = 1.f;
	resp.PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.emplace_back(std::move(c));

	std::shared_ptr<tt__Space1DDescription> d(new tt__Space1DDescription);
	d->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace";
	d->Min = -1.0f;
	d->Max = 1.0f;
	resp.PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.emplace_back(std::move(d));

	std::shared_ptr<tt__Space2DDescription> e(new tt__Space2DDescription);
	e->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceFOV";
	e->xMin = -1.f;
	e->xMax = 1.f;
	e->yMin = -1.f;
	e->yMax = 1.f;
	resp.PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.emplace_back(std::move(e));

	std::shared_ptr<tt__Space1DDescription> f(new tt__Space1DDescription);
	f->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace";
	f->Min = -1.0f;
	f->Max = 1.0f;
	resp.PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.emplace_back(std::move(f));

	std::shared_ptr<tt__Space1DDescription> g(new tt__Space1DDescription);
	g->URI = "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace";
	g->Min = 0.0f;
	g->Max = 1.0f;
	resp.PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.emplace_back(std::move(g));

	std::shared_ptr<tt__Space1DDescription> h(new tt__Space1DDescription);
	h->URI = "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	h->Min = 0.0f;
	h->Max = 1.0f;
	resp.PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.emplace_back(std::move(h));

	body = std::move(resp.Serialization());
	return 200;
}

int soapPTZ::OnGetConfigurationOptions(void* req, std::string& body)
{
	
	return 200;
}

int soapPTZ::OnContinuousMove(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
//	double px = 0;//+ right, - left
//    double py = 0;//+ up, - down
//    double spx = 0;
//    double spy = 0;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto cMoveNode = child->FirstChildElement();
		for (auto i = cMoveNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
			else if (strstr(i->Value(), "Velocity"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
//						px = atof(j->Attribute("x"));
//						py = atof(j->Attribute("y"));
					}
				}
			}
			else if (strstr(i->Value(), "Timeout"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
//						spx = atof(j->Attribute("x"));
//						spy = atof(j->Attribute("y"));
					}
				}
			}
		}
	}

	/*TODO*/

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:ContinuousMoveResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnRelativeMove(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
	int px = 0;
	int py = 0;
//	int spx = 0;
//	int spy = 0;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto relMoveNode = child->FirstChildElement();
		for (auto i = relMoveNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
			else if (strstr(i->Value(), "Position"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
						px = atoi(j->Attribute("x"));
						py = atoi(j->Attribute("y"));
					}
				}
			}
			else if (strstr(i->Value(), "Speed"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
//						spx = atoi(j->Attribute("x"));
//						spy = atoi(j->Attribute("y"));
					}
				}
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:RelativeMoveResponse/></s:Body></s:Envelope>\n");


	printf("Relative Move x=%d,y=%d\n", px, py);
	return 200;
}

int soapPTZ::OnAbsoluteMove(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
//	int px = -2;
//	int py = -2;
//	int spx = 0;
//	int spy = 0;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto absMoveNode = child->FirstChildElement();
		for (auto i = absMoveNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
			else if(strstr(i->Value(), "Position"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
//						px = atoi(j->Attribute("x"));
//						py = atoi(j->Attribute("y"));
					}
				}
			}
			else if (strstr(i->Value(), "Speed"))
			{
				for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
				{
//					auto gg = j->Value();
					if (strstr(j->Value(), "PanTilt"))
					{
//						spx = atoi(j->Attribute("x"));
//						spy = atoi(j->Attribute("y"));
					}
				}
			}
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:AbsoluteMoveResponse/></s:Body></s:Envelope>\n");

	return 200;
}

int soapPTZ::OnGeoMove(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:GeoMoveResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnStop(void* req, std::string& body)
{
	/*todo*/
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:StopResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnGetStatus(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto getStatusNode = child->FirstChildElement();
		for (auto i = getStatusNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
		}
	}
	/*TODO*/
	_tptz__GetStatusResponse resp;
	resp.PTZStatus.Error = "NO error";
	resp.PTZStatus.UtcTime = time(NULL);
	resp.PTZStatus.MoveStatus.reset(new tt__PTZMoveStatus);
	resp.PTZStatus.MoveStatus->PanTilt = "IDLE";
	resp.PTZStatus.MoveStatus->Zoom = "IDLE";
	resp.PTZStatus.Position.reset(new tt__PTZVector);
	resp.PTZStatus.Position->PanTilt.reset(new tt__Vector2D);
	resp.PTZStatus.Position->PanTilt->x = 0.0f;
	resp.PTZStatus.Position->PanTilt->y = 0.0f;
	resp.PTZStatus.Position->Zoom.reset(new tt__Vector1D);
	resp.PTZStatus.Position->Zoom->x = 0.0f;

	body = std::move(resp.Serialization());
	return 200;
}

int soapPTZ::OnGotoHomePosition(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:GotoHomePositionResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnRemovePreset(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string profileToken;
	std::string presetToken;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto removePresetNode = child->FirstChildElement();
		for (auto i = removePresetNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
//			auto x = i->Value();
//			auto y = i->Name();
			if (strstr(i->Value(), "ProfileToken"))
				profileToken = i->GetText();
			else if (strstr(i->Value(), "PresetToken"))
				presetToken = i->GetText();
		}
	}

	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:RemovePresetResponse/></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnSetPreset(void* req, std::string& body)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	std::string token;
	std::string presetName;
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto setpresetNode = child->FirstChildElement();
		for (auto i = setpresetNode->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strstr(i->Value(), "ProfileToken"))
				token = i->GetText();
			else if (strstr(i->Value(), "PresetName"))
				presetName = i->GetText();
		}
	}


	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:SetPresetResponse>\n<tptz:PresetToken>");
	body.append(presetName);//presettoken
	body.append("</tptz:PresetToken></tptz:SetPresetResponse></s:Body></s:Envelope>\n");
	return 200;
}

int soapPTZ::OnSetHomePosition(void* req, std::string& body)
{
	body.clear();
	body.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	body.append("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:sc=\"http://www.w3.org/2003/05/soap-encoding\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">");
	body.append("<s:Header/><s:Body><tptz:SetHomePositionResponse/></s:Body></s:Envelope>\n");
	return 200;
}