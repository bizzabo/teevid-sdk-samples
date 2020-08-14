#include "AudioParams.h"

const int kDummyAudioFrameChannelsCount = 2;
const int kDummyVideoFrameSampleSize = 2;
const int kWavFileDataOffset = 44;

AudioParams::AudioParams()
{

}

void AudioParams::SetSampleRate(int rate)
{
    _sampleRate = rate;
}

int AudioParams::GetSampleRate() const
{
    return _sampleRate;
}

void AudioParams::SetFrameSampleRate(int rate)
{
    _frameSampleRate = rate;
}

int AudioParams::GetFrameSampleRate() const
{
    return _frameSampleRate;
}

int AudioParams::GetChannelsCount() const
{
    return kDummyAudioFrameChannelsCount;
}

int AudioParams::GetBps() const
{
    return kDummyVideoFrameSampleSize;
}

int AudioParams::GetWavFileDataOffset() const
{
    return kWavFileDataOffset;
}

int AudioParams::GetDataChunkSize() const
{
    return _frameSampleRate * kDummyAudioFrameChannelsCount * kDummyVideoFrameSampleSize;
}
