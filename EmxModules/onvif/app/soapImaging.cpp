#include "soapImaging.h"

BaseImagingResponse::~BaseImagingResponse() {}
tinyxml2::XMLElement* BaseImagingResponse::InitHeader(const std::string& tag)
{
	auto declare = _doc.NewDeclaration();
	_doc.InsertFirstChild(declare);

	/*Envelope*/
	auto envelopeNode = _doc.NewElement("s:Envelope");
	_doc.InsertEndChild(envelopeNode);
	envelopeNode->SetAttribute("xmlns:s", "http://www.w3.org/2003/05/soap-envelope");
	envelopeNode->SetAttribute("xmlns:sc", "http://www.w3.org/2003/05/soap-encoding");
	envelopeNode->SetAttribute("xmlns:tt", "http://www.onvif.org/ver10/schema");
	envelopeNode->SetAttribute("xmlns:timg", "http://www.onvif.org/ver20/imaging/wsdl");
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
std::string _timg__GetImagingSettingsResponse::Serialization()
{
	auto respNode = InitHeader("timg:GetImagingSettingsResponse");
	auto isNode = _doc.NewElement("timg:ImagingSettings");
	respNode->InsertEndChild(isNode);
	if (ImagingSettings.BacklightCompensation)
	{
		auto blcNode = _doc.NewElement("tt:BacklightCompensation");
		auto modeNode = _doc.NewElement("tt:Mode");
		modeNode->SetText(ImagingSettings.BacklightCompensation->Mode.c_str());
		auto levelNode = _doc.NewElement("tt:Level");
		levelNode->SetText(ImagingSettings.BacklightCompensation->Level);
		blcNode->InsertEndChild(modeNode);
		blcNode->InsertEndChild(levelNode);
		isNode->InsertEndChild(blcNode);
	}
	if (ImagingSettings.Brightness)
	{
		auto brightnessNode = _doc.NewElement("tt:Brightness");
		brightnessNode->SetText(*ImagingSettings.Brightness);
		isNode->InsertEndChild(brightnessNode);
	}
	if (ImagingSettings.ColorSaturation)
	{
		auto csNode = _doc.NewElement("tt:ColorSaturation");
		csNode->SetText(*ImagingSettings.ColorSaturation);
		isNode->InsertEndChild(csNode);
	}
	if (ImagingSettings.Contrast)
	{
		auto contrastNode = _doc.NewElement("tt:Contrast");
		contrastNode->SetText(*ImagingSettings.Contrast);
		isNode->InsertEndChild(contrastNode);
	}
	if (ImagingSettings.Exposure)
	{
		auto exposureNode = _doc.NewElement("tt:Exposure");
		auto modeNode = _doc.NewElement("tt:Mode");
		modeNode->SetText(ImagingSettings.Exposure->Mode.c_str());
		auto priorityNode = _doc.NewElement("tt:Priority");
		switch (ImagingSettings.Exposure->Priority)
		{
		case tt__ExposurePriority__LowNoise:
			priorityNode->SetText("LowNoise");
			break;
		case tt__ExposurePriority__FrameRate:
			priorityNode->SetText("FrameRate");
			break;
		}
		auto wndNode = _doc.NewElement("tt:Window");
		wndNode->SetAttribute("bottom", ImagingSettings.Exposure->Window->bottom);
		wndNode->SetAttribute("top", ImagingSettings.Exposure->Window->top);
		wndNode->SetAttribute("right", ImagingSettings.Exposure->Window->right);
		wndNode->SetAttribute("left", ImagingSettings.Exposure->Window->left);
		exposureNode->InsertEndChild(modeNode);
		exposureNode->InsertEndChild(priorityNode);
		exposureNode->InsertEndChild(wndNode);
		if (ImagingSettings.Exposure->MinExposureTime)
		{
			auto minETNode = _doc.NewElement("tt:MinExposureTime");
			minETNode->SetText(*ImagingSettings.Exposure->MinExposureTime);
			exposureNode->InsertEndChild(minETNode);
		}
		if (ImagingSettings.Exposure->MaxExposureTime)
		{
			auto maxETNode = _doc.NewElement("tt:MaxExposureTime");
			maxETNode->SetText(*ImagingSettings.Exposure->MaxExposureTime);
			exposureNode->InsertEndChild(maxETNode);
		}
		if (ImagingSettings.Exposure->MinGain)
		{
			auto minGainNode = _doc.NewElement("tt:MinGain");
			minGainNode->SetText(*ImagingSettings.Exposure->MinGain);
			exposureNode->InsertEndChild(minGainNode);
		}
		if (ImagingSettings.Exposure->MaxGain)
		{
			auto maxGainNode = _doc.NewElement("tt:MaxGain");
			maxGainNode->SetText(*ImagingSettings.Exposure->MaxGain);
			exposureNode->InsertEndChild(maxGainNode);
		}
		if (ImagingSettings.Exposure->MinIris)
		{
			auto minIrisNode = _doc.NewElement("tt:MinIris");
			minIrisNode->SetText(*ImagingSettings.Exposure->MinIris);
			exposureNode->InsertEndChild(minIrisNode);
		}
		if (ImagingSettings.Exposure->MaxIris)
		{
			auto maxIrisNode = _doc.NewElement("tt:MaxIris");
			maxIrisNode->SetText(*ImagingSettings.Exposure->MaxIris);
			exposureNode->InsertEndChild(maxIrisNode);
		}
		if (ImagingSettings.Exposure->ExposureTime)
		{
			auto etNode = _doc.NewElement("tt:ExposureTime");
			etNode->SetText(*ImagingSettings.Exposure->ExposureTime);
			exposureNode->InsertEndChild(etNode);
		}
		if (ImagingSettings.Exposure->Gain)
		{
			auto gainNode = _doc.NewElement("tt:Gain");
			gainNode->SetText(*ImagingSettings.Exposure->Gain);
			exposureNode->InsertEndChild(gainNode);
		}
		if (ImagingSettings.Exposure->Iris)
		{
			auto irisNode = _doc.NewElement("tt:Iris");
			irisNode->SetText(*ImagingSettings.Exposure->Iris);
			exposureNode->InsertEndChild(irisNode);
		}
		isNode->InsertEndChild(exposureNode);
	}
	if (ImagingSettings.Focus)
	{
		auto focusNode = _doc.NewElement("tt:Focus");
		focusNode->SetAttribute("AFMode", "");
		auto modeNode = _doc.NewElement("tt:AutoFocusMode");
		modeNode->SetText(ImagingSettings.Focus->AutoFocusMode.c_str());
		auto dsNode = _doc.NewElement("tt:DefaultSpeed");
		dsNode->SetText(ImagingSettings.Focus->DefaultSpeed);
		auto nlNode = _doc.NewElement("tt:NearLimit");
		nlNode->SetText(ImagingSettings.Focus->NearLimit);
		auto flNode = _doc.NewElement("tt:FarLimit");
		flNode->SetText(ImagingSettings.Focus->FarLimit);
		focusNode->InsertEndChild(modeNode);
		focusNode->InsertEndChild(dsNode);
		focusNode->InsertEndChild(nlNode);
		focusNode->InsertEndChild(flNode);
		isNode->InsertEndChild(focusNode);
	}
	if (ImagingSettings.IrCutFilter)
	{
		auto icfNode  = _doc.NewElement("tt:IrCutFilter");
		switch (*ImagingSettings.IrCutFilter)
		{
		case tt__IrCutFilterMode__ON:
			icfNode->SetText("ON");
			break;
		case tt__IrCutFilterMode__OFF:
			icfNode->SetText("OFF");
			break;
		case tt__IrCutFilterMode__AUTO:
			icfNode->SetText("AUTO");
			break;
		}
		isNode->InsertEndChild(icfNode);
	}
	if (ImagingSettings.Sharpness)
	{
		auto sharpnessNode = _doc.NewElement("tt:Sharpness");
		sharpnessNode->SetText(*ImagingSettings.Sharpness);
		isNode->InsertEndChild(sharpnessNode);
	}
	if (ImagingSettings.WideDynamicRange)
	{
		auto wdrNode = _doc.NewElement("tt:WideDynamicRange");
		auto modeNode = _doc.NewElement("tt:Mode");
		modeNode->SetText(ImagingSettings.WideDynamicRange->Mode.c_str());
		auto levelNode = _doc.NewElement("tt:Level");
		levelNode->SetText(ImagingSettings.WideDynamicRange->Level);
		wdrNode->InsertEndChild(modeNode);
		wdrNode->InsertEndChild(levelNode);
		isNode->InsertEndChild(wdrNode);
	}
	if (ImagingSettings.WhiteBalance)
	{
		auto wbNode = _doc.NewElement("tt:WhiteBalance");
		auto modeNode = _doc.NewElement("tt:Mode");
		modeNode->SetText(ImagingSettings.WhiteBalance->Mode.c_str());
		wbNode->InsertEndChild(modeNode);
		if (ImagingSettings.WhiteBalance->CrGain)
		{
			auto crGainNode = _doc.NewElement("tt:CrGain");
			crGainNode->SetText(*ImagingSettings.WhiteBalance->CrGain);
			wbNode->InsertEndChild(crGainNode);
		}
		if (ImagingSettings.WhiteBalance->CbGain)
		{
			auto cbGainNode = _doc.NewElement("tt:CbGain");
			cbGainNode->SetText(*ImagingSettings.WhiteBalance->CbGain);
			wbNode->InsertEndChild(cbGainNode);
		}
		isNode->InsertEndChild(wbNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _timg__SetImagingSettingsResponse::Serialization()
{
//	auto respNode = InitHeader("timg:SetImagingSettingsResponse");
	InitHeader("timg:SetImagingSettingsResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}

std::string _timg__MoveResponse::Serialization()
{
//	auto respNode = InitHeader("timg:MoveResponse");
	InitHeader("timg:MoveResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _timg__StopResponse::Serialization()
{
//	auto respNode = InitHeader("timg:StopResponse");
	InitHeader("timg:StopResponse");
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _timg__GetMoveOptionsResponse::Serialization()
{
	auto respNode = InitHeader("timg:GetMoveOptionsResponse");
	auto moveOptionsNode = _doc.NewElement("timg:MoveOptions");
	respNode->InsertEndChild(moveOptionsNode);
	if (MoveOptions.Absolute)
	{
		auto absNode = _doc.NewElement("tt:Absolute");
		if (MoveOptions.Absolute->Position)
		{
			auto positionNode = _doc.NewElement("tt:Position");
			auto pminNode = _doc.NewElement("tt:Min");
			pminNode->SetText(MoveOptions.Absolute->Position->Min);
			auto pmaxNode = _doc.NewElement("tt:Max");
			pmaxNode->SetText(MoveOptions.Absolute->Position->Max);
			positionNode->InsertEndChild(pminNode);
			positionNode->InsertEndChild(pmaxNode);
			absNode->InsertEndChild(positionNode);
		}
		if (MoveOptions.Absolute->Speed)
		{
			auto speedNode = _doc.NewElement("tt:Speed");
			auto sminNode = _doc.NewElement("tt:Min");
			sminNode->SetText(MoveOptions.Absolute->Speed->Min);
			auto smaxNode = _doc.NewElement("tt:Max");
			smaxNode->SetText(MoveOptions.Absolute->Speed->Max);
			speedNode->InsertEndChild(sminNode);
			speedNode->InsertEndChild(smaxNode);
			absNode->InsertEndChild(speedNode);
		}
		moveOptionsNode->InsertEndChild(absNode);
	}
	if (MoveOptions.Relative)
	{
		auto relativeNode = _doc.NewElement("tt:Relative");
		if (MoveOptions.Relative->Distance)
		{
			auto distanceNode = _doc.NewElement("tt:Distance");
			auto dminNode = _doc.NewElement("tt:Min");
			dminNode->SetText(MoveOptions.Relative->Distance->Min);
			auto dmaxNode = _doc.NewElement("tt:Max");
			dmaxNode->SetText(MoveOptions.Relative->Distance->Max);
			distanceNode->InsertEndChild(dminNode);
			distanceNode->InsertEndChild(dminNode);
			relativeNode->InsertEndChild(distanceNode);
		}
		if (MoveOptions.Relative->Speed)
		{
			auto speedNode = _doc.NewElement("tt:Speed");
			auto sminNode = _doc.NewElement("tt:Min");
			sminNode->SetText(MoveOptions.Relative->Speed->Min);
			auto smaxNode = _doc.NewElement("tt:Max");
			smaxNode->SetText(MoveOptions.Relative->Speed->Max);
			speedNode->InsertEndChild(sminNode);
			speedNode->InsertEndChild(smaxNode);
			relativeNode->InsertEndChild(speedNode);
		}
		moveOptionsNode->InsertEndChild(relativeNode);
	}
	if (MoveOptions.Continuous)
	{
		auto continuousNode = _doc.NewElement("tt:Continuous");
		if (MoveOptions.Continuous->Speed)
		{
			auto speedNode = _doc.NewElement("tt:Speed");
			auto minNode = _doc.NewElement("tt:Min");
			minNode->SetText(MoveOptions.Continuous->Speed->Min);
			auto maxNode = _doc.NewElement("tt:Max");
			maxNode->SetText(MoveOptions.Continuous->Speed->Max);
			speedNode->InsertEndChild(minNode);
			speedNode->InsertEndChild(maxNode);
			continuousNode->InsertEndChild(speedNode);
		}
		moveOptionsNode->InsertEndChild(continuousNode);
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);

	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
std::string _timg__GetOptionsResponse::Serialization()
{
	auto respNode = InitHeader("timg:GetOptionsResponse");
	auto imgOptionsNode = _doc.NewElement("timg:ImagingOptions");
	respNode->InsertEndChild(imgOptionsNode);
	if (ImagingOptions.BacklightCompensation)
	{
		auto blcNode = _doc.NewElement("tt:BacklightCompensation");
		for (size_t i = 0; i < ImagingOptions.BacklightCompensation->Mode.size(); i++)
		{
			auto modeNode = _doc.NewElement("tt:Mode");
			modeNode->SetText(ImagingOptions.BacklightCompensation->Mode[i].c_str());
			blcNode->InsertEndChild(modeNode);
		}
		auto levelNode = _doc.NewElement("tt:Level");
		auto minNode = _doc.NewElement("tt:Min");
		minNode->SetText(ImagingOptions.BacklightCompensation->Level->Min);
		auto maxNode = _doc.NewElement("tt:Max");
		maxNode->SetText(ImagingOptions.BacklightCompensation->Level->Max);
		levelNode->InsertEndChild(minNode);
		levelNode->InsertEndChild(maxNode);
		blcNode->InsertEndChild(levelNode);
		imgOptionsNode->InsertEndChild(blcNode);
	}
	if (ImagingOptions.Brightness)
	{
		auto brightnessNode = _doc.NewElement("tt:Brightness");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(ImagingOptions.Brightness->Min);
		maxNode->SetText(ImagingOptions.Brightness->Max);
		brightnessNode->InsertEndChild(minNode);
		brightnessNode->InsertEndChild(maxNode);
		imgOptionsNode->InsertEndChild(brightnessNode);
	}
	if (ImagingOptions.ColorSaturation)
	{
		auto csNode = _doc.NewElement("tt:ColorSaturation");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(ImagingOptions.ColorSaturation->Min);
		maxNode->SetText(ImagingOptions.ColorSaturation->Max);
		csNode->InsertEndChild(minNode);
		csNode->InsertEndChild(maxNode);
		imgOptionsNode->InsertEndChild(csNode);
	}
	if (ImagingOptions.Contrast)
	{
		auto contrastNode = _doc.NewElement("tt:Contrast");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(ImagingOptions.Contrast->Min);
		maxNode->SetText(ImagingOptions.Contrast->Max);
		contrastNode->InsertEndChild(minNode);
		contrastNode->InsertEndChild(maxNode);
		imgOptionsNode->InsertEndChild(contrastNode);
	}
	if (ImagingOptions.Exposure)
	{
		auto exposureNode = _doc.NewElement("tt:Exposure");
		imgOptionsNode->InsertEndChild(exposureNode);
		for (size_t i = 0; i < ImagingOptions.Exposure->Mode.size(); i++)
		{
			auto modeNode = _doc.NewElement("tt:Mode");
			modeNode->SetText(ImagingOptions.Exposure->Mode[i].c_str());
			exposureNode->InsertEndChild(modeNode);
		}
		for (size_t i = 0; i < ImagingOptions.Exposure->Priority.size(); i++)
		{
			auto priorityNode = _doc.NewElement("tt:Priority");
			switch (ImagingOptions.Exposure->Priority[i])
			{
			case tt__ExposurePriority__LowNoise:
				priorityNode->SetText("LowNoise");
				break;
			case tt__ExposurePriority__FrameRate:
				priorityNode->SetText("FrameRate");
				break;
			}
			exposureNode->InsertEndChild(priorityNode);
		}
		if (ImagingOptions.Exposure->MinExposureTime)
		{
			auto metNode = _doc.NewElement("tt:MinExposureTime");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MinExposureTime->Min);
			maxNode->SetText(ImagingOptions.Exposure->MinExposureTime->Max);
			metNode->InsertEndChild(minNode);
			metNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(metNode);
		}
		if (ImagingOptions.Exposure->MaxExposureTime)
		{
			auto metNode = _doc.NewElement("tt:MaxExposureTime");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MaxExposureTime->Min);
			maxNode->SetText(ImagingOptions.Exposure->MaxExposureTime->Max);
			metNode->InsertEndChild(minNode);
			metNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(metNode);
		}
		if(ImagingOptions.Exposure->MinGain)
		{
			auto mgNode = _doc.NewElement("tt:MinGain");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MinGain->Min);
			maxNode->SetText(ImagingOptions.Exposure->MinGain->Max);
			mgNode->InsertEndChild(minNode);
			mgNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(mgNode);
		}
		if (ImagingOptions.Exposure->MaxGain)
		{
			auto mgNode = _doc.NewElement("tt:MaxGain");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MinGain->Min);
			maxNode->SetText(ImagingOptions.Exposure->MinGain->Max);
			mgNode->InsertEndChild(minNode);
			mgNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(mgNode);
		}
		if (ImagingOptions.Exposure->MinIris)
		{
			auto miNode = _doc.NewElement("tt:MinIris");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MinIris->Min);
			maxNode->SetText(ImagingOptions.Exposure->MinIris->Max);
			miNode->InsertEndChild(minNode);
			miNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(miNode);
		}
		if (ImagingOptions.Exposure->MaxIris)
		{
			auto miNode = _doc.NewElement("tt:MaxIris");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->MaxIris->Min);
			maxNode->SetText(ImagingOptions.Exposure->MaxIris->Max);
			miNode->InsertEndChild(minNode);
			miNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(miNode);
		}
		if (ImagingOptions.Exposure->ExposureTime)
		{
			auto etNode = _doc.NewElement("tt:ExposureTime");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->ExposureTime->Min);
			maxNode->SetText(ImagingOptions.Exposure->ExposureTime->Max);
			etNode->InsertEndChild(minNode);
			etNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(etNode);
		}
		if (ImagingOptions.Exposure->Gain)
		{
			auto gainNode = _doc.NewElement("tt:Gain");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->Gain->Min);
			maxNode->SetText(ImagingOptions.Exposure->Gain->Max);
			gainNode->InsertEndChild(minNode);
			gainNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(gainNode);
		}
		if (ImagingOptions.Exposure->Iris)
		{
			auto irisNode = _doc.NewElement("tt:Iris");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Exposure->Iris->Min);
			maxNode->SetText(ImagingOptions.Exposure->Iris->Max);
			irisNode->InsertEndChild(minNode);
			irisNode->InsertEndChild(maxNode);
			exposureNode->InsertEndChild(irisNode);
		}
	}
	if (ImagingOptions.Focus)
	{
		auto focusNode = _doc.NewElement("tt:Focus");
		imgOptionsNode->InsertEndChild(focusNode);
		for (size_t i = 0; i < ImagingOptions.Focus->AutoFocusModes.size(); i++)
		{
			auto modeNode = _doc.NewElement("tt:AutoFocusModes");
			modeNode->SetText(ImagingOptions.Focus->AutoFocusModes[i].c_str());
			focusNode->InsertEndChild(modeNode);
		}
		if (ImagingOptions.Focus->DefaultSpeed)
		{
			auto dsNode = _doc.NewElement("tt:DefaultSpeed");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Focus->DefaultSpeed->Min);
			maxNode->SetText(ImagingOptions.Focus->DefaultSpeed->Max);
			dsNode->InsertEndChild(minNode);
			dsNode->InsertEndChild(maxNode);
			focusNode->InsertEndChild(dsNode);
		}
		if (ImagingOptions.Focus->NearLimit)
		{
			auto nlNode = _doc.NewElement("tt:NearLimit");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Focus->NearLimit->Min);
			maxNode->SetText(ImagingOptions.Focus->NearLimit->Max);
			nlNode->InsertEndChild(minNode);
			nlNode->InsertEndChild(maxNode);
			focusNode->InsertEndChild(nlNode);
		}
		if (ImagingOptions.Focus->FarLimit)
		{
			auto flNode = _doc.NewElement("tt:FarLimit");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.Focus->FarLimit->Min);
			maxNode->SetText(ImagingOptions.Focus->FarLimit->Max);
			flNode->InsertEndChild(minNode);
			flNode->InsertEndChild(maxNode);
			focusNode->InsertEndChild(flNode);
		}
	}
	for (size_t i = 0; i < ImagingOptions.IrCutFilterModes.size(); i++)
	{
		auto icfNode = _doc.NewElement("tt:IrCutFilterModes");
		switch (ImagingOptions.IrCutFilterModes[i])
		{
		case tt__IrCutFilterMode__ON:
			icfNode->SetText("ON");
			break;
		case tt__IrCutFilterMode__OFF:
			icfNode->SetText("OFF");
			break;
		case tt__IrCutFilterMode__AUTO:
			icfNode->SetText("AUTO");
			break;
		}
		imgOptionsNode->InsertEndChild(icfNode);
	}

	if (ImagingOptions.Sharpness)
	{
		auto sharpnessNode = _doc.NewElement("tt:Sharpness");
		auto minNode = _doc.NewElement("tt:Min");
		auto maxNode = _doc.NewElement("tt:Max");
		minNode->SetText(ImagingOptions.Sharpness->Min);
		maxNode->SetText(ImagingOptions.Sharpness->Max);
		sharpnessNode->InsertEndChild(minNode);
		sharpnessNode->InsertEndChild(maxNode);
		imgOptionsNode->InsertEndChild(sharpnessNode);
	}
	if (ImagingOptions.WideDynamicRange)
	{
		auto wdrNode = _doc.NewElement("tt:WideDynamicRange");
		imgOptionsNode->InsertEndChild(wdrNode);
		for (size_t i = 0; i < ImagingOptions.WideDynamicRange->Mode.size(); i++)
		{
			auto modeNode = _doc.NewElement("tt:Mode");
			modeNode->SetText(ImagingOptions.WideDynamicRange->Mode[i].c_str());
			wdrNode->InsertEndChild(modeNode);
		}
		if (ImagingOptions.WideDynamicRange->Level)
		{
			auto levelNode = _doc.NewElement("tt:Level");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.WideDynamicRange->Level->Min);
			maxNode->SetText(ImagingOptions.WideDynamicRange->Level->Max);
			levelNode->InsertEndChild(minNode);
			levelNode->InsertEndChild(maxNode);
			wdrNode->InsertEndChild(levelNode);
		}
	}
	if (ImagingOptions.WhiteBalance)
	{
		auto wbNode = _doc.NewElement("tt:WhiteBalance");
		imgOptionsNode->InsertEndChild(wbNode);
		for (size_t i = 0; i < ImagingOptions.WhiteBalance->Mode.size(); i++)
		{
			auto modeNode = _doc.NewElement("tt:Mode");
			modeNode->SetText(ImagingOptions.WhiteBalance->Mode[i].c_str());
			wbNode->InsertEndChild(modeNode);
		}
		if (ImagingOptions.WhiteBalance->YrGain)
		{
			auto yrGainNode = _doc.NewElement("tt:YrGain");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.WhiteBalance->YrGain->Min);
			maxNode->SetText(ImagingOptions.WhiteBalance->YrGain->Max);
			yrGainNode->InsertEndChild(minNode);
			yrGainNode->InsertEndChild(maxNode);
			wbNode->InsertEndChild(yrGainNode);
		}
		if (ImagingOptions.WhiteBalance->YbGain)
		{
			auto ybGainNode = _doc.NewElement("tt:YbGain");
			auto minNode = _doc.NewElement("tt:Min");
			auto maxNode = _doc.NewElement("tt:Max");
			minNode->SetText(ImagingOptions.WhiteBalance->YbGain->Min);
			maxNode->SetText(ImagingOptions.WhiteBalance->YbGain->Max);
			ybGainNode->InsertEndChild(minNode);
			ybGainNode->InsertEndChild(maxNode);
			wbNode->InsertEndChild(ybGainNode);
		}
	}
	tinyxml2::XMLPrinter printer(0, true);
	_doc.Print(&printer);
	return std::string(printer.CStr(), printer.CStrSize() - 1);
}
int soapImaging::OnGetImagingSettings(void* req, std::string& body)
{
	_timg__GetImagingSettingsResponse resp;
	resp.ImagingSettings.BacklightCompensation = std::make_shared<tt__BacklightCompensation20>();
	resp.ImagingSettings.BacklightCompensation->Mode = "ON";
	resp.ImagingSettings.BacklightCompensation->Level = 1.0f;
	resp.ImagingSettings.Brightness = std::make_shared<float>(50.f);
	resp.ImagingSettings.ColorSaturation = std::make_shared<float>(50.f);
	resp.ImagingSettings.Contrast = std::make_shared<float>(50.f);
	resp.ImagingSettings.Exposure = std::make_shared<tt__Exposure20>();
	resp.ImagingSettings.Exposure->Mode = "AUTO";
	resp.ImagingSettings.Exposure->Priority = tt__ExposurePriority__FrameRate;
	resp.ImagingSettings.Exposure->Window = std::make_shared<tt__Rectangle>();
	resp.ImagingSettings.Exposure->Window->left = 0;
	resp.ImagingSettings.Exposure->Window->top = 0;
	resp.ImagingSettings.Exposure->Window->bottom = 0;
	resp.ImagingSettings.Exposure->Window->right = 0;
	resp.ImagingSettings.Exposure->MinExposureTime = std::make_shared<float>(15.f);
	resp.ImagingSettings.Exposure->MaxExposureTime = std::make_shared<float>(25000.f);
	resp.ImagingSettings.Exposure->MinGain = std::make_shared<float>(5.f);
	resp.ImagingSettings.Exposure->MaxGain = std::make_shared<float>(64.f);
	//resp.ImagingSettings.Exposure->MinIris = 0.f;
	//resp.ImagingSettings.Exposure->MaxIris = 10.f;
	//resp.ImagingSettings.Exposure->ExposureTime = 1.5f;
	//resp.ImagingSettings.Exposure->Gain = 1.7f;
	//resp.ImagingSettings.Exposure->Iris = 1.6f;
	resp.ImagingSettings.IrCutFilter = std::make_shared<enum tt__IrCutFilterMode>(tt__IrCutFilterMode__AUTO);
	resp.ImagingSettings.WhiteBalance = std::make_shared<tt__WhiteBalance20>();
	resp.ImagingSettings.WhiteBalance->Mode = "AUTO";
	//resp.ImagingSettings.WhiteBalance->CrGain = 25.f;
	//resp.ImagingSettings.WhiteBalance->CbGain = 65.f;
	resp.ImagingSettings.Sharpness = std::make_shared<float>(50.f);
	body = std::move(resp.Serialization());
	return 200;
}

int soapImaging::OnSetImagingSettings(void* req, std::string& body)
{
	tt__ImagingSettings20 a;
	std::string token;
	tinyxml2::XMLDocument doc;
	doc.Parse(body.c_str(), body.size());
	auto envelopeNode = doc.FirstChildElement();
	if (envelopeNode)
	{
		auto child = envelopeNode->FirstChildElement();//Header
		if (strstr(child->Value(), "Header"))
			child = child->NextSiblingElement();//Body
		auto setNode = child->FirstChildElement();
		if (setNode && strstr(setNode->Value(), "SetImagingSettings"))
		{
			for (auto i = setNode->FirstChildElement(); i; i = i->NextSiblingElement())
			{
				if (strstr(i->Value(), "VideoSourceToken"))
					token = i->GetText();
				else if (strstr(i->Value(), "ImagingSettings"))
				{
					for (auto j = i->FirstChildElement(); j; j = j->NextSiblingElement())
					{
						if (strstr(j->Value(), "BacklightCompensation"))
						{
							a.BacklightCompensation = std::make_shared<tt__BacklightCompensation20>();
							for (auto k = j->FirstChildElement(); k; k = j->NextSiblingElement())
							{
								if (strstr(k->Value(), "Mode"))
									a.BacklightCompensation->Mode = k->GetText();
								else if (strstr(k->Value(), "Level"))
									a.BacklightCompensation->Level = k->FloatText();
							}
						}
						else if (strstr(j->Value(), "Brightness"))
							a.Brightness = std::make_shared<float>(j->FloatText());
						else if (strstr(j->Value(), "ColorSaturation"))
							a.ColorSaturation = std::make_shared<float>(j->FloatText());
						else if (strstr(j->Value(), "Contrast"))
							a.Contrast = std::make_shared<float>(j->FloatText());
						else if (strstr(j->Value(), "Exposure"))
						{
							a.Exposure = std::make_shared<tt__Exposure20>();
							for (auto k = j->FirstChildElement(); k; k = k->NextSiblingElement())
							{
								if (strstr(k->Value(), "Mode"))
									a.Exposure->Mode = k->GetText();
								else if (strstr(k->Value(), "Priority"))
									a.Exposure->Priority = (tt__ExposurePriority)k->IntText();
								else if (strstr(k->Value(), "MinExposureTime"))
									a.Exposure->MinExposureTime = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "MaxExposureTime"))
									a.Exposure->MaxExposureTime = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "MinGain"))
									a.Exposure->MinGain = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "MaxGain"))
									a.Exposure->MaxGain = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "MinIris"))
									a.Exposure->MinIris = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "MaxIris"))
									a.Exposure->MaxIris = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "ExposureTime"))
									a.Exposure->ExposureTime = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "Gain"))
									a.Exposure->Gain = std::make_shared <float>(k->FloatText());
								else if (strstr(k->Value(), "Iris"))
									a.Exposure->Iris = std::make_shared <float>(k->FloatText());
							}
						}
						else if (strstr(j->Value(), "IrCutFilter"))
							a.IrCutFilter = std::make_shared<tt__IrCutFilterMode>((tt__IrCutFilterMode)j->IntText());
						else if (strstr(j->Value(), "Sharpness"))
							a.Sharpness = std::make_shared<float>(j->FloatText());
						else if (strstr(j->Value(), "WhiteBalance"))
						{
							a.WhiteBalance = std::make_shared<tt__WhiteBalance20>();
							for (auto k = j->FirstChildElement(); k; k = j->NextSiblingElement())
							{
								if (strstr(k->Value(), "Mode"))
									a.WhiteBalance->Mode = k->GetText();
								else if (strstr(k->Value(), "CrGain"))
									a.WhiteBalance->CrGain = std::make_shared<float>(k->FloatText());
								else if(strstr(k->Value(), "CbGain"))
									a.WhiteBalance->CbGain = std::make_shared<float>(k->FloatText());
							}
						}
					}
				}
			}
		}
	}

	_timg__SetImagingSettingsResponse resp;
	body = std::move(resp.Serialization());
	return 200;
}

int soapImaging::OnMove(void* req, std::string& body)
{
	_timg__MoveResponse resp;
	body = std::move(resp.Serialization());
	return 200;
}

int soapImaging::OnStop(void* req, std::string& body)
{
	_timg__StopResponse resp;
	body = std::move(resp.Serialization());
	return 200;
}
int soapImaging::OnGetMoveOptions(void* req, std::string& body)
{
	_timg__GetMoveOptionsResponse resp;
	resp.MoveOptions.Absolute = std::make_shared<tt__AbsoluteFocusOptions>(); 
	resp.MoveOptions.Absolute->Position = std::make_shared<tt__FloatRange>();
	resp.MoveOptions.Absolute->Position->Min = 0.f;
	resp.MoveOptions.Absolute->Position->Max = 1.f;

	resp.MoveOptions.Relative = std::make_shared<tt__RelativeFocusOptions20>();
	resp.MoveOptions.Relative->Distance = std::make_shared<tt__FloatRange>();
	resp.MoveOptions.Relative->Distance->Min = -1.f;
	resp.MoveOptions.Relative->Distance->Max = 1.f;
	body = std::move(resp.Serialization());
	return 200;
}

int soapImaging::OnGetOptions(void* req, std::string& body)
{
	_timg__GetOptionsResponse resp;
	resp.ImagingOptions.BacklightCompensation = std::make_shared<tt__BacklightCompensationOptions20>();
	resp.ImagingOptions.BacklightCompensation->Mode.emplace_back("OFF");
	resp.ImagingOptions.BacklightCompensation->Mode.emplace_back("ON");
	resp.ImagingOptions.BacklightCompensation->Level = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.BacklightCompensation->Level->Min = 0.f;
	resp.ImagingOptions.BacklightCompensation->Level->Max = 5.f;

	resp.ImagingOptions.Brightness = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Brightness->Min = 0.f;
	resp.ImagingOptions.Brightness->Max = 100.f;

	resp.ImagingOptions.ColorSaturation = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.ColorSaturation->Min = 0.f;
	resp.ImagingOptions.ColorSaturation->Max = 100.f;
	resp.ImagingOptions.Contrast = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Contrast->Min = 0.f;
	resp.ImagingOptions.Contrast->Max = 100.f;

	resp.ImagingOptions.Exposure = std::make_shared<tt__ExposureOptions20>();
	resp.ImagingOptions.Exposure->Mode.emplace_back("AUTO");
	resp.ImagingOptions.Exposure->Mode.emplace_back("MANUAL");
	resp.ImagingOptions.Exposure->Priority.emplace_back(tt__ExposurePriority__LowNoise);
	resp.ImagingOptions.Exposure->Priority.emplace_back(tt__ExposurePriority__FrameRate);
	resp.ImagingOptions.Exposure->MinExposureTime = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->MinExposureTime->Min = 10.f;
	resp.ImagingOptions.Exposure->MinExposureTime->Max = 40000.f;
	resp.ImagingOptions.Exposure->MaxExposureTime = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->MaxExposureTime->Min = 10.f;
	resp.ImagingOptions.Exposure->MaxExposureTime->Max = 40000.f;
	resp.ImagingOptions.Exposure->MinGain = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->MinGain->Min = 0.f;
	resp.ImagingOptions.Exposure->MinGain->Max = 100.f;
	resp.ImagingOptions.Exposure->MaxGain = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->MaxGain->Min = 0.f;
	resp.ImagingOptions.Exposure->MaxGain->Max = 100.f;
	resp.ImagingOptions.Exposure->ExposureTime = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->ExposureTime->Min = 10.f;
	resp.ImagingOptions.Exposure->ExposureTime->Max = 40000.f;
	resp.ImagingOptions.Exposure->Gain = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Exposure->Gain->Min = 0.f;
	resp.ImagingOptions.Exposure->Gain->Max = 100.f;
	//resp.ImagingOptions.Exposure->Iris = std::make_shared<tt__FloatRange>();
	//resp.ImagingOptions.Exposure->Iris->Min = 0.f;
	//resp.ImagingOptions.Exposure->Iris->Max = 10.f;

	resp.ImagingOptions.IrCutFilterModes.emplace_back(tt__IrCutFilterMode__ON);
	resp.ImagingOptions.IrCutFilterModes.emplace_back(tt__IrCutFilterMode__OFF);
	resp.ImagingOptions.IrCutFilterModes.emplace_back(tt__IrCutFilterMode__AUTO);
	
	resp.ImagingOptions.WhiteBalance = std::make_shared<tt__WhiteBalanceOptions20>();
	resp.ImagingOptions.WhiteBalance->Mode.emplace_back("AUTO");
	resp.ImagingOptions.WhiteBalance->Mode.emplace_back("MANUAL");
	resp.ImagingOptions.WhiteBalance->YrGain = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.WhiteBalance->YrGain->Min = -1270.f;
	resp.ImagingOptions.WhiteBalance->YrGain->Max = 127.0f;
	resp.ImagingOptions.WhiteBalance->YbGain = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.WhiteBalance->YbGain->Min = -127.f;
	resp.ImagingOptions.WhiteBalance->YbGain->Max = 127.f;
	resp.ImagingOptions.Sharpness = std::make_shared<tt__FloatRange>();
	resp.ImagingOptions.Sharpness->Min = 0.f;
	resp.ImagingOptions.Sharpness->Max = 100.f;
	body = std::move(resp.Serialization());
	return 200;
}
