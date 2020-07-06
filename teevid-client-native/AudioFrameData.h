#ifndef AUDIOFRAMEDATA_H
#define AUDIOFRAMEDATA_H

#include <iostream>

class AudioFrameData
{
public:
    AudioFrameData();
    virtual ~ AudioFrameData();

    unsigned char* GetData() const;
    size_t GetSize() const;

private:
    unsigned char* data_ = nullptr;
};

#endif // AUDIOFRAMEDATA_H
