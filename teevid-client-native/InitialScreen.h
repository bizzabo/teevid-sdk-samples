#ifndef INITIALSCREEN_H
#define INITIALSCREEN_H

#include <QWidget>
#include <vector>

#include "Contact.h"
#include "teevid_sdk/ITeeVidClient.h"

class QButtonGroup;
class ServerSimulationDialog;
class CallItemVideoView;
class ConnectParamsDialog;

using namespace teevid_sdk;
namespace Ui {
class InitialScreen;
}

class InitialScreen : public QWidget, ITeeVidClientObserver
{
    Q_OBJECT

    enum eModeButton
    {
        Friends = 0,
        CallHistory= 1,
        Settings = 2
    };

public:
    InitialScreen(QWidget *parent = 0);
    ~InitialScreen();

    void setFriendsData(std::vector<Contact> friends);

    bool isMicrophoneOn() const;
    bool isCameraOn() const;

    void OnConnected (long streamId, const std::string& invitationToken) override;
    void OnConnectionError (const std::string& reason) override;
    void OnStreamAdded (long streamId, const std::string& participantId, const std::string& name, int type, bool isOwn) override;
    void OnStreamRemoved(long streamId) override;
    void OnDisconnected () override;
    void OnRoomModeChanged (RoomMode mode) override;
    void OnError (const std::string& error) override;
    void OnParticipantRemoved (const std::string& participantId) override;
    void OnParticipantVideoViewRemoved (const std::string& participantId) override;
    int OnAccessPinRequested () override;
    void OnActiveSpeakerChanged (const std::string& streamId, const std::string& participantId) override;
    void OnMuteAttributesUpdated (const MuteAttributes& muteAttr) override;
    void OnParticipantUpdated (const std::string& participantId, const MuteAttributes& muteAttr) override;
    void OnRaiseHandStatusUpdated (bool allowed) override;

protected slots:
    void onConnectParamsApplied();
    void onConnectParamsCancelled();

    void onModeSelected(int modeId);
    void onCameraChecked(int state);
    void onTanslationChecked(int state);
    void onInvitePressed();
    void onServerSimulationPressed();

    void onBtnEndCallPressed();
    void onBtnMicrophonePressed();
    void onBtnCameraPressed();

    void onRoomNameSubmitted(const std::string& roomId);

    void onLowQualitySelected(long streamId);
    void onHighQualitySelected(long streamId);

    void onDisplayLocalVideoChecked(int state);

protected:
    void UnsubscribeFromVideo();

    CallItemVideoView* GetVacantVideoView() const;
    CallItemVideoView* GetVideoViewById(long streamId) const;

private:
    void InitSDK();
    void InitUI();

private:
    Ui::InitialScreen *ui;

    QButtonGroup* _modeGroup = nullptr;
    std::vector<Contact> _friends;
    ITeeVidClientPtr teeVidClient_ = nullptr;
    std::vector<CallItemVideoView*> callItems_;

    ServerSimulationDialog* _serverSimulationDialog = nullptr;
    ConnectParamsDialog* _connectParamsDialog = nullptr;
};

#endif // INITIALSCREEN_H
