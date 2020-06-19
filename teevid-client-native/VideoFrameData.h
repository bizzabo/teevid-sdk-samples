#ifndef VIDEOFRAMEDATA_H
#define VIDEOFRAMEDATA_H

#include <iostream>

class VideoFrameData
{
public:
    VideoFrameData();
    VideoFrameData(unsigned char index);
    virtual ~VideoFrameData();

    unsigned char* GetData() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetBytesPerPixel() const;
    size_t GetSize() const;

private:
    unsigned char* data_ = nullptr;
};

#endif // VIDEOFRAMEDATA_H
