#ifndef TEEVID_SDK_ITEEVIDCLIENTOBSERVER_H
#define TEEVID_SDK_ITEEVIDCLIENTOBSERVER_H

#include <string>
#include <vector>
#include <map>

#include "ITeeVidConnection.h"
#include "RoomMode.h"
#include "Resolution.h"
#include "MuteAttributes.h"
#include "teevid_sdk/Participant.h"
#include "teevid_sdk/RoomParameters.h"
#include "teevid_sdk/StreamType.h"

namespace teevid_sdk
{
class ITeeVidClientObserver {
 public:
  // Raised when signaling process between client and room is done and media has started to publish
  // It has invitation token which can be used by others participant to join this room
  // Also it has streamId - it is ID of outgoing stream, it can be used for identifying video frames
  virtual void OnConnected (long streamId, const std::string& invitationToken) = 0;

  // Raised when room connection is successful (credentials are obtained) but signaling has not started yet
  virtual void OnRoomConnected(const RoomParameters& roomParameters) = 0;

  // Raised when new incoming stream has been created (another participant connected to the room)
  // If client connects to the room with already N existing participants, then client will receive
  // OnStreamAdded N times
  // It contains parameters for identifying stream: streamId, participantId (one participant may have several streams),
  // name and StreamType
  virtual void OnStreamAdded (long streamId, const std::string& name, const std::string& participantId, StreamType type, bool isLocal, int order, const Participant::Status& status) = 0;

  // Raised when one of observed streams has ended
  virtual void OnStreamRemoved (long streamId) = 0;

  // Informs application that it has been disconnected from the room
  virtual void OnDisconnected () = 0;

  // Informs application that room mode has changed
  virtual void OnRoomModeChanged (RoomMode mode) = 0;

  // Raised if some error occurred while connection
  virtual void OnError (const std::string& error) = 0;

  // Informs application that one of remote participant has left the room
  virtual void OnParticipantRemoved (const std::string& participantId) = 0;

  // Informs application that preview for this participant should be hide. It happens when he mute his audio and video
  virtual void OnParticipantMute (long streamId, bool audioMuted, bool videoMuted) = 0;

  // Requests PIN from application to enter the room (if it was not provided in ConnectTo())
  virtual int OnAccessPinRequested () = 0;

  // Server detected that active speaker has changed, applications may change layouts
  virtual void OnActiveSpeakerChanged (const std::map<long, int>& order) = 0;

  // Informs application that his mute attributes has changed by moderator
  virtual void OnMuteAttributesUpdated (const MuteAttributes& muteAttr) = 0;

  // Participant changed his mute attributes
  virtual void OnParticipantUpdated (const std::string& participantId, const MuteAttributes& muteAttr) = 0;

  // Informs client about allow/not allow to participant in the meeting after RiseHand request
  virtual void OnRaiseHandStatusUpdated (bool allowed) = 0;

  // Informs client that screen sharing has started
  virtual void OnScreenStarted () = 0;

  // Informs client that screen sharing has stopped due to some reason (normal or error)
  virtual void OnScreenStopped (const std::string& reason) = 0;
};
}

#endif //TEEVID_SDK_ITEEVIDCLIENTOBSERVER_H
