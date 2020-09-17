//
// Created by lena on 17.09.20.
//

#ifndef _VIDEOORIENTATION_H_
#define _VIDEOORIENTATION_H_
namespace teevid_sdk
{
  enum VideoOrientation {
      kOrientationIdentity = 0,
      kOrientation90R = 1,
      kOrientation180 = 2,
      kOrientation90L = 3,
      kOrientationHoriz = 4,
      kOrientationVertic = 5,
      kOrientationUL_LR = 6,
      kOrientationUR_LL = 7

  };
}
#endif //_VIDEOORIENTATION_H_
