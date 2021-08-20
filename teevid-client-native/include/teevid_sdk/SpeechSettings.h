#ifndef TEEVID_SDK_SPEECHSETTINGS_H
#define TEEVID_SDK_SPEECHSETTINGS_H

namespace teevid_sdk
{
  struct SpeechSettings
  {
    // speech SDK initialization key
    std::string sdk_speech_key;
    std::string sdk_subscribe_region;

    bool show_subtitles = false;

    // language used for translation for current user
    // "simple" language format (e.g. "de", "fr")
    std::string read_lang;

    // language current user speaks
    // language and region format (e.g. "en-US")
    std::string speak_lang;
  };
}

#endif //TEEVID_SDK_SPEECHSETTINGS_H
