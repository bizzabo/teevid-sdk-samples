#ifndef TEEVID_SDK_SETTINGS_H
#define TEEVID_SDK_SETTINGS_H

#include "SourceMode.h"

#include <string>

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

typedef enum
{
  TopLeft = 0,
  TopRight = 1,
  BottomLeft = 2,
  BottomRight = 3
} eWatermarkPosition;

struct WatermarkOptions
{
  std::string imageFileName;
  int width = 0;
  int height = 0;

  int offset_x = 0;
  int offset_y = 0;
  eWatermarkPosition position = eWatermarkPosition::TopLeft;

  double alpha = 1;
};

typedef enum
{
  kNone,               // Identity (no rotation)
  kCounterClockwise,   // Rotate counter-clockwise 90 degrees
  kRotate180,          // Rotate 180 degrees
  kClockwise,          // Rotate clockwise 90 degrees
  kHorizontalFlip,     // Flip horizontally
  kUpperRightDiagonal, // Flip across upper right/lower left diagonal
  kVerticalFlip,       // Flip vertically
  kUpperLeftDiagonal   // Flip across upper left/lower right diagonal
} FlipMethod;

typedef enum {
  eGStreamer,
  eNVidia
} SinkElement;

struct CropSettings
{
  int left = 0;   // Pixels to crop at left
  int right = 0;  // Pixels to crop at right
  int top = 0;    // Pixels to crop at top
  int bottom = 0; // Pixels to crop at bottom
};

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

  // determine whether the source video should be resized
  // 0 means no resize
  int sourceWidth = 0;
  int sourceHeight = 0;

  // this flag allows MJPG camera format usage
  // available only for INTERNAL pipeline mode and ONLY for EMBEDDED architecture
  // disabled by default
  bool allowMJPG = false;

  bool useWatermark = false;
  WatermarkOptions watermarkOptions;

  FlipMethod flipMethod = kNone;
  CropSettings cropSettings;
};

typedef struct {
  AudioSettings audioSettings;
  VideoSettings videoSettings;
  SourceMode sourceMode;
  unsigned int previewWindowId = 0;
  SinkElement sinkElement = eGStreamer;
} MediaSettings;

}

#endif //TEEVID_SDK_SETTINGS_H
