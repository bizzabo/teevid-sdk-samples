#include "VideoFrameData.h"

//const int kDummyVideoFrameWidth = 1920;
//const int kDummyVideoFrameHeight = 1080;
const int kBytesPerPixel = 4;
//const size_t kDummyVideoSize = kDummyVideoFrameWidth * kDummyVideoFrameHeight * kBytesPerPixel;

VideoFrameData::VideoFrameData(unsigned char index, int width, int height) : width_(width), height_(height)
{
    // these are dummy video frames generated based on input value
    size_t videoSize = width_ * height_ * kBytesPerPixel;
    data_ = (unsigned char*)malloc(videoSize * sizeof(unsigned char));

    for (size_t j = 0; j < videoSize; j += kBytesPerPixel)
    {
        if (j < videoSize / 3)
        {
            data_[j] = 0xff;
            data_[j + 1] = (index < 16) ? (index * 8) : (32 - index) * 8;
            data_[j + 2] = (index < 16) ? (index * 8) : (32 - index) * 8;
        }
        else if (j < videoSize * 2 / 3)
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
        free(data_);
        data_ = nullptr;
    }
}

unsigned char* VideoFrameData::GetData() const
{
    return data_;
}

int VideoFrameData::GetWidth() const
{
    return width_;
}

int VideoFrameData::GetHeight() const
{
    return height_;
}

int VideoFrameData::GetBytesPerPixel() const
{
    return kBytesPerPixel;
}

size_t VideoFrameData::GetSize() const
{
    return width_ * height_ * kBytesPerPixel;
}
