#ifndef _TEEVIDSETTINGS_H_
#define _TEEVIDSETTINGS_H_

#include <teevid_sdk/MediaSettings.h>
#include <teevid_sdk/SpeechSettings.h>

namespace teevid_sdk
{
    struct TeeVidSettings
    {
      MediaSettings webcam_media_settings;
      MediaSettings screen_media_settings;
      SpeechSettings speech_settings;
    };
}
#endif //_TEEVIDSETTINGS_H_
