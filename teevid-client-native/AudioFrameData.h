#ifndef AUDIOFRAMEDATA_H
#define AUDIOFRAMEDATA_H

#include <iostream>
#include <QByteArray>

#include "AudioParams.h"

class AudioFrameData
{
public:
    AudioFrameData(const AudioParams& audioParams);
    AudioFrameData(const AudioParams& audioParams, int index);
    AudioFrameData(const AudioParams& audioParams, QByteArray buffer, int index);
    virtual ~ AudioFrameData();

    unsigned char* GetData() const;
    int GetSize();

private:
    unsigned char* data_ = nullptr;

    int _dummyAudioSize;
    int _wavFileDataPos;
};

#endif // AUDIOFRAMEDATA_H
