#ifndef _IFRAMEMODIFIER_H_
#define _IFRAMEMODIFIER_H_

namespace teevid_sdk
{
    class IFrameModifier {
     public:
      virtual void OnAudioSourceFrame (unsigned char *data, size_t size, int channels, int bps) = 0;
      virtual void OnVideoSourceFrame (unsigned char *data, size_t size, size_t stride) = 0;
    };
}
#endif //_IFRAMEMODIFIER_H_
