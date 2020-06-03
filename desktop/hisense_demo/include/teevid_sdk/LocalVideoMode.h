//
// Created by root on 21.05.20.
//

#ifndef _LOCALVIDEOMODE_H_
#define _LOCALVIDEOMODE_H_
namespace teevid_sdk
{
    typedef enum {
        eNone,   // Do no show local video
        eSmall,  // Show it in small window
        eMedium, // Show it in medium-size window (for large TVs)
        eMax     // Show maximum available resolution
    } LocalVideoMode;
}
#endif //_LOCALVIDEOMODE_H_
