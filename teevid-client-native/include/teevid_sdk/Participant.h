#ifndef TEEVID_SDK_PARTICIPANT_H
#define TEEVID_SDK_PARTICIPANT_H
#include <string>
#include <vector>

namespace teevid_sdk
{
struct Participant
{
  struct Status
  {
    struct Devices
    {
      int audio = 0;
      int video = 0;
    };

    struct UnmuteRequest
    {
      bool audio = false;
      bool video = false;
    };

    Devices devices;
    UnmuteRequest unmuteRequest;
    bool handRaised;
  };

  struct Streams
  {
    long screenStreamId = 0;
    long mediaStreamId = 0;  // 626585127349684500
  };

  std::string _id;       // "5ef44676b5662d0010039278"
  std::string name;      // "somename"
  std::string role;      // "Participant"
  Status status;
  Streams streams;
  std::string ip;
  int order = -1;
  std::string city;       // "Odessa"
  std::string country;    // "UA"
  std::string os;         // "Windows 10.0"
  std::string browser;    // "Chrome/83.0.4103.106"
  std::string type;       // "web"
  std::vector<double> ll; // 50.45, 30.5233
  std::string avatar;
};
}

#endif //TEEVID_SDK_PARTICIPANT_H
