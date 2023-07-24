/*
 *  Third party audio device factory
 */


#ifndef WEBRTC_AUDIO_DEVICE_THIRD_PARTY_DEVICE_FACTORY_H
#define WEBRTC_AUDIO_DEVICE_THIRD_PARTY_DEVICE_FACTORY_H

#include "common_types.h"
#include "third_party_device.h"

namespace cmcc_webrtc {

class ThirdpartyAudioDevice;

class ThirdpartyAudioDeviceFactory {
 public:
  static ThirdpartyAudioDevice* CreateThirdpartyAudioDevice(const int32_t id);

  // The input file must be a readable 48k stereo raw file. The output
  // file must be writable. The strings will be copied.
  static void SetFilenamesToUse(const char* inputAudioFilename,
                                const char* outputAudioFilename);

 private:
  static const uint32_t MAX_FILENAME_LEN = 256;
  static char _inputAudioFilename[MAX_FILENAME_LEN];
  static char _outputAudioFilename[MAX_FILENAME_LEN];
};

}  // namespace cmcc_webrtc

#endif  // WEBRTC_AUDIO_DEVICE_THIRD_PARTY_DEVICE_FACTORY_H
 