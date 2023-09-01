//
// Created by xiong on 2020/6/18.
//
#include "EmxCore.hpp"
#include "AACCodec.hpp"

using namespace Emx;

ErrCodeE AACEncoder::Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth, int32_t channelNum) {
    if ((CHANNEL_MODE)channelNum < MODE_1 || (CHANNEL_MODE)channelNum > MODE_2) {
        emxloge("Unable to set the channel[%d]\n", channelNum);
        return ErrCodeE::IllegalParam;
    }
    int aot;
    int afterburner;
    AACENC_InfoStruct info = {0};

    aot = AOT_AAC_LC;
    afterburner = 1;

    if (aacEncOpen(&m_handle, 0, channelNum) != AACENC_OK) {
        emxloge("Unable to open encoder\n");
        return ErrCodeE::OpenFailed;
    }
    if (aacEncoder_SetParam(m_handle, AACENC_AOT, aot) != AACENC_OK) {
        emxloge("Unable to set the AOT\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncoder_SetParam(m_handle, AACENC_SAMPLERATE, sampleRate) != AACENC_OK) {
        emxloge("Unable to set the AOT\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncoder_SetParam(m_handle, AACENC_CHANNELMODE, (CHANNEL_MODE)channelNum) != AACENC_OK) {
        emxloge("Unable to set the channel mode\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncoder_SetParam(m_handle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
        emxloge("Unable to set the wav channel order\n");
        return ErrCodeE::IllegalParam;
    }

    if (aacEncoder_SetParam(m_handle, AACENC_BITRATE, bitRate * 1024) != AACENC_OK) {
        emxloge("Unable to set the bitrate\n");
        return ErrCodeE::IllegalParam;
    }

    if (aacEncoder_SetParam(m_handle, AACENC_TRANSMUX, 2) != AACENC_OK) {
        emxloge("Unable to set the ADTS transmux\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncoder_SetParam(m_handle, AACENC_AFTERBURNER, afterburner) != AACENC_OK) {
        emxloge("Unable to set the afterburner mode\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncEncode(m_handle, nullptr, nullptr, nullptr, nullptr) != AACENC_OK) {
        emxloge("Unable to initialize the encoder\n");
        return ErrCodeE::IllegalParam;
    }
    if (aacEncInfo(m_handle, &info) != AACENC_OK) {
        emxloge("Unable to get the encoder info\n");
        return ErrCodeE::IllegalParam;
    }
    return ErrCodeE::Success;
}

void AACEncoder::Destroy() {
    aacEncClose(&m_handle);
}

ErrCodeE AACEncoder::Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) {
    ErrCodeE ret = ErrCodeE::ResNotAvailable;
    m_buff.append(std::string((char *)inData, inNum * 2));
    if (m_buff.size() < (1024 * 2)) {
        outNum = 0;
        return ret;
    }
    ret = ToEncode((int16_t *) m_buff.data(), 1024, outData, outNum);
    int off = 1024 * 2;
    int size = m_buff.size() - off;
    memcpy((char*)m_buff.data(), m_buff.data() + off, size);
    m_buff.resize(size);
    m_buff.shrink_to_fit();
    return ret;
}

ErrCodeE AACEncoder::ToEncode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) {
    short *convert_buf = inData;
    AACENC_BufDesc in_buf = {0};
    AACENC_BufDesc out_buf = {0};
    AACENC_InArgs in_args = {0};
    AACENC_OutArgs out_args = {0};
    int in_identifier = IN_AUDIO_DATA;
    int out_identifier = OUT_BITSTREAM_DATA;
    int in_size, in_elem_size;
    int out_size, out_elem_size;
    void *in_ptr, *out_ptr;
    AACENC_ERROR err;

    in_ptr = convert_buf;
    in_size = inNum * 2;
    in_elem_size = 2;

    in_args.numInSamples = inNum;
    in_buf.numBufs = 1;
    in_buf.bufs = &in_ptr;
    in_buf.bufferIdentifiers = &in_identifier;
    in_buf.bufSizes = &in_size;
    in_buf.bufElSizes = &in_elem_size;

    out_ptr = outData;
    out_size = outNum;
    out_elem_size = 1;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes = &out_size;
    out_buf.bufElSizes = &out_elem_size;
    err = aacEncEncode(m_handle, &in_buf, &out_buf, &in_args, &out_args);
    if (err != AACENC_OK) {
        emxloge("Encoding failed\n");
        return ErrCodeE::Failure;
    }
    outNum = out_args.numOutBytes;
    return ErrCodeE::Success;
}

ErrCodeE AACDecoder::Create(int32_t sampleRate, int32_t bitWidth) {
    m_handle = aacDecoder_Open(TT_MP4_ADTS, 1);
    return ErrCodeE::Success;
}

void AACDecoder::Destroy() {
    aacDecoder_Close(m_handle);
}

ErrCodeE AACDecoder::Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) {
    AAC_DECODER_ERROR err;
    CStreamInfo *info;
    UINT valid;
    UINT packet_size;
    valid = packet_size = inNum;
    err = aacDecoder_Fill(m_handle, &inData, &packet_size, &valid);
    if (err != AAC_DEC_OK) {
        emxloge("Fill failed: %x\n", err);
        return ErrCodeE::Failure;
    }
    err = aacDecoder_DecodeFrame(m_handle, (INT_PCM *) outData, 2048, 0);
    if (err == AAC_DEC_NOT_ENOUGH_BITS)
        return ErrCodeE::FrameError;
    if (err != AAC_DEC_OK) {
        emxloge("Decode failed: %x\n", err);
        return ErrCodeE::Failure;
    }
    info = aacDecoder_GetStreamInfo(m_handle);
    if (!info || info->sampleRate <= 0) {
        emxloge("No stream info\n");
        return ErrCodeE::IllegalParam;
    }
    outNum = info->frameSize * info->numChannels;
    return ErrCodeE::ResNotAvailable;
}

