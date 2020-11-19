//
// Created by root on 20.05.20.
//

#ifndef _LOGLEVEL_H_
#define _LOGLEVEL_H_

namespace teevid_sdk
{
    typedef enum
    {
      ERROR = 0,
      WARNING = 1,
      INFO = 2,
      DEBUG = 3,
      TRACE = 4
    } LogLevel;
}

#endif //_LOGLEVEL_H_
