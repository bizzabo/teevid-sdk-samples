### How to switch between internal and external pipeline usage

#   For now we have 2 modes of video/audio pipeline usage:
    *   INTERNAL - means the sending pipelines are inside the SDK. DeviceVideoManager and DeviceAudioManager classes are NOT used;
    *   EXTERNAL - means we build pipelines in the demo app as before.

1.  By default we use INTERNAL mode.
2.  In order to switch to EXTERNAL mode you need to change in the demo app code - InitialScreen.cpp, line 46. You need to change from:
    ```
    _sourceMode = kInternalSourceMode;
    ```
    to
    ```
    _sourceMode = kExternalSourceMode;
    ```
#   Please note: there's no option to have one pipeline (e.g. video) as internal and another one as external. The mode changes for both of them.
