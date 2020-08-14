#include "AudioFrameData.h"

AudioFrameData::AudioFrameData(const AudioParams &audioParams) : _wavFileDataPos(audioParams.GetWavFileDataOffset())
{
    _dummyAudioSize = audioParams.GetDataChunkSize();
    data_ = (unsigned char*)malloc(_dummyAudioSize * sizeof(unsigned char));
    memset(data_, 0, _dummyAudioSize * sizeof(data_[0]));
}

AudioFrameData::AudioFrameData(const AudioParams &audioParams, int index) : _wavFileDataPos(audioParams.GetWavFileDataOffset())
{
    // this method normally should not be used
    // only if sample file is absent or invalid

    _dummyAudioSize = audioParams.GetDataChunkSize();
    data_ = (unsigned char*)malloc(_dummyAudioSize * sizeof(unsigned char));
    size_t sampleValuesCount = 16;

    // this are dummy values from white noise WAV file
    for (size_t i = 0; i < _dummyAudioSize; i += sampleValuesCount)
    {
        data_[i]     = 0x10;
        data_[i + 1] = 0xb1;
        data_[i + 2] = 0x02;
        data_[i + 3] = 0x00;
        data_[i + 4] = 0x04;
        data_[i + 5] = 0x00;
        data_[i + 6] = 0x10;
        data_[i + 7] = 0x00;
        data_[i + 8] = 0x64;
        data_[i + 9] = 0x61;
        data_[i + 10] = 0x74;
        data_[i + 11] = 0x61;
        data_[i + 12] = 0xa0;
        data_[i + 13] = 0xea;
        data_[i + 14] = 0x1a;
        data_[i + 15] = 0x00;
    }
}

AudioFrameData::AudioFrameData(const AudioParams &audioParams, QByteArray buffer, int index)  : _wavFileDataPos(audioParams.GetWavFileDataOffset())
{
    _dummyAudioSize = audioParams.GetDataChunkSize();
    data_ = (unsigned char*)malloc(_dummyAudioSize * sizeof(unsigned char));
    memcpy(data_, buffer.data() + index * _dummyAudioSize + _wavFileDataPos, _dummyAudioSize);
}

AudioFrameData::~AudioFrameData()
{
    if (data_ != nullptr)
    {
        free(data_);
        data_ = nullptr;
    }
}

unsigned char* AudioFrameData::GetData() const
{
    return data_;
}

int AudioFrameData::GetSize()
{
    return _dummyAudioSize;
}
