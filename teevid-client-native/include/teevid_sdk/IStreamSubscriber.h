//
// Created by root on 25.05.20.
//

#ifndef _ISTREAMSUBSCRIBER_H_
#define _ISTREAMSUBSCRIBER_H_

#include <string>
#include "Resolution.h"
#include "VideoOrientation.h"

namespace teevid_sdk
{
    struct IStreamSubscriber {
      // Each stream (including local one) is generating video frames. Each new frame is comming by this
      // callback method. It contains pointer to allocated buffer with data, it's size and stride, and a video orientation
      // Format of video is RGB24 by default and can be changed by MediaSettings and Configure()
      // Orientation values can be:
      // 0 - identity,
      // 1 - rotate 90 degrees right
      // 2 - rotate 180 degrees
      // 3 - rotate 90 degrees left
      virtual void OnVideoFrame (unsigned char *data, size_t size, size_t stride, VideoOrientation orientation) = 0;

      // Each remote stream is generating audio frames. In PCM format
      virtual void OnAudioFrame (unsigned char *data, size_t size, int channels, int bps) = 0;

      // Informs client application that resolution of remote video has changed
      virtual void OnVideoSizeChanged ( const std::string& participantId, const Resolution& res) = 0;
    };
}
#endif //_ISTREAMSUBSCRIBER_H_
