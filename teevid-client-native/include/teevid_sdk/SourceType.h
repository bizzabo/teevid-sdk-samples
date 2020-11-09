//
// Created by root on 20.05.20.
//

#ifndef _SOURCETYPE_H_
#define _SOURCETYPE_H_
namespace teevid_sdk
{
    typedef enum {
        eAudio = 0,
        eVideo = 1
    } SourceType;

    typedef struct{
        SourceType type;
        int deviceId;
        std::string name;
    }SourceInfo;
}
#endif //_SOURCETYPE_H_
