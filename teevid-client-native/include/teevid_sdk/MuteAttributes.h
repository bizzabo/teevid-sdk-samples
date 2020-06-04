//
// Created by root on 21.05.20.
//

#ifndef _MUTEATTRIBUTES_H_
#define _MUTEATTRIBUTES_H_
namespace teevid_sdk
{
    struct MuteAttributes
    {
      struct MuteStatus
      {
        bool muted = false;
        bool mutedByModerator = false;
      };

      MuteStatus audio;
      MuteStatus video;
    };
}
#endif //_MUTEATTRIBUTES_H_
