#ifndef INITIALSCREEN_H
#define INITIALSCREEN_H

#include <QWidget>
#include <QTimer>
#include <vector>

#include "Contact.h"
#include "VideoFrameData.h"
#include "AudioFrameData.h"
#include "AudioParams.h"
#include "DeviceVideoManager.h"
#include "DeviceAudioManager.h"
#include "teevid_sdk/ITeeVidClient.h"

class QButtonGroup;
class ServerSimulationDialog;
class CallItemVideoView;
class ConnectParamsDialog;

using namespace teevid_sdk;
namespace Ui {
class InitialScreen;
}

class InitialScreen : public QWidget, ITeeVidClientObserver, IFrameModifier
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
    void OnRoomConnected(const RoomParameters &roomParameters) override;
    void OnStreamAdded (long streamId, const std::string& name, const std::string& participantId, StreamType type, bool isLocal, int order, const Participant::Status& status) override;
    void OnStreamRemoved(long streamId) override;
    void OnDisconnected () override;
    void OnRoomModeChanged (RoomMode mode) override;
    void OnError (const std::string& error) override;
    void OnParticipantRemoved (const std::string& participantId) override;
    void OnParticipantMute (long streamId, bool audioMuted, bool videoMuted) override;
    int OnAccessPinRequested () override;
    void OnActiveSpeakerChanged (const std::map<long, int>& order) override;
    void OnMuteAttributesUpdated (const MuteAttributes& muteAttr) override;
    void OnParticipantUpdated (const std::string& participantId, const MuteAttributes& muteAttr) override;
    void OnRaiseHandStatusUpdated (bool allowed) override;
    void OnScreenStarted () override;
    void OnScreenStopped (const std::string& reason) override;

    // IFrameModifier
    void OnVideoSourceFrame (unsigned char *data, size_t size, size_t stride) override;
    void OnAudioSourceFrame (unsigned char *data, size_t size, int channels, int bps) override;
signals:
    void roomConnectReceived(int videoWidth, int videoHeight);
    void sdkOnConnectedRecieved(QString token);
    void roomErrorReceived(QString errorMessage);

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
    void onBtnScreenSharePressed();

    void onRoomSubmitted(const QString& caller, const QString& invitationUrl);

    void onLowQualitySelected(long streamId);
    void onHighQualitySelected(long streamId);
    void onTransformSettingsUpdated(long streamId);

    void onDisplayLocalVideoChecked(int state);

    void OnRoomConnectReceived(int videoWidth, int videoHeight);
    void OnSdkOnConnectedReceived(QString token);
    void OnRoomErrorReceived(QString errorMessage);

    void OnDummyVideoFrameTimer();
    void OnDummyAudioFrameTimer();

    void OnPublishVideoFrame(unsigned char* data, long size, int stride, bool screenSharing);
    void OnInternalVideoFrame(unsigned char* data, long size, int stride, bool screenSharing);
    void OnVideoError(QString message, bool screenSharing);
    void OnVideoStarted(int width, int height, bool screenSharing);
    void OnVideoCapsUpdated(int width, int height, int fps, bool screenSharing);

    void OnAudioFrame(unsigned char* data, long size, bool screenSharing);
    void OnAudioError(QString message, bool screenSharing);

protected:
    void UnsubscribeFromVideo();

    CallItemVideoView* GetVacantVideoView() const;
    CallItemVideoView* GetVideoViewById(long streamId) const;
    void GenerateDummyVideoFrames(int width, int height);
    void GenerateDummyAudioFrames();

    void UpdatePublishSettings();

private:
    void InitSDK();
    void InitUI();

    std::string GetVideoFormatName(const VideoSettings& videoSettings);
    std::string GetAudioFormatName(const AudioSettings& audioSettings);

    void ChangeVideoSource();

private:
    Ui::InitialScreen *ui;

    QButtonGroup* _modeGroup = nullptr;
    std::vector<Contact> _friends;
    ITeeVidClientPtr teeVidClient_ = nullptr;
    std::vector<CallItemVideoView*> callItems_;

    ServerSimulationDialog* _serverSimulationDialog = nullptr;
    ConnectParamsDialog* _connectParamsDialog = nullptr;

    // dummy frames
    std::vector<std::shared_ptr<VideoFrameData>> _videoFrames;
    std::vector<std::shared_ptr<AudioFrameData>> _audioFrames;
    std::shared_ptr<AudioFrameData> _silentAudioFrame;
    QTimer _dummyVideoFramesTimer;
    int _videoTimerIteration = 0;
    int _dummyAudioFrameSetsCount = 32;

    QTimer _dummyAudioFramesTimer;
    int _audioTimerIteration = 0;
    AudioParams _audioParams;
    DeviceVideoManager _webcamVideoMgr;
    DeviceAudioManager _webcamAudioMgr;
    DeviceVideoManager _screenVideoMgr;
    DeviceAudioManager _screenAudioMgr;

    MediaSettings _webcamPublishSettings;
    MediaSettings _screenPublishSettings;
};

#endif // INITIALSCREEN_H
