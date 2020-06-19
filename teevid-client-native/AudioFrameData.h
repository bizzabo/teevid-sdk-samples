#ifndef AUDIOFRAMEDATA_H
#define AUDIOFRAMEDATA_H

#include <iostream>

class AudioFrameData
{
public:
    AudioFrameData();
    virtual ~ AudioFrameData();

    unsigned char* GetData() const;
    int GetChannelsCount() const;
    int GetSampleSize() const;
    size_t GetSize() const;

private:
    unsigned char* data_ = nullptr;
};

#endif // AUDIOFRAMEDATA_H
