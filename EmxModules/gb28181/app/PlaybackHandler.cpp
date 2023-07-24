#include "PlaybackHandler.h"
#include "GBSdp.h"
#include "SipUAC.h"
#include "PSMuxer.h"
#include "EmxCore.hpp"

PlaybackHandler::PlaybackHandler(const std::string& id) :VideoHandler(id)
{
    emxlogd("construct playbackhandler %p\n", this);
}

PlaybackHandler::~PlaybackHandler()
{
    _quitFlag = 1;
    if (_streamTask)
        _streamTask->join();

    emxlogd("deconstruct playbackhandler %p\n", this);
}

int PlaybackHandler::Init(void* dlg, GBSdp& inSdp, std::string& outSdp) {
    if (inSdp.medias.size() > 1)
        return 1;

    auto mDesc = inSdp.medias[0];
    if (strcasecmp("RTP/AVP", mDesc._protocol.c_str()) == 0) {
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
    m_beginTime = inSdp.start_time_;
    m_endTime = inSdp.end_time_;
    _devDlg = dlg;
    return 0;
}

int PlaybackHandler::Start()
{
    if (!_streamTask)
    {
        _streamTask.reset(new std::thread(&PlaybackHandler::sendStreamTask, this));
    }
    return 0;
}

void PlaybackHandler::sendStreamTask()
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
    /*TODO*/
    /*�ҵ�¼���ļ�*/
    PSMuxer psmuxer(0x1b, 0x90);
//    char* out;
//    uint32_t outsize = 0;
    while (!_quitFlag)
    {
        
        /*��¼���ļ�*/
        /*TODO*/
        break;
    }
}

int PlaybackHandler::Stop()
{
    _quitFlag = 1;
    return 0;
}

int PlaybackHandler::Play(float speed)
{
    return 0;
}

int PlaybackHandler::Pause(float pauseTime)
{
    return 0;
}

int PlaybackHandler::SeekPos(float pos)
{
    return 0;
}

int PlaybackHandler::Teardown()
{
    return 0;
}