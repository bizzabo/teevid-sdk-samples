#ifndef _TEEVIDSETTINGS_H_
#define _TEEVIDSETTINGS_H_

#include <teevid_sdk/MediaSettings.h>

namespace teevid_sdk
{
    struct TeeVidSettings
    {
      MediaSettings webcam_media_settings;
      MediaSettings screen_media_settings;
    };
}
#endif //_TEEVIDSETTINGS_H_
