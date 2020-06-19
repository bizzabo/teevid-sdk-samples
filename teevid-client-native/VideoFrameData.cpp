#include "VideoFrameData.h"

const int kDummyVideoFrameWidth = 1920;
const int kDummyVideoFrameHeight = 1080;
const int kBytesPerPixel = 4;
const size_t kDummyVideoSize = kDummyVideoFrameWidth * kDummyVideoFrameHeight * kBytesPerPixel;

VideoFrameData::VideoFrameData(unsigned char index)
{
    // these are dummy video frames generated based on input value
    data_ = new unsigned char[kDummyVideoSize];

    for (size_t j = 0; j < kDummyVideoSize; j += kBytesPerPixel)
    {
        if (j < kDummyVideoSize / 3)
        {
            data_[j] = 0xff;
            data_[j + 1] = (index < 16) ? (index * 8) : (32 - index) * 8;
            data_[j + 2] = (index < 16) ? (index * 8) : (32 - index) * 8;
        }
        else if (j < kDummyVideoSize * 2 / 3)
        {
            data_[j] = (index < 16) ? (index * 8) : (32 - index) * 8;
            data_[j + 1] = 0xff;
            data_[j + 2] = (index < 16) ? (index * 8) : (32 - index) * 8;
        }
        else
        {
            data_[j] = (index < 16) ? (index * 8) : (32 - index) * 8;
            data_[j + 1] = (index < 16) ? (index * 8) : (32 - index) * 8;
            data_[j + 2] = 0xff;
        }
        data_[j + 3] = 0xff; // alpha channel
    }
}

VideoFrameData::~VideoFrameData()
{
    if (data_ != nullptr)
    {
        delete[] data_;
        data_ = nullptr;
    }
}

unsigned char* VideoFrameData::GetData() const
{
    return data_;
}

int VideoFrameData::GetWidth() const
{
    return kDummyVideoFrameWidth;
}

int VideoFrameData::GetHeight() const
{
    return kDummyVideoFrameHeight;
}

int VideoFrameData::GetBytesPerPixel() const
{
    return kBytesPerPixel;
}

size_t VideoFrameData::GetSize() const
{
    return kDummyVideoSize;
}
