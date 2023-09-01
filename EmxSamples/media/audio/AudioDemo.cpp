/**
 * @file AACDemo.cpp
 * @author hening
 * @brief audio编解码测试demo
 * @date 2023-8-1: 支持fdk-aac音频编码测试
 */
#include "EmxCore.hpp"
#include "AudioCodec.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

#define bitRate 64
#define sampleRate 8000
#define bitWidth 16

int main(int argc, char *argv[]) {
    if (argc < 3) {
        emxloge("argument error: [AudioDemo input_pcm_path output_aac_path channelNum]\n");
        return -1;
    }
    std::string pcmfile = std::string(argv[1]);
    std::string aacfile = std::string(argv[2]);
    int channelNum = std::stoi(argv[3]);
    
    std::string data;
    if (File::Read(pcmfile.c_str(), data) == ErrCodeE::Failure) {
        emxloge("read %s failed\n", pcmfile.c_str());
        return -1;
    }

    std::shared_ptr<AudioEncoder> encoder;
    encoder.reset(AudioCodecFactory::MakeEncoder(AudioCodecE::AAC));
    encoder->Create(bitRate, sampleRate, bitWidth, channelNum);
    
    std::ofstream file;
    file.open(aacfile.c_str(), std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        emxloge("open %s failed.\n", aacfile.c_str());
        return -1;
    }
    int inSize = data.size();
    int blockSize = 1024;
    int off = 0;
    while(inSize > 0) {
        int inputSize = inSize > blockSize ? blockSize : inSize;
        int outputSize = 2048;
        uint8_t output[outputSize] = { 0 };
        encoder->Encode((int16_t*)(data.data() + off), inputSize, output, outputSize);
        file.write(reinterpret_cast<const char*>(output), outputSize);
        off += (inputSize * 2);
        inSize -= (inputSize * 2);
    }
    file.close();
    return 0;
}