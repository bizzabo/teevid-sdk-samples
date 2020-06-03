#ifndef IVIDEOPROVIDER_H
#define IVIDEOPROVIDER_H

#include <cstdlib>

struct IVideoProvider
{    
    enum Quality
    {
        eLow = 0,
        eHigh = 1
    };

    virtual void onVideoFrame(unsigned char* data, size_t size, size_t stride, long streamId) = 0;
};

#endif // IVIDEOPROVIDER_H
