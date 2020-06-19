#include "AudioFrameData.h"


const int kDummyAudioFrameChannelsCount = 2;
const int kDummyVideoFrameSampleSize = 2;
const int kDummyVideoFrameSampleRate = 1600;
const size_t kDummyAudioSize = kDummyAudioFrameChannelsCount * kDummyVideoFrameSampleSize * kDummyVideoFrameSampleRate;

AudioFrameData::AudioFrameData()
{
    data_ = new unsigned char[kDummyAudioSize];
    size_t sampleValuesCount = 16;

    // this are dummy values from white noise WAV file
    for (size_t i = 0; i < kDummyAudioSize; i += sampleValuesCount)
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

AudioFrameData::~AudioFrameData()
{
    if (data_ != nullptr)
    {
        delete[] data_;
        data_ = nullptr;
    }
}

unsigned char* AudioFrameData::GetData() const
{
    return data_;
}

int AudioFrameData::GetChannelsCount() const
{
    return kDummyAudioFrameChannelsCount;
}

int AudioFrameData::GetSampleSize() const
{
    return kDummyAudioFrameChannelsCount;
}

size_t AudioFrameData::GetSize() const
{
    return kDummyAudioSize;
}
