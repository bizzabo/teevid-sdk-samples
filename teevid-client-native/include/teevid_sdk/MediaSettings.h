#ifndef TEEVID_SDK_SETTINGS_H
#define TEEVID_SDK_SETTINGS_H

namespace teevid_sdk
{

typedef enum {
    kRGBA = 0,
    kBGRx = 1,
    kGRAY8 = 2,
    kUYVY = 3,
    kYUY2 = 4,
    kYVYU = 5,
    kI420 = 6,
    kI420_10LE = 7,
    kNV12 = 8
} VideoFormatType;

typedef enum {
  kMono = 1,
  kStereo = 2
} AudioChannels;

typedef enum {
  kF64LE = 0,
  kF64BE = 1,
  kF32LE = 2,
  kF32BE = 3,
  kS32LE = 4,
  kS32BE = 5,
  kU32LE = 6,
  kU32BE = 7,
  kS24_32LE = 8,
  kS24_32BE = 9,
  kU24_32LE = 10,
  kU24_32BE = 11,
  kS24LE = 12,
  kS24BE = 13,
  kU24LE = 14,
  kU24BE = 15,
  kS20LE = 16,
  kS20BE = 17,
  kU20LE = 18,
  kU20BE = 19,
  kS18LE = 20,
  kS18BE = 21,
  kU18LE = 22,
  kU18BE = 23,
  kS16LE = 24,
  kS16BE = 25,
  kU16LE = 26,
  kU16BE = 27,
  kS8 = 28,
  kU8 = 29
} AudioBpsType;

struct AudioSettings
{
  AudioChannels audioChannels = kStereo;
  AudioBpsType audioBpsType = kS16LE;
  int audioSampleRate = 48000;
};

struct VideoSettings
{
  VideoFormatType videoFormatType = kRGBA;
  int videoFps = 30;
  int videoWidth = 1920;
  int videoHeight = 1080;
};

typedef struct {
  AudioSettings audioSettings;
  VideoSettings videoSettings;
} MediaSettings;

}

#endif //TEEVID_SDK_SETTINGS_H
