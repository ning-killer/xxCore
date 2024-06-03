/*
 *  Third party audio device
 */
#include <iostream>
#include "VoipBase.hpp"
#include "third_party_device.h"
#include "sleep.h"
#include "thread_wrapper.h"
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "VoipDefine.hpp"
//#pragma GCC diagnostic ignored "-Wpermissive"
extern AudioParam gAudioParam;
using namespace Emx;

//#define CMCC_PCM_FILE  //pcm数据抓取

namespace cmcc_webrtc {

// #define _DEBUG_

Rectifier::Rectifier()
    : _beginningTimePointMs(KBeginningTimePointMsNone), _calledCnt(0) {}

// 获取当次应取样次数(考虑此前时间误差导致的取样偏少情况)
int64_t Rectifier::RestCntForNow(uint64_t currentTime) {

    // 第一次取样，设置统计时间起点为一个周期之前时刻
    if (_beginningTimePointMs == KBeginningTimePointMsNone) {
        _beginningTimePointMs = currentTime - KIntervalMs * 1;

        return 1;
    }

    // 期望累积取样次数：累积时间/取样间隔
    // 当次应取样次数 = 期望累积取样次数 - 已取样次数
    int64_t restCnt =
        (currentTime - _beginningTimePointMs) / KIntervalMs - _calledCnt;

    // 重置累计统计值和统计时间起点（以当前时刻为新基点），下次计算应取样次数时生效
    // 该重置是为了避免系统时钟误差累积；若系统时钟误差很小，可调大参数
    // KMaxAccumulatedTimeMs
    if (currentTime - _beginningTimePointMs > KMaxAccumulatedTimeMs) {
        _beginningTimePointMs = currentTime;
        _calledCnt = 0;
    }

    return restCnt;
}

// 递增已取样次数
void Rectifier::AddToStatistics() { _calledCnt++; }

// 清除统计信息
void Rectifier::ClearStatistics() {
    _beginningTimePointMs = KBeginningTimePointMsNone;
    _calledCnt = 0;
}


ThirdpartyAudioDevice::ThirdpartyAudioDevice(const int32_t id,
                                 const char* inputFilename,
                                 const char* outputFile):
    _ptrAudioBuffer(NULL),
    _recordingBuffer(NULL),
    _playoutBuffer(NULL),
    _recordingFramesLeft(0),
    _playoutFramesLeft(0),
    _critSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _recordingBufferSizeIn10MS(0),
    _recordingFramesIn10MS(0),
    _playoutFramesIn10MS(0),
    _ptrThreadRec(NULL),
    _ptrThreadPlay(NULL),
    _recThreadID(0),
    _playThreadID(0),
    _playing(false),
    _recording(false),
    _lastCallPlayoutMillis(0),
    _lastCallRecordMillis(0),
    _outputFilename(outputFile),
    _inputFilename(inputFilename),
    _clock(Clock::GetRealTimeClock()) {
#ifdef CMCC_PCM_FILE
    _outputFile = *FileWrapper::Create();
    _inputFile = *FileWrapper::Create();
#endif
    _RecAudio_index = 0;
    _RecAudio_size = 0;
    _stream = new MediaClientStreamSync;
    emxlogi("ThirdpartyAudioDevice(构造函数)\n");
}

ThirdpartyAudioDevice::~ThirdpartyAudioDevice() {
#ifdef CMCC_PCM_FILE
  _outputFile.Flush();
  _outputFile.CloseFile();
  delete &_outputFile;
  _inputFile.Flush();
  _inputFile.CloseFile();
  delete &_inputFile;
#endif
  delete &_critSect;
   delete _stream;
    emxlogi("~ThirdpartyAudioDevice(析构函数)\n");
}

int32_t ThirdpartyAudioDevice::ActiveAudioLayer(
    AudioDeviceModule::AudioLayer& audioLayer) const {
      audioLayer = AudioDeviceModule::kLinuxThirdpartyAudio;
  return 0;
}

int32_t ThirdpartyAudioDevice::Init() {
    emxlogi("ThirdpartyAudioDevice::Init()\n");
    //Init Thirdparty device
    return 0;
}

int32_t ThirdpartyAudioDevice::Terminate() {
    emxlogi("ThirdpartyAudioDevice::Terminate()\n");

    CriticalSectionScoped lock(&_critSect);

    // RECORDING
    if (_ptrThreadRec)
    {
        ThreadWrapper* tmpThread = _ptrThreadRec;
        _ptrThreadRec = NULL;
        _critSect.Leave();

        tmpThread->SetNotAlive();

        if (tmpThread->Stop())
        {
            delete tmpThread;
        }
        else
        {
            emxlogi("ThirdpartyAudioDevice::Terminate()  failed to close down the rec audio thread\n");
        }

        _critSect.Enter();
    }

    // PLAYOUT
    if (_ptrThreadPlay)
    {
        ThreadWrapper* tmpThread = _ptrThreadPlay;
        _ptrThreadPlay = NULL;
        _critSect.Leave();

        tmpThread->SetNotAlive();

        if (tmpThread->Stop())
        {
            delete tmpThread;
        }
        else
        {
            emxlogi("ThirdpartyAudioDevice::Terminate()  failed to close down the play audio thread");
        }

        _critSect.Enter();
    }


   return 0;
   }

bool ThirdpartyAudioDevice::Initialized() const
{
  return true;
}

int16_t ThirdpartyAudioDevice::PlayoutDevices() {
  return 1;
}

int16_t ThirdpartyAudioDevice::RecordingDevices() {
  return 1;
}

int32_t ThirdpartyAudioDevice::PlayoutDeviceName(uint16_t index,
                                            char name[kAdmMaxDeviceNameSize],
                                            char guid[kAdmMaxGuidSize]) {
  const char* kName = "thirdparty_device";
  const char* kGuid = "thirdparty_device_unique_id";
  if (index < 1) {
    memset(name, 0, kAdmMaxDeviceNameSize);
    memset(guid, 0, kAdmMaxGuidSize);
    memcpy(name, kName, strlen(kName));
    memcpy(guid, kGuid, strlen(guid));
    return 0;
  }
  return -1;
}

int32_t ThirdpartyAudioDevice::RecordingDeviceName(uint16_t index,
                                              char name[kAdmMaxDeviceNameSize],
                                              char guid[kAdmMaxGuidSize]) {
  const char* kName = "thirdparty_device";
  const char* kGuid = "thirdparty_device_unique_id";
  if (index < 1) {
    memset(name, 0, kAdmMaxDeviceNameSize);
    memset(guid, 0, kAdmMaxGuidSize);
    memcpy(name, kName, strlen(kName));
    memcpy(guid, kGuid, strlen(guid));
    return 0;
  }
  return -1;
}

int32_t ThirdpartyAudioDevice::SetPlayoutDevice(uint16_t index) {
  if (index == 0) {
    _playout_index = index;
    return 0;
  }
  return -1;
}

int32_t ThirdpartyAudioDevice::SetPlayoutDevice(
    AudioDeviceModule::WindowsDeviceType device) {
  return -1;
}

int32_t ThirdpartyAudioDevice::SetRecordingDevice(uint16_t index) {
  if (index == 0) {
    _record_index = index;
    return _record_index;
  }
  return -1;
}

int32_t ThirdpartyAudioDevice::SetRecordingDevice(
    AudioDeviceModule::WindowsDeviceType device) {
  return -1;
}

int32_t ThirdpartyAudioDevice::PlayoutIsAvailable(bool& available) {
  if (_playout_index == 0) {
    available = true;
    return _playout_index;
  }
  available = false;
  return -1;
}

int32_t ThirdpartyAudioDevice::InitPlayout() {
  emxlogi("ThirdpartyAudioDevice::InitPlayout()\n");
  if (_playing) {
    return -1;
  }

  // Initialize the speaker (devices might have been added or removed)
  if (InitSpeaker() == -1) {
    emxlogi("ThirdpartyAudioDevice  InitSpeaker() failed\n");
  }
  if (_ptrAudioBuffer) {
    // Update webrtc audio buffer with the selected parameters
    _ptrAudioBuffer->SetPlayoutSampleRate(gAudioParam.sampleRate);
    _ptrAudioBuffer->SetPlayoutChannels(gAudioParam.chnNum);
  }
  return 0;
}

bool ThirdpartyAudioDevice::PlayoutIsInitialized() const {
  return true;
}

int32_t ThirdpartyAudioDevice::RecordingIsAvailable(bool& available) {
  if (_record_index == 0) {
    available = true;
    return _record_index;
  }
  available = false;
  return -1;
}

int32_t ThirdpartyAudioDevice::InitRecording() {
  CriticalSectionScoped lock(&_critSect);
  if (_recording) {
    return -1;
  }
    
  _recordingFramesIn10MS = gAudioParam.sampleRate / 100;

  if (_ptrAudioBuffer) {
    _ptrAudioBuffer->SetRecordingSampleRate(gAudioParam.sampleRate);
    _ptrAudioBuffer->SetRecordingChannels(gAudioParam.chnNum);
  }
  return 0;
}

bool ThirdpartyAudioDevice::RecordingIsInitialized() const {
  return true;
}

int32_t ThirdpartyAudioDevice::StartPlayout() {
    emxlogi("ThirdpartyAudioDevice::StartPlayout()\n");

//    cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
//    set_audio_play_status(1,1);
//    if (voip_info_t->talkAudioPlay_interrupt) voip_info_t->talkAudioPlay_interrupt();
    MediaClientAdecStreamSync::PlayCtrlQuit(0);
    if (_playing)
    {
      return 0;
    }
  _playing = true;
  _playoutFramesLeft = 0;
  _playoutFramesIn10MS = gAudioParam.sampleRate/100;

  _playingRectifier.ClearStatistics(); // 播放累积统计重置

  if (!_playoutBuffer)
      _playoutBuffer = new int8_t[2 * gAudioParam.chnNum * gAudioParam.sampleRate/100];

  if (!_playoutBuffer)
  {
    _playing = false;
    return -1;
  }

  // PLAYOUT
  const char* threadName = "webrtc_audio_module_play_thread";
    emxlogi("ThirdpartyAudioDevice CreateThread(PlayThreadFunc)\n!");
  _ptrThreadPlay =  ThreadWrapper::CreateThread(PlayThreadFunc,
                                                this,
                                                kRealtimePriority,
                                                threadName);
  if (_ptrThreadPlay == NULL)
  {
      emxlogi("ThirdpartyAudioDevice::StartPlayout() CreateThread(PlayThreadFunc is NULL)\n");
      _playing = false;
      delete [] _playoutBuffer;
      _playoutBuffer = NULL;
      return -1;
  }
#ifdef CMCC_PCM_FILE
  if (_outputFile.OpenFile(_outputFilename.c_str(),
                           false, false, false) == -1) {
      emxlogi("ThirdpartyAudioDevice Failed to open playout file %s!", _outputFilename.c_str());
    _playing = false;
    delete [] _playoutBuffer;
    _playoutBuffer = NULL;
    return -1;
  }
#endif
  unsigned int threadID(0);
  if (!_ptrThreadPlay->Start(threadID))
  {
      _playing = false;
      delete _ptrThreadPlay;
      _ptrThreadPlay = NULL;
      delete [] _playoutBuffer;
      _playoutBuffer = NULL;
      return -1;
  }
  _playThreadID = threadID;

  return 0;
}

int32_t ThirdpartyAudioDevice::StopPlayout() {
  {
      CriticalSectionScoped lock(&_critSect);
      _playing = false;
  }
  //set_audio_play_status(1,0);
  // stop playout thread first
  if (_ptrThreadPlay && !_ptrThreadPlay->Stop())
  {
      return -1;
  }
  else {
      delete _ptrThreadPlay;
      _ptrThreadPlay = NULL;
  }

  CriticalSectionScoped lock(&_critSect);

  _playoutFramesLeft = 0;
  delete [] _playoutBuffer;
  _playoutBuffer = NULL;
#ifdef CMCC_PCM_FILE
  _outputFile.Flush();
  _outputFile.CloseFile();
#endif
//  cmcc_voip_info_t *voipInfo = get_cmcc_voip_info();
//    if (voipInfo->talkAudioPlay_end) voipInfo->talkAudioPlay_end();
//    voipInfo->audio_play = 0;
   return 0;
}

bool ThirdpartyAudioDevice::Playing() const {
  StartPlayout();//stupid code
  return true;
}

int32_t ThirdpartyAudioDevice::StartRecording() {
  _recording = true;
    emxlogi("ThirdpartyAudioDevice::StartRecording(), prepare to send mic data\n");
    _recordingRectifier.ClearStatistics();

    // cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
    // Make sure we only create the buffer once.
    _recordingBufferSizeIn10MS = _recordingFramesIn10MS *
                                 gAudioParam.chnNum *
                                 2;
    printf("_recordingBuffer size(%d)\n", _recordingBufferSizeIn10MS);
//    if (voip_info_t->talkAudioPlay_interrupt) voip_info_t->talkAudioPlay_interrupt();
    MediaClientAdecStreamSync::PlayCtrlQuit(0);
    emxlogi("[%s:%d] _recordingBufferSizeIn10MS:%d\n", __FUNCTION__ , __LINE__, _recordingBufferSizeIn10MS);
  if (!_recordingBuffer) {
      _recordingBuffer = new int8_t[_recordingBufferSizeIn10MS];
  }
    _RecAudio_index = 0;
    _RecAudio_size = 0;
#if 1
#ifdef CMCC_PCM_FILE
  if (_inputFile.OpenFile(_inputFilename.c_str(), true,
                              true, false) == -1) {
      emxlogi("ThirdpartyAudioDevice Failed to open audio input file %s!\n",
           _inputFilename.c_str());
    _recording = false;
    delete[] _recordingBuffer;
    _recordingBuffer = NULL;
    return -1;
  }
#else
    MediaClientStream::Channel chn;
    chn.Add(MediaFrame::TypeE::Pcm,0);
    if(_stream->Open(chn,3000) != ErrCodeE::Success) {
        emxloge("voice media open failed\n");
        _recording = false;
        delete [] _recordingBuffer;
        _recordingBuffer = NULL;
        return -1;
    }

//    _voiceStreamId = stream_id;
//    emxlogi("[%s:%d] _voiceStreamId:%d\n", __FUNCTION__, __LINE__, _voiceStreamId);
#endif
#endif
  const char* threadName = "webrtc_audio_module_capture_thread";
    emxlogi("ThirdpartyAudioDevice::StartRecording() CreateThread(RecThreadFunc)\n");
  _ptrThreadRec = ThreadWrapper::CreateThread(RecThreadFunc,
                                              this,
                                              kRealtimePriority,
                                              threadName);
  if (_ptrThreadRec == NULL)
  {
      emxlogi("ThirdpartyAudioDevice::StartRecording() CreateThread(RecThreadFunc is NULL)\n");
      _recording = false;
      delete [] _recordingBuffer;
      _recordingBuffer = NULL;
      return -1;
  }

  unsigned int threadID(0);
  if (!_ptrThreadRec->Start(threadID))
  {
      _recording = false;
      delete _ptrThreadRec;
      _ptrThreadRec = NULL;
      delete [] _recordingBuffer;
      _recordingBuffer = NULL;
      return -1;
  }
  _recThreadID = threadID;

  return 0;
}


int32_t ThirdpartyAudioDevice::StopRecording() {
    emxlogi("ThirdpartyAudioDevice::StopRecording() \n");

  {
    CriticalSectionScoped lock(&_critSect);
    _recording = false;
  }
    if(_stream){
        _stream->Close();
    }
//  if (_voiceStreamId >= 0)
//    media_stream_close(_voiceStreamId);

  _voiceStreamId = -1;
  if (_ptrThreadRec && !_ptrThreadRec->Stop())
  {
      return -1;
  }
  else {
      delete _ptrThreadRec;
      _ptrThreadRec = NULL;
  }

  CriticalSectionScoped lock(&_critSect);
  _recordingFramesLeft = 0;
  if (_recordingBuffer)
  {
      delete [] _recordingBuffer;
      _recordingBuffer = NULL;
  }
//    cmcc_voip_info_t *voipInfo = get_cmcc_voip_info();
//    if (voipInfo->talkAudioPlay_end) voipInfo->talkAudioPlay_end();
//    voipInfo->audio_play = 0;
  return 0;
}

bool ThirdpartyAudioDevice::Recording() const {
  return _recording;
}

int32_t ThirdpartyAudioDevice::SetAGC(bool enable) { return -1; }

bool ThirdpartyAudioDevice::AGC() const { return false; }

int32_t ThirdpartyAudioDevice::SetWaveOutVolume(uint16_t volumeLeft,
                                           uint16_t volumeRight) {
  return -1;
}

int32_t ThirdpartyAudioDevice::WaveOutVolume(uint16_t& volumeLeft,
                                        uint16_t& volumeRight) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::InitSpeaker()
{
  int ret = -2;
    emxlogi("ThirdpartyAudioDevice::InitSpeaker()\n");
  CriticalSectionScoped lock(&_critSect);

  if (_playing)
  {
      emxlogi("ThirdpartyAudioDevice::InitSpeaker _playing is true, return -1\n");
    return -1;
  }

  //ret =_mixerManager.OpenSpeaker(devName);
  ret = 1;

  return ret;
}

bool ThirdpartyAudioDevice::SpeakerIsInitialized() const
{
    emxlogi("ThirdpartyAudioDevice::SpeakerIsInitialized()\n");
  return true;
}

int32_t ThirdpartyAudioDevice::InitMicrophone()
{
    emxlogi("ThirdpartyAudioDevice::InitMicrophone()\n");
  return 0;
}

bool ThirdpartyAudioDevice::MicrophoneIsInitialized() const { return true; }

int32_t ThirdpartyAudioDevice::SpeakerVolumeIsAvailable(bool& available) {
  return -1;
}

int32_t ThirdpartyAudioDevice::SetSpeakerVolume(uint32_t volume) { return -1; }

int32_t ThirdpartyAudioDevice::SpeakerVolume(uint32_t& volume) const { return -1; }

int32_t ThirdpartyAudioDevice::MaxSpeakerVolume(uint32_t& maxVolume) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::MinSpeakerVolume(uint32_t& minVolume) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::SpeakerVolumeStepSize(uint16_t& stepSize) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::MicrophoneVolumeIsAvailable(bool& available) {
  return -1;
}

int32_t ThirdpartyAudioDevice::SetMicrophoneVolume(uint32_t volume) { return -1; }

int32_t ThirdpartyAudioDevice::MicrophoneVolume(uint32_t& volume) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::MaxMicrophoneVolume(uint32_t& maxVolume) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::MinMicrophoneVolume(uint32_t& minVolume) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::MicrophoneVolumeStepSize(uint16_t& stepSize) const {
  return -1;
}

int32_t ThirdpartyAudioDevice::SpeakerMuteIsAvailable(bool& available) { return -1; }

int32_t ThirdpartyAudioDevice::SetSpeakerMute(bool enable) { return -1; }

int32_t ThirdpartyAudioDevice::SpeakerMute(bool& enabled) const { return -1; }

int32_t ThirdpartyAudioDevice::MicrophoneMuteIsAvailable(bool& available) {
  return -1;
}

int32_t ThirdpartyAudioDevice::SetMicrophoneMute(bool enable) { return -1; }

int32_t ThirdpartyAudioDevice::MicrophoneMute(bool& enabled) const { return -1; }

int32_t ThirdpartyAudioDevice::MicrophoneBoostIsAvailable(bool& available) {
  return -1;
}

int32_t ThirdpartyAudioDevice::SetMicrophoneBoost(bool enable) { return -1; }

int32_t ThirdpartyAudioDevice::MicrophoneBoost(bool& enabled) const { return -1; }

int32_t ThirdpartyAudioDevice::StereoPlayoutIsAvailable(bool& available) {

    emxlogi("ThirdpartyAudioDevice::StereoPlayoutIsAvailable()\n");
    CriticalSectionScoped lock(&_critSect);

    // Save rec states and the number of rec channels
    bool playing = _playing;

    available = false;


    if (InitPlayout() == 0)
    {
        available = true;
    }

    // Stop/uninitialize recording
    StopPlayout();

    if (playing)
    {
        emxlogi("ThirdpartyAudioDevice::StereoPlayoutIsAvailable(), playing is true, StartPlayout()\n");
        StartPlayout();
    }

    return 0;
}
int32_t ThirdpartyAudioDevice::SetStereoPlayout(bool enable) {
  return 0;
}

int32_t ThirdpartyAudioDevice::StereoPlayout(bool& enabled) const {
  enabled = true;
  return 0;
}

int32_t ThirdpartyAudioDevice::StereoRecordingIsAvailable(bool& available) {
  available = true;
  return 0;
}

int32_t ThirdpartyAudioDevice::SetStereoRecording(bool enable) {
  return 0;
}

int32_t ThirdpartyAudioDevice::StereoRecording(bool& enabled) const {
  enabled = true;
  return 0;
}

int32_t ThirdpartyAudioDevice::SetPlayoutBuffer(
  const AudioDeviceModule::BufferType type,
  uint16_t sizeMS) {
  _playBufType = type;

  return 0;
}

int32_t ThirdpartyAudioDevice::PlayoutBuffer(AudioDeviceModule::BufferType& type,
                                        uint16_t& sizeMS) const {
  type = _playBufType;
  return 0;
}

int32_t ThirdpartyAudioDevice::PlayoutDelay(uint16_t& delayMS) const {
  return 0;
}

int32_t ThirdpartyAudioDevice::RecordingDelay(uint16_t& delayMS) const { return -1; }

int32_t ThirdpartyAudioDevice::CPULoad(uint16_t& load) const { return -1; }

bool ThirdpartyAudioDevice::PlayoutWarning() const { return false; }

bool ThirdpartyAudioDevice::PlayoutError() const { return false; }

bool ThirdpartyAudioDevice::RecordingWarning() const { return false; }

bool ThirdpartyAudioDevice::RecordingError() const { return false; }

void ThirdpartyAudioDevice::ClearPlayoutWarning() {}

void ThirdpartyAudioDevice::ClearPlayoutError() {}

void ThirdpartyAudioDevice::ClearRecordingWarning() {}

void ThirdpartyAudioDevice::ClearRecordingError() {}

void ThirdpartyAudioDevice::AttachAudioBuffer(AudioDeviceBuffer* audioBuffer) {
  CriticalSectionScoped lock(&_critSect);

  _ptrAudioBuffer = audioBuffer;

  // Inform the AudioBuffer about default settings for this implementation.
  // Set all values to zero here since the actual settings will be done by
  // InitPlayout and InitRecording later.
  _ptrAudioBuffer->SetRecordingSampleRate(0);
  _ptrAudioBuffer->SetPlayoutSampleRate(0);
  _ptrAudioBuffer->SetRecordingChannels(0);
  _ptrAudioBuffer->SetPlayoutChannels(0);
}

bool ThirdpartyAudioDevice::PlayThreadFunc(void* pThis)
{
    return (static_cast<ThirdpartyAudioDevice*>(pThis)->PlayThreadProcess());
}

bool ThirdpartyAudioDevice::RecThreadFunc(void* pThis) {
  return (static_cast<ThirdpartyAudioDevice*>(pThis)->RecThreadProcess());
}

bool ThirdpartyAudioDevice::PlayThreadProcess()
{
    // printf("[%s:%d]\n", __FUNCTION__, __LINE__);
    if(!_playing)
    {
        emxlogi("ThirdpartyAudioDevice _playing if false\n");
        return false;
    }

    // const int KGroupNums = 2;
//    static int cnt = 0;
    uint64_t currentTime = _clock->CurrentNtpInMilliseconds();

    int32_t toBeCalledCnt = _playingRectifier.RestCntForNow(currentTime);

#ifdef  _DEBUG_
    cnt++;
    //if (cnt % 500 == 1)
    {
        printf("play:  toBeCalledCnt:%d, currentTime=%llu, interval=%llu\n", toBeCalledCnt, currentTime,
               currentTime - _lastCallPlayoutMillis);
    }
    if (toBeCalledCnt > 1 && cnt % 20 == 1) {
        printf("play: toBeCalledCnt=%d\n", toBeCalledCnt);
    }
    if (toBeCalledCnt >= 4) {
      printf("play: toBeCalledCnt=%d is to large, meaning the sleep delay is too large!!!\n", toBeCalledCnt);
      printf("play: currentTime=%llu, interval=%llu\n", currentTime,
               currentTime - _lastCallPlayoutMillis);
    }
#endif
    int tmpPlayoutBufferSize = gAudioParam.sampleRate / 100 * gAudioParam.chnNum * 2;
    if (toBeCalledCnt < 0) {
      SleepMs(1); //休眠1ms，避免线程空转
      return true;
    }
    //emxlogd("toBeCalledCnt(%d)\n", toBeCalledCnt);
    for (int i = 0; i < toBeCalledCnt; i++) {
        _ptrAudioBuffer->RequestPlayoutData(_playoutFramesIn10MS);
        _playoutFramesLeft = _ptrAudioBuffer->GetPlayoutData(_playoutBuffer);
        assert(_playoutFramesLeft == _playoutFramesIn10MS);
#ifdef CMCC_PCM_FILE
        if (_outputFile.Open()) {
          _outputFile.Write(_playoutBuffer, tmpPlayoutBufferSize);
          _outputFile.Flush();
        }
#else
        /**
         * 1.非升级状态
         * 2.非系统语音播报过程进行中
         * 3.非实时语音对讲过程中
         * 4.非设备未绑定状态
         */
//        cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
//        if (voip_info_t->get_voip_audio_play_feasible()) {
                //数据指针_playoutBuffer，数据长度固定 160（8K）或 320（16K）字节；
//            media_pcm_frame(0, (unsigned char*)_playoutBuffer, tmpPlayoutBufferSize);

            MediaAdec::Frame frame = {};
            frame.data = (uint8_t *)_playoutBuffer;
            frame.size = tmpPlayoutBufferSize;
            // printf("%s:%d,frame.size=%d\n", __FUNCTION__, __LINE__,frame.size);
            if (*(gAudioParam.isPlayVoiceCall)) {
                MediaClientAdecStreamSync::PushRawPcm(0,frame);
            }
//        }
#endif
        _playoutFramesLeft = 0;
        _playingRectifier.AddToStatistics();
    }
    _lastCallPlayoutMillis = currentTime;

    uint64_t beforeSleepTs = _clock->CurrentNtpInMilliseconds();
    int      deltaTime = (int)(beforeSleepTs - currentTime);
    int      offsetTimeMs = 0;

    if (10 - deltaTime - offsetTimeMs > 0) {
        SleepMs(10 - deltaTime - offsetTimeMs);

#ifdef  _DEBUG_
        //if (cnt % 500 == 1)
        {
            uint64_t afterSleepTs = _clock->CurrentNtpInMilliseconds();
            printf(
                "play: deltaTime:%d, beforeSleepTs:%llu, afterSleepTs=%llu, reSleepTime=%llu, exSleepTime=%llu, "
                "offsetTimeMs=%d\n",
                deltaTime, beforeSleepTs, afterSleepTs, afterSleepTs - beforeSleepTs,
                10 - deltaTime, offsetTimeMs);
        }
#endif
    }else{
#ifdef  _DEBUG_
      printf("play: not need to sleep, deltaTime:%d beforeSleepTs:%llu\n",deltaTime,beforeSleepTs);
#endif
    }

    return true;
}

int ThirdpartyAudioDevice::PushAudioProcess(int needSize)
{
    MediaFrame frame = {};
    if(needSize > _RecAudio_size) {
        // printf("%s:%d\n", __FUNCTION__, __LINE__);
        auto e = _stream->GetFrame(frame,5000);
        if (e == ErrCodeE::Success) {
            if ((4096 - (_RecAudio_index + _RecAudio_size)) < frame.size) {
                memcpy(&_RecAudio_buf[0], &_RecAudio_buf[_RecAudio_index], _RecAudio_size);
                _RecAudio_index = 0;
            }
            memcpy(&_RecAudio_buf[_RecAudio_index], frame.data, frame.size);
            _RecAudio_size += frame.size;
           // printf( "mbuf.data_len = %d,_RecAudio_index = %d,_RecAudio_size=%d\n",mbuf.data_len, _RecAudio_index , _RecAudio_size);
//            media_stream_release(&mbuf);
        } else {
          emxloge("GetFrame is failed\n");
        }
    }
    return 0;
}

bool ThirdpartyAudioDevice::RecThreadProcess() {

    if (!_recording) {
        printf( "[%s:%d]_recording:%d\n", __FUNCTION__ , __LINE__, _recording);
        return false;
    }
//    cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
    // if(!voip_info_t->get_voip_audio_play_feasible()){
    //     return false;
    // }
    int i = 0;
//    int loop=0;
//    int mqCnt = 0;
//    static int cnt = 0;
    static int err_cnt = 0;
    uint64_t currentTime = _clock->CurrentNtpInMilliseconds();
    // 获取当次应取样次数(考虑此前时间误差导致的取样偏少情况)
    int64_t toBeCalledCnt = _recordingRectifier.RestCntForNow(currentTime);
    if (toBeCalledCnt < 0) {
        err_cnt++;
        SleepMs(10); //休眠1ms，避免线程空转
        if(err_cnt > 50){
            _recordingRectifier.ClearStatistics();
        }
        //printf( "[%s:%d] sleep toBeCalledCnt:%lld\n", __FUNCTION__ , __LINE__, toBeCalledCnt);
        return true;
    }
    PushAudioProcess(toBeCalledCnt*_recordingBufferSizeIn10MS);
    err_cnt = 0;
    for (i = 0; i < toBeCalledCnt; i++) {
        char *pdata = &_RecAudio_buf[_RecAudio_index];
        if(_RecAudio_size >= _recordingBufferSizeIn10MS) {
            memcpy(_recordingBuffer, pdata, _recordingBufferSizeIn10MS);
            _RecAudio_index += _recordingBufferSizeIn10MS;
            _RecAudio_size -= _recordingBufferSizeIn10MS;
          //  printf( "111_RecAudio_index = %d,_RecAudio_size=%d\n", _RecAudio_index , _RecAudio_size);

            _ptrAudioBuffer->SetRecordedBuffer(_recordingBuffer, _recordingFramesIn10MS);
            _ptrAudioBuffer->DeliverRecordedData();
        }
        _recordingRectifier.AddToStatistics();
    }
    _lastCallRecordMillis = currentTime;

    uint64_t beforeSleepTs = _clock->CurrentNtpInMilliseconds();
    int      deltaTime = (int)(beforeSleepTs - currentTime);
    int      offsetTimeMs = 0;

    if (10 - deltaTime - offsetTimeMs > 0) {
        SleepMs(10 - deltaTime- offsetTimeMs);
#ifdef  _DEBUG_
        if (cnt % 500 == 1) {
            uint64_t afterSleepTs = _clock->CurrentNtpInMilliseconds();
            printf("record: afterSleepTs=%llu, reSleepTime=%llu, "
                   "exSleepTime=%llu, "
                   "offsetTimeMs=%d\n",
                   afterSleepTs, afterSleepTs - beforeSleepTs, 10 - deltaTime,
                   offsetTimeMs);
        }
#endif
    }else{
#ifdef  _DEBUG_
      printf("record: not need to sleep, deltaTime:%d, beforeSleepTs:%llu\n", deltaTime,beforeSleepTs);
#endif
    }

    return true;
}

#if 0
bool ThirdpartyAudioDevice::RecThreadProcess() {
    if (!_recording) {
        printf( "[%s:%d]_recording:%d\n", __FUNCTION__ , __LINE__, _recording);
        return false;
    }

    cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
    if(!voip_info_t->get_voip_audio_play_feasible()){
        return false;
    }
    int loop=0;
    int mqCnt = 0;
    // const int KGroupNums = 2;
    static int cnt = 0;
    static int err_cnt = 0;
    uint64_t currentTime = _clock->CurrentNtpInMilliseconds();
    int64_t toBeCalledCnt = _recordingRectifier.RestCntForNow(currentTime);

#ifdef  _DEBUG_
    cnt++;
    if (cnt % 500 == 1) {
        printf("record: currentTime=%llu, interval=%llu\n", currentTime,
               currentTime - _lastCallRecordMillis);
    }
    if (toBeCalledCnt > 1 && cnt % 20 == 1) {
        printf("record: toBeCalledCnt=%d\n", toBeCalledCnt);
    }
    if (toBeCalledCnt >= 4) {
         printf("record: toBeCalledCnt=%d is to large, meaning the sleep delay is too large!!!\n", toBeCalledCnt);
    }
#endif

    if (toBeCalledCnt < 0) {
        err_cnt++;
        SleepMs(10); //休眠1ms，避免线程空转
        if(err_cnt > 50){
            _recordingRectifier.ClearStatistics();
        }
        //printf( "[%s:%d] sleep toBeCalledCnt:%lld\n", __FUNCTION__ , __LINE__, toBeCalledCnt);
        return true;
    }
    err_cnt = 0;
    mqCnt = media_stream_bufed_get(_voiceStreamId);
    loop = (int)toBeCalledCnt-mqCnt;
    //printf( "[%s:%d]mqCnt:%d, toBeCalledCnt:%llu, loop:%d\n", __FUNCTION__ , __LINE__, mqCnt, toBeCalledCnt, loop);
    for (int i = 0; i < loop; i++) {
        _recordingRectifier.AddToStatistics();
    }
    if (toBeCalledCnt > mqCnt) {
        toBeCalledCnt = mqCnt;
    }

    for (int i = 0; i < toBeCalledCnt; i++) {
#if 0
        if (_inputFile.Open()) {
            if (_inputFile.Read(_recordingBuffer, _recordingBufferSizeIn10MS) > 0) {
                _ptrAudioBuffer->SetRecordedBuffer(_recordingBuffer,
                                                   _recordingFramesIn10MS);
            } else {
                _inputFile.Rewind();
            }

            _ptrAudioBuffer->DeliverRecordedData();

            _recordingRectifier.AddToStatistics();
        }
#endif
        media_buf_t mbuf;
        int res = media_stream_get(_voiceStreamId, 5, &mbuf);
        if (res == X_SUCCESS)
        {
            unsigned char *pdata = mbuf.data;
            int need_len = mbuf.data_len;
            do {
                int send_len = _recordingBufferSizeIn10MS;
                if(need_len < _recordingBufferSizeIn10MS)
                    send_len = need_len;
                memcpy(_recordingBuffer,pdata, send_len);
                _ptrAudioBuffer->SetRecordedBuffer(_recordingBuffer, _recordingFramesIn10MS);
                _ptrAudioBuffer->DeliverRecordedData();
                _recordingRectifier.AddToStatistics();
                pdata += send_len;
                need_len -= send_len;
            }while(need_len > 0);
            media_stream_release(&mbuf);
        }
    }
    _lastCallRecordMillis = currentTime;

    uint64_t beforeSleepTs = _clock->CurrentNtpInMilliseconds();
    int      deltaTime = (int)(beforeSleepTs - currentTime);
    int      offsetTimeMs = 0;

    if (10 - deltaTime - offsetTimeMs > 0) {
        SleepMs(10 - deltaTime- offsetTimeMs);
#ifdef  _DEBUG_
        if (cnt % 500 == 1) {
            uint64_t afterSleepTs = _clock->CurrentNtpInMilliseconds();
            printf("record: afterSleepTs=%llu, reSleepTime=%llu, "
                   "exSleepTime=%llu, "
                   "offsetTimeMs=%d\n",
                   afterSleepTs, afterSleepTs - beforeSleepTs, 10 - deltaTime,
                   offsetTimeMs);
        }
#endif
    }else{
#ifdef  _DEBUG_
      printf("record: not need to sleep, deltaTime:%d, beforeSleepTs:%llu\n", deltaTime,beforeSleepTs);
#endif
    }

    return true;
}
#endif
int ThirdpartyAudioDevice::ProvideRecordedAudioData(const char* recordedData, unsigned int nSamples){
  int ret = -1;
  _critSect.Enter();
  _ptrAudioBuffer->SetRecordedBuffer(recordedData,
                                             _recordingFramesIn10MS);
  ret = _ptrAudioBuffer->DeliverRecordedData();

  _critSect.Leave();

  return ret;
}


int ThirdpartyAudioDevice::RequestPlayoutAudioData(char* recordedData){
  int ret = -1;
  _critSect.Enter();
  _ptrAudioBuffer->RequestPlayoutData(_playoutFramesIn10MS);
  _playoutFramesLeft = _ptrAudioBuffer->GetPlayoutData(recordedData);
  assert(_playoutFramesLeft == _playoutFramesIn10MS);
  _playoutFramesLeft = 0;
  _critSect.Leave();
  return ret;
}

}  // namespace cmcc_webrtc

