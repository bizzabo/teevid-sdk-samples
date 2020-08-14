#ifndef VIDEOFRAMEDATA_H
#define VIDEOFRAMEDATA_H

#include <iostream>

class VideoFrameData
{
public:
    VideoFrameData();
    VideoFrameData(unsigned char index, int width, int height);
    virtual ~VideoFrameData();

    unsigned char* GetData() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetBytesPerPixel() const;
    size_t GetSize() const;

private:
    unsigned char* data_ = nullptr;

    int width_;
    int height_;
};

#endif // VIDEOFRAMEDATA_H
