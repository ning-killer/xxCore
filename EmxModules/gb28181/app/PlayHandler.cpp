#include "PlayHandler.h"
#include "GBSdp.h"
#include "SipUAC.h"
#include "PSMuxer.h"
#include "EmxCore.hpp"

PlayHandler::PlayHandler(const std::string& id) :VideoHandler(id)
{
    emxlogd("construct playhandler %p\n", this);
}

PlayHandler::~PlayHandler()
{
    _quitFlag = 1;
    if (_streamTask)
        _streamTask->join();

    emxlogd("deconstruct playhandler %p\n", this);
}

int PlayHandler::Init(void* dlg, GBSdp& inSdp, std::string& outSdp) {
    if(inSdp.medias.size() > 1)
        return 1;

    auto mDesc = inSdp.medias[0];
    if(strcasecmp("RTP/AVP", mDesc._protocol.c_str()) == 0) {
        _iTcp = 0;
    }
    else {
        _iTcp = 1;
        inSdp.medias[0].setup = 1;
    }

    inSdp.medias[0]._payloadTypes.clear();
    inSdp.unicast_address_ = SipUAC::Instance()._localIP;
    inSdp.medias[0]._port = 23418;
    inSdp.medias[0].sendonly = 1;
    inSdp.medias[0].recvonly = 0;

    MediaPayloadType mpt(96);
    mpt._encodingName = "PS";
    inSdp.medias[0]._payloadTypes.push_back(mpt);
    outSdp = inSdp.encode();

    emxlogi("%s\n", outSdp.c_str());

    _ssrc = atoi(mDesc.exty.c_str());
    _remoteIp = inSdp.unicast_address_;
    _remotePort = mDesc._port;
    _devDlg = dlg;
    return 0;
}

int PlayHandler::Start() {
    if (!_streamTask)
    {
        _streamTask.reset(new std::thread(&PlayHandler::sendStreamTask, this));
    }
    
    return 0;
}

void PlayHandler::sendStreamTask()
{
    if (ConnectServer())
    {
        auto dlg = (pjsip_dialog*)_devDlg;
        emxlogi("dlg %p call bye\n", dlg);
        SipUAC::Instance().PushAsyncCommand([dlg]()
        {
            SipUAC::Instance().SendCallMessage(dlg, "BYE");
        });
    }

    PSMuxer psmuxer(0x1b, 0x90);//h264 g711a
//    char* out;
//    uint32_t outsize = 0;

    while (!_quitFlag)
    {
        /* 
        * �򿪱�����
        * ��֡I֡
        * ���PS�� ��������
        if (!_sendKeyFrame)
        {
        _sendKeyFrame=1;
        }
        if(video)
        psmuxer.PacketMediaData(data, dataLen, pts*90, false, 0, &out, outsize);
        else if(audio)
        psmuxer.PacketMediaData(data, dataLen, pts*90, false, 1, &out, outsize);
        if (InputPSFrame(out, outsize))
        {
            auto dlg = (pjsip_dialog*)_devDlg;
            SPDLOG_INFO("dlg {} call bye", fmt::ptr(dlg));
            SipUAC::Instance().PushAsyncCommand([dlg]()
            {
                SipUAC::Instance().SendCallMessage(dlg, "BYE");
            });
            break;
        }*/


        /*auto dlg = (pjsip_dialog*)_devDlg;
        SPDLOG_INFO("dlg {} call bye", fmt::ptr(dlg));
        SipUAC::Instance().PushAsyncCommand([dlg]()
        {
            SipUAC::Instance().SendCallMessage(dlg, "BYE");
        });*/
        break;

    }
}

int PlayHandler::Stop() {
    _quitFlag = 1;
    return 0;
}
