#ifndef TEEVID_SDK_SETTINGS_H
#define TEEVID_SDK_SETTINGS_H

namespace teevid_sdk
{

typedef enum {
    kRGB24 = 0
} VideoFormatType;

typedef enum {
    kPCM = 0
} AudioFormatType;

typedef enum {
} AudioCodecType;

typedef enum {
    kVP8 = 0,
    kVP9 = 1,
    kH264 = 2,
    kH265 = 3,
    kOPUS = 4
} CodecType;

typedef struct {
    VideoFormatType videoFormatType;
    AudioFormatType audioFormatType;
    CodecType videoCodecType;
    CodecType audioCodecType;
    int audioChannels = 2;
    int audioBps = 16;
    int audioSampleRate = 48000;
} MediaSettings;

}

#endif //TEEVID_SDK_SETTINGS_H
