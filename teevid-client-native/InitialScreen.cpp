#include "InitialScreen.h"
#include "ui_InitialScreen.h"

#include "FriendItem.h"
#include "CallItemVideoView.h"
#include "ServerSimulationDialog.h"
#include "ConnectParamsDialog.h"
#include "InvitationManager.h"

#include "teevid_sdk/RoomParameters.h"

#include <QButtonGroup>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QFileInfo>
#include <memory>

using namespace teevid_sdk;

const int cVideoFps = 30;
const int cVideoTimerInterval = 1000 / cVideoFps;
const int cDummyVideoFrameSetsCount = 32;

const int cAudioFps = 30;
const int cAudioTimerInterval = 1000 / cAudioFps;
const int cAudioIntervalSeconds = 20; // sound/silence duration
const int cAudioIntervals = 2; // sound and silence
const int cTimerCountReset = cAudioFps * cAudioIntervalSeconds * cAudioIntervals;

const int cAudioSampleRate = 48000;
const int cMaxAudioSetsCount = 512;

const QString cSourceDir = "teevid-client-native";

// choose audio sample file
const QString cAudioSampleFile = "audio-sample-48000.wav";
//const QString cAudioSampleFile = "audio-sample-44100.wav";

InitialScreen::InitialScreen(QWidget *parent) : QWidget(parent), ui(new Ui::InitialScreen)
{
    InitUI();
    //InitSDK();

    _publishVideoSettings.videoFormatType = VideoFormatType::kI420;

    _subscribeVideoSettings.videoFormatType = VideoFormatType::kRGBA;

    // values to prevent video resizing
    _subscribeVideoSettings.videoWidth = 0;
    _subscribeVideoSettings.videoHeight = 0;
}

InitialScreen::~InitialScreen()
{
    //_dummyVideoFramesTimer.stop();
    //_dummyAudioFramesTimer.stop();

    UnsubscribeFromVideo();

    if (teeVidClient_)
    {
        teeVidClient_->Disconnect();
    }

    // Please note: this should be called AFTER disconnection from TeeVidClient !!!
    _deviceVideoMgr.Stop();
    _deviceAudioMgr.Stop();

    delete ui;
}

void InitialScreen::InitSDK()
{
    if (nullptr != teeVidClient_)
    {
        // already initialized
        return;
    }

    std::string teevidServer = _connectParamsDialog->GetHost().toStdString();
    std::string validationToken = _connectParamsDialog->GetToken().toStdString();
    std::string room = _connectParamsDialog->GetRoom().toStdString();
    std::string user = _connectParamsDialog->GetUser().toStdString();

    if (teevidServer.empty() || validationToken.empty() || room.empty() || user.empty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Streaming component initialization has failed.\nPlease relaunch the app");
        mb.exec();
        return;
    }

    try
    {
        teeVidClient_ = TeeVidFactory::CreateTeeVidClient();

        // TODO: set desired logging level
        // if you need no SDK signalling traces - set any level except DEBUG
        teeVidClient_->Initialize(validationToken, teevidServer, LogLevel::DEBUG, (ITeeVidClientObserver*)this);

        // TODO: uncomment this if default configuration is required
//        TeeVidSettings settings;
//        settings.media_settings.audioSettings.audioChannels = kStereo;
//        settings.media_settings.audioSettings.audioBpsType = kS16LE;
//        settings.media_settings.audioSettings.audioSampleRate = cAudioSampleRate;
//        settings.media_settings.videoSettings.videoFormatType = VideoFormatType::kRGBA;
//        settings.media_settings.videoSettings.videoFps = cVideoFps;
//        teeVidClient_->Configure(settings);

    }
    catch (std::exception& e)
    {
        QString errorMsg = "Streaming component initialization has failed.\n" + QString::fromStdString(e.what());
        QMessageBox mb(QMessageBox::Critical, "Error", errorMsg);
        mb.exec();

        // TODO: do we need to quit the app here?
        exit(0);
    }
}

void InitialScreen::InitUI()
{
    ui->setupUi(this);

    QString styleSheet = "QFrame#frameUser, #frameUserContainer, #frameCallContainer { border: none; } "
                         "QFrame#frameSettings, #frameInvite { background-color: #009999; } "
                         "QPushButton#btnModeFriends, #btnModeCallHistory, #btnModeSettings { background-color: transparent; color: #ffffff; border: 1px solid #dddddd; } "
                         "QPushButton#btnModeFriends:checked, #btnModeCallHistory:pressed, #btnModeSettings:pressed { background-color: #99cc99; } "
                         "QPushButton#btnEndCall { background-color: transparent; background-image: url(:/end_call.png); } "
                         "QPushButton#btnMicrophone { background-color: transparent; background-image: url(:/microphone_off.png); } "
                         "QPushButton#btnMicrophone[turn_on=\"true\"] { background-color: transparent; background-image: url(:/microphone_on.png); } "
                         "QPushButton#btnCamera { background-color: transparent; background-image: url(:/camera_off.png); } "
                         "QPushButton#btnCamera[turn_on=\"true\"] { background-color: transparent; background-image: url(:/camera_on.png); } "
                         "QLabel#labelCamera, #labelTranslate, #labelLanguage { color: #000000; }"
                         "QLabel#labelCamera[checked=\"true\"], #labelTranslate[checked=\"true\"], #labelLanguage[checked=\"true\"] { color: #ffffff; }"
                         "QLabel#labelUserName { color: #000000; } ";
    setStyleSheet(styleSheet);

    _modeGroup = new QButtonGroup(this);
    _modeGroup->setExclusive(true);

    _modeGroup->addButton(ui->btnModeFriends, eModeButton::Friends);
    _modeGroup->addButton(ui->btnModeCallHistory, eModeButton::CallHistory);
    _modeGroup->addButton(ui->btnModeSettings, eModeButton::Settings);

    ui->btnModeFriends->setChecked(true);

    ui->comboBoxLanguage->addItem("English");
    ui->comboBoxLanguage->addItem("Chinese");

    connect(_modeGroup, SIGNAL(buttonClicked(int)), this, SLOT(onModeSelected(int)));
    connect(ui->checkBoxCamera, SIGNAL(stateChanged(int)), this, SLOT(onCameraChecked(int)));
    connect(ui->checkBoxTranslate, SIGNAL(stateChanged(int)), this, SLOT(onTanslationChecked(int)));
    connect(ui->btnInvite, SIGNAL(pressed()), this, SLOT(onInvitePressed()));
    connect(ui->btnServerSimulation, SIGNAL(pressed()), this, SLOT(onServerSimulationPressed()));

    ui->btnMicrophone->setProperty("turn_on", true);
    ui->btnCamera->setProperty("turn_on", true);

    connect(ui->btnEndCall, SIGNAL(pressed()), this, SLOT(onBtnEndCallPressed()));
    connect(ui->btnMicrophone, SIGNAL(pressed()), this, SLOT(onBtnMicrophonePressed()));
    connect(ui->btnCamera, SIGNAL(pressed()), this, SLOT(onBtnCameraPressed()));

    ui->listViewFriends->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->frameCallPart_Local->setAudioSampleRate(cAudioSampleRate);

    // add all video items except local
    callItems_.push_back(ui->frameCallPart_1);
    callItems_.push_back(ui->frameCallPart_2);
    callItems_.push_back(ui->frameCallPart_3);
    callItems_.push_back(ui->frameCallPart_4);

    for (auto iterCallItem = callItems_.begin(); iterCallItem != callItems_.end(); ++iterCallItem)
    {
        CallItemVideoView* itemView = *iterCallItem;
        itemView->setAudioSampleRate(cAudioSampleRate);
        connect(itemView, SIGNAL(lowQualitySelected(long)), this, SLOT(onLowQualitySelected(long)));
        connect(itemView, SIGNAL(highQualitySelected(long)), this, SLOT(onHighQualitySelected(long)));
    }

    ui->frameCallPart_Local->setAudioSampleRate(cAudioSampleRate);
    connect(ui->frameCallPart_Local, SIGNAL(lowQualitySelected(long)), this, SLOT(onLowQualitySelected(long)));
    connect(ui->frameCallPart_Local, SIGNAL(highQualitySelected(long)), this, SLOT(onHighQualitySelected(long)));

    ui->checkBoxLocalVideo->setChecked(true);
    connect(ui->checkBoxLocalVideo, SIGNAL(stateChanged(int)), this, SLOT(onDisplayLocalVideoChecked(int)));

    // signals emitted to itself in order to handle different thread messages
    connect(this, SIGNAL(roomConnectReceived(int, int)), this, SLOT(OnRoomConnectReceived(int, int)));
    connect(this, SIGNAL(sdkOnConnectedRecieved(QString)), this, SLOT(OnSdkOnConnectedReceived(QString)));
    connect(this, SIGNAL(roomErrorReceived(QString)), this, SLOT(OnRoomErrorReceived(QString)));

    _connectParamsDialog = new ConnectParamsDialog(this);
    _connectParamsDialog->show();

    connect(_connectParamsDialog, SIGNAL(paramsApplied()), this, SLOT(onConnectParamsApplied()));
    connect(_connectParamsDialog, SIGNAL(paramsCancelled()), this, SLOT(onConnectParamsCancelled()));

    // TODO: should it be called only at first successful publish?
    // currently only audio frames are generated here
    //GenerateDummyAudioFrames();

//    _dummyVideoFramesTimer.setInterval(cVideoTimerInterval);
//    _dummyVideoFramesTimer.setSingleShot(false);
//    connect(&_dummyVideoFramesTimer, SIGNAL(timeout()), this, SLOT(OnDummyVideoFrameTimer()));

//    _dummyAudioFramesTimer.setInterval(cAudioTimerInterval);
//    _dummyAudioFramesTimer.setSingleShot(false);
//    connect(&_dummyAudioFramesTimer, SIGNAL(timeout()), this, SLOT(OnDummyAudioFrameTimer()));

    connect(&_deviceVideoMgr, SIGNAL(publishVideoFrame(unsigned char*,long,int)), this, SLOT(OnPublishVideoFrame(unsigned char*,long,int)));
    connect(&_deviceVideoMgr, SIGNAL(internalVideoFrame(unsigned char*,long,int)), this, SLOT(OnInternalVideoFrame(unsigned char*,long,int)));
    connect(&_deviceVideoMgr, SIGNAL(videoError(QString)), this, SLOT(OnVideoError(QString)));
    connect(&_deviceVideoMgr, SIGNAL(videoStarted(int, int)), this, SLOT(OnVideoStarted(int,int)));

    connect(&_deviceAudioMgr, SIGNAL(audioFrame(unsigned char*,long)), this, SLOT(OnAudioFrame(unsigned char*,long)));
    connect(&_deviceAudioMgr, SIGNAL(audioError(QString)), this, SLOT(OnAudioError(QString)));
}

void InitialScreen::setFriendsData(std::vector<Contact> friends)
{
    _friends = friends;

    QStandardItemModel *model = new QStandardItemModel();
    model->setColumnCount(1);
    for (size_t i = 0; i < _friends.size(); ++i)
    {
        model->appendRow(new QStandardItem());
        model->setData(model->index(i, 0), _friends[i].name, Qt::UserRole);
        model->setData(model->index(i, 0), QSize(ui->listViewFriends->width(), 50), Qt::SizeHintRole);
    }

    ui->listViewFriends->setModel(model);

    for (size_t i = 0; i < _friends.size(); ++i)
    {
        FriendItem *item = new FriendItem();
        item->setName(_friends[i].name);
        item->setPhone(_friends[i].phone);
        ui->listViewFriends->setIndexWidget(ui->listViewFriends->model()->index(i, 0), item);
    }
}

bool InitialScreen::isMicrophoneOn() const
{
    return ui->btnMicrophone->property("turn_on").toBool();
}

bool InitialScreen::isCameraOn() const
{
    return ui->btnCamera->property("turn_on").toBool();
}

void InitialScreen::OnConnected (long streamId, const std::string& invitationToken)
{
    // invitation token receive notification is implemented via SIGNAL-SLOT connection because this logic is in different thread (not GUI)
    QString token = QString::fromStdString(invitationToken);
    emit sdkOnConnectedRecieved(token);
}

void InitialScreen::OnRoomConnected(const RoomParameters &roomParameters)
{
    int width = roomParameters.video_resolution_.width;
    int height = roomParameters.video_resolution_.height;

    emit roomConnectReceived(width, height);

    _publishVideoSettings.videoWidth = width;
    _publishVideoSettings.videoHeight = height;

    // BE AWARE
    // for now incoming video is set to be resized to the resolution from the room settings
    // to prevent resizing just comment (or remove these 2 lines below):
    _subscribeVideoSettings.videoWidth = width;
    _subscribeVideoSettings.videoHeight = height;

    if (teeVidClient_)
    {
        TeeVidSettings settings;
        settings.media_settings.audioSettings.audioChannels = kStereo;
        settings.media_settings.audioSettings.audioBpsType = kS16LE;
        settings.media_settings.audioSettings.audioSampleRate = cAudioSampleRate;
        settings.media_settings.videoSettings.videoFormatType = _publishVideoSettings.videoFormatType;
        settings.media_settings.videoSettings.videoWidth = width;
        settings.media_settings.videoSettings.videoHeight = height;
        settings.media_settings.videoSettings.videoFps = cVideoFps;
        teeVidClient_->Configure(settings);
    }
}

void InitialScreen::OnStreamAdded (long streamId, const std::string& name, const std::string& participantId, int type, bool isLocal, int order, const Participant::Status &status)
{
    if (isLocal)
    {
        ui->frameCallPart_Local->setStreamId(streamId);
        ui->frameCallPart_Local->setParticipantOrder(order);
    }
    else
    {
        CallItemVideoView* callItem = GetVacantVideoView();
        if (teeVidClient_ && callItem)
        {
            callItem->setStreamId(streamId);
            callItem->setParticipantOrder(order);
            callItem->setVideoFormat(kRGBA);

            teeVidClient_->Subscribe(streamId, _subscribeVideoSettings, callItem);
        }
    }
}

void InitialScreen::OnStreamRemoved(long streamId)
{
    CallItemVideoView* callItem = GetVideoViewById(streamId);
    if (teeVidClient_ && callItem)
    {
        teeVidClient_->Unsubscribe(streamId);
        callItem->setStreamId(0);
    }
}

void InitialScreen::OnDisconnected ()
{
    // TODO: add needed logic here
    qDebug() << "OnDisconnected";
}

void InitialScreen::OnRoomModeChanged (RoomMode ){
}

void InitialScreen::OnError (const std::string& error)
{
    // just emit this signal to handle it in the UI thread (show message box)
    emit roomErrorReceived(QString::fromStdString(error));

    try
    {
        if (teeVidClient_)
        {
            teeVidClient_->Disconnect();
        }
    }
    catch (std::exception& e)
    {
        qInfo() << "Room disconnection error: " << e.what();
    }
}

void InitialScreen::OnParticipantRemoved (const std::string& ){
}

void InitialScreen::OnParticipantMute(long streamId, bool audioMuted, bool videoMuted)
{
    bool isLocal = false;

    CallItemVideoView* callItem = GetVideoViewById(streamId);
    if (!callItem)
    {
        callItem = ui->frameCallPart_Local;
        if (callItem->getStreamId() != streamId)
        {
            return;
        }
        isLocal = true;
    }

    if (!isLocal && teeVidClient_)
    {
        if (audioMuted && videoMuted)
        {
            teeVidClient_->Unsubscribe(streamId);
        }
        else
        {
            teeVidClient_->Subscribe(streamId, _subscribeVideoSettings, callItem);
        }
    }

    callItem->setAudioMuted(audioMuted);
    callItem->setVideoMuted(videoMuted);
}

int InitialScreen::OnAccessPinRequested (){
    return 12345;
}
void InitialScreen::OnActiveSpeakerChanged (const std::map<long, int> &order){

    for (auto const& item : order)
    {
        CallItemVideoView* callItem = GetVideoViewById(item.first);
        if (!callItem)
        {
            callItem = ui->frameCallPart_Local;
            if (callItem->getStreamId() != item.first)
            {
                return;
            }
        }
        callItem->setParticipantOrder(item.second);
    }
}
void InitialScreen::OnMuteAttributesUpdated (const MuteAttributes& ){
}
void InitialScreen::OnParticipantUpdated (const std::string& , const MuteAttributes& ){
}
void InitialScreen::OnRaiseHandStatusUpdated (bool ){
}

void InitialScreen::onConnectParamsApplied()
{
    InitSDK();
}

void InitialScreen::onConnectParamsCancelled()
{
    exit(0);
}

void InitialScreen::onModeSelected(int modeId)
{
    ui->stackedWidget->setCurrentIndex(modeId);
}


void InitialScreen::onCameraChecked(int state)
{
    ui->labelCamera->setProperty("checked", state == Qt::Checked);
    style()->polish(ui->labelCamera);
}


void InitialScreen::onTanslationChecked(int state)
{
    ui->labelTranslate->setProperty("checked", state == Qt::Checked);
    style()->polish(ui->labelTranslate);
    ui->comboBoxLanguage->setEnabled(state == Qt::Checked);
}


void InitialScreen::onInvitePressed()
{
    if (nullptr == teeVidClient_)
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Streaming component initialization has failed.\nPlease relaunch the app");
        mb.exec();
        return;
    }

    QItemSelectionModel *model = dynamic_cast<QItemSelectionModel*>(ui->listViewFriends->selectionModel());
    if (!model)
        return;

    QModelIndexList selectedIndices = ui->listViewFriends->selectionModel()->selectedIndexes();
    if (selectedIndices.count() == 0)
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please select at least one contact");
        mb.exec();
        return;
    }

    QString msgText = "Invite contacts:\n";
    for (QModelIndex index : selectedIndices)
    {
        msgText.append(_friends[index.row()].name + " (" + _friends[index.row()].phone + ")\n");
    }
    QMessageBox mb(QMessageBox::Information, "Invitation", msgText);
    mb.exec();

    std::string room = _connectParamsDialog->GetRoom().toStdString();
    std::string user = _connectParamsDialog->GetUser().toStdString();
    std::string password = _connectParamsDialog->GetPassword().toStdString();
    int accessPin = _connectParamsDialog->GetAccessPin();
    try
    {
        bool sendAudio = isMicrophoneOn();
        bool sendVideo = isCameraOn();
        teeVidClient_->ConnectTo(room, user, password, accessPin, 0, sendAudio, sendVideo);
    }
    catch (std::exception& e)
    {
        QString errorMsg = "Room connection has failed.\n" + QString::fromStdString(e.what());
        QMessageBox mb(QMessageBox::Critical, "Error", errorMsg);
        mb.exec();
    }
}

void InitialScreen::onServerSimulationPressed()
{
    if (!_serverSimulationDialog)
    {
        _serverSimulationDialog = new ServerSimulationDialog(this);
        connect(_serverSimulationDialog, SIGNAL(roomSubmitted(const QString&, const QString&)), this, SLOT(onRoomSubmitted(const QString&, const QString&)));
    }
    _serverSimulationDialog->show();
}

void InitialScreen::onBtnEndCallPressed()
{
    //_dummyVideoFramesTimer.stop();
    //_dummyAudioFramesTimer.stop();
    _videoTimerIteration = 0;
    _audioTimerIteration = 0;
    ui->textEditInvitationToken->clear();

    UnsubscribeFromVideo();
    if (teeVidClient_)
    {
        teeVidClient_->Disconnect();
    }

    // Please note: this should be called AFTER disconnection from TeeVidClient !!!
    _deviceVideoMgr.Stop();
    _deviceAudioMgr.Stop();

    QMessageBox mb(QMessageBox::Information, "Invitation", "Call ended");
    mb.exec();
}

void InitialScreen::onBtnMicrophonePressed()
{
    // get the current state and invert it
    bool enabled = ui->btnMicrophone->property("turn_on").toBool();
    enabled = !enabled;

    ui->btnMicrophone->setProperty("turn_on", enabled);
    style()->polish(ui->btnMicrophone);

    if (teeVidClient_)
    {
        teeVidClient_->SendAudio(enabled);
    }

    ui->frameCallPart_Local->setAudioMuted(!enabled);
}

void InitialScreen::onBtnCameraPressed()
{
    // get the current state and invert it
    bool enabled = ui->btnCamera->property("turn_on").toBool();
    enabled = !enabled;

    ui->btnCamera->setProperty("turn_on", enabled);
    style()->polish(ui->btnCamera);

    if (teeVidClient_)
    {
        teeVidClient_->SendVideo(enabled);
    }

    ui->frameCallPart_Local->setVideoMuted(!enabled);
}

void InitialScreen::onRoomSubmitted(const QString &caller, const QString &invitationUrl)
{
    InvitationParams inviteParams = InvitationManager::GetInvitationParams(invitationUrl);
    std::string currentHost = _connectParamsDialog->GetHost().toStdString();

    // TODO: for now accept invitation only if it is for the same domain
    if (inviteParams.host_.compare(currentHost) != 0)
    {
        QString errorMsg = "The domain in the invitation link is different from currently connected.\nPlease relaunch the app and connec to an appropriate domain";
        QMessageBox mb(QMessageBox::Critical, "Invitation failed", errorMsg);
        mb.exec();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Incoming call from " + caller);
    QAbstractButton* btnAccept = msgBox.addButton("Accept", QMessageBox::YesRole);
    msgBox.addButton("Decline", QMessageBox::NoRole);

    msgBox.exec();

    if (msgBox.clickedButton() == btnAccept && teeVidClient_)
    {
        try
        {
            std::string user = _connectParamsDialog->GetUser().toStdString();
            std::string password = _connectParamsDialog->GetPassword().toStdString();
            bool sendAudio = isMicrophoneOn();
            bool sendVideo = isCameraOn();
            teeVidClient_->ConnectTo(inviteParams.token_, inviteParams.room_, user, password, sendAudio, sendVideo);
        }
        catch (std::exception& e)
        {
            QString errorMsg = "Room connection has failed.\n" + QString::fromStdString(e.what());
            QMessageBox mb(QMessageBox::Critical, "Error", errorMsg);
            mb.exec();
        }
    }
}

void InitialScreen::onLowQualitySelected(long streamId)
{
}

void InitialScreen::onHighQualitySelected(long streamId)
{
}

void InitialScreen::onDisplayLocalVideoChecked(int state)
{
    bool showVideo = (state == Qt::Checked);
    ui->frameLocalVideo->setVisible(showVideo);
}

void InitialScreen::OnRoomConnectReceived(int videoWidth, int videoHeight)
{
    //GenerateDummyVideoFrames(videoWidth, videoHeight);
    //_dummyVideoFramesTimer.start();
    //_dummyAudioFramesTimer.start();

    std::string videoFormat = GetVideoFormatName(_publishVideoSettings);
    _deviceVideoMgr.Start(videoWidth, videoHeight, videoFormat);
    _deviceAudioMgr.Start(cAudioFps, cAudioSampleRate, 2, "S16LE");
}

void InitialScreen::OnSdkOnConnectedReceived(QString token)
{
    // empty invitation token means no invitation URL can be generated
    // set this field empty
    QString invitationUrl;
    if (!token.isEmpty())
    {
        InvitationParams inviteParams;
        inviteParams.host_ = _connectParamsDialog->GetHost().toStdString();
        inviteParams.room_ = _connectParamsDialog->GetRoom().toStdString();
        inviteParams.token_ = token.toStdString();
        invitationUrl = InvitationManager::MakeInvitationUrl(inviteParams);
    }
    ui->textEditInvitationToken->setPlainText(invitationUrl);
}

void InitialScreen::OnRoomErrorReceived(QString errorMessage)
{
    QMessageBox mb(QMessageBox::Critical, "Room Connection Error", errorMessage);
    mb.exec();
}

void InitialScreen::OnDummyVideoFrameTimer()
{
    if (_videoFrames.empty())
        return;

    // timer is common for video and audio so reset by the biggest value
    if (++_videoTimerIteration == cDummyVideoFrameSetsCount)
    {
        _videoTimerIteration = 0;
    }
    if (teeVidClient_)
    {
        std::shared_ptr<VideoFrameData> video_frame = _videoFrames[_videoTimerIteration];
        size_t stride = video_frame->GetWidth() * video_frame->GetBytesPerPixel();

        // play dummy local video
        if (ui->checkBoxLocalVideo->isChecked())
        {
            // only local video, local audoi should not be rendered
            ui->frameCallPart_Local->OnVideoFrame(video_frame->GetData(), video_frame->GetSize(), stride, VideoOrientation::kOrientationIdentity);
        }

        teeVidClient_->PutVideoFrame(video_frame->GetData(), video_frame->GetSize(), stride);
    }
}

void InitialScreen::OnDummyAudioFrameTimer()
{
    if (_audioFrames.empty() || !_silentAudioFrame)
        return;

    // timer is common for video and audio so reset by the biggest value
    if (++_audioTimerIteration == cTimerCountReset)
    {
        _audioTimerIteration = 0;
    }
    if (teeVidClient_)
    {
        // audio has 2 intervals - one is sound, another is silent
        bool sound = (_audioTimerIteration < cAudioFps * cAudioIntervalSeconds);
        std::shared_ptr<AudioFrameData> audio_frame = sound ? _audioFrames[_audioTimerIteration % _dummyAudioFrameSetsCount] : _silentAudioFrame;

        //ui->frameCallPart_Local->OnAudioFrame(audio_frame->GetData(), audio_frame->GetSize(), 2, 16);

        teeVidClient_->PutAudioFrame(audio_frame->GetData(), audio_frame->GetSize());
    }
}

void InitialScreen::OnPublishVideoFrame(unsigned char *data, long size, int stride)
{
    if (teeVidClient_)
    {
        teeVidClient_->PutVideoFrame(data, size, stride);
    }
}

void InitialScreen::OnInternalVideoFrame(unsigned char *data, long size, int stride)
{
    ui->frameCallPart_Local->OnVideoFrame(data, size, stride, VideoOrientation::kOrientationIdentity);
}

void InitialScreen::OnVideoError(QString message)
{
    QMessageBox mb(QMessageBox::Critical, "Video error", message);
    mb.exec();
}

void InitialScreen::OnVideoStarted(int width, int height)
{
    if (width == _publishVideoSettings.videoWidth && height == _publishVideoSettings.videoHeight)
    {
        // the settings are the same as original - no need to reconfigure
        return;
    }

    _publishVideoSettings.videoWidth = width;
    _publishVideoSettings.videoHeight = height;
    if (ui->frameCallPart_Local->getStreamId() > 0)
    {
        // we already have our stream so we need to reconfigure it
        teeVidClient_->SetStreamVideoSettings(ui->frameCallPart_Local->getStreamId(), _publishVideoSettings);
    }
    else
    {
        // not published yet, so just update settings
        TeeVidSettings settings;
        settings.media_settings.audioSettings.audioChannels = kStereo;
        settings.media_settings.audioSettings.audioBpsType = kS16LE;
        settings.media_settings.audioSettings.audioSampleRate = cAudioSampleRate;
        settings.media_settings.videoSettings.videoFormatType = _publishVideoSettings.videoFormatType;
        settings.media_settings.videoSettings.videoWidth = _publishVideoSettings.videoWidth;
        settings.media_settings.videoSettings.videoHeight = _publishVideoSettings.videoHeight;
        settings.media_settings.videoSettings.videoFps = cVideoFps;
        teeVidClient_->Configure(settings);
    }
}

void InitialScreen::OnAudioFrame(unsigned char *data, long size)
{
    if (teeVidClient_)
    {
        teeVidClient_->PutAudioFrame(data, size);
    }
}

void InitialScreen::OnAudioError(QString message)
{
    QMessageBox mb(QMessageBox::Critical, "Audio error", message);
    mb.exec();
}

void InitialScreen::UnsubscribeFromVideo()
{
    if (!teeVidClient_)
        return;

    for (auto iter = callItems_.begin(); iter != callItems_.end(); ++iter)
    {
        CallItemVideoView* callItem = *iter;
        if (callItem && callItem->getStreamId() > 0)
        {
            teeVidClient_->Unsubscribe(callItem->getStreamId());
            callItem->setStreamId(0);
        }
    }

    // unsubscribe from local video
    teeVidClient_->Unsubscribe(ui->frameCallPart_Local->getStreamId());
    ui->frameCallPart_Local->setStreamId(0);
}

CallItemVideoView* InitialScreen::GetVacantVideoView() const
{
    CallItemVideoView* view = nullptr;
    for (auto iter = callItems_.begin(); iter != callItems_.end(); ++iter)
    {
        CallItemVideoView* callItem = *iter;
        if (callItem && (callItem->getStreamId() == 0))
        {
            view = callItem;
            break;
        }
    }
    return view;
}

CallItemVideoView* InitialScreen::GetVideoViewById(long streamId) const
{
    CallItemVideoView* view = nullptr;
    for (auto iter = callItems_.begin(); iter != callItems_.end(); ++iter)
    {
        CallItemVideoView* callItem = *iter;
        if (callItem && callItem->getStreamId() == streamId)
        {
            view = callItem;
            break;
        }
    }
    return view;
}


void InitialScreen::GenerateDummyVideoFrames(int width, int height)
{
    _videoFrames.clear();
    for (unsigned char i = 0; i < cDummyVideoFrameSetsCount; ++i)
    {
        _videoFrames.push_back(std::make_shared<VideoFrameData>(i, width, height));
    }
}

void InitialScreen::GenerateDummyAudioFrames()
{
    _audioFrames.clear();

    QString audioFileFullName = QCoreApplication::applicationDirPath();
    int lastSlashPos = audioFileFullName.lastIndexOf("/");
    audioFileFullName = audioFileFullName.left(lastSlashPos + 1) + cSourceDir + "/" + cAudioSampleFile;
    QFileInfo audioFileInfo(audioFileFullName);
    bool isAudioFileValid = false;
    if (audioFileInfo.exists())
    {
        // try to read WAV file
        int audioFileSize = audioFileInfo.size();
        _dummyAudioFrameSetsCount = (audioFileSize - _audioParams.GetWavFileDataOffset()) / _audioParams.GetDataChunkSize();
        if (_dummyAudioFrameSetsCount > cMaxAudioSetsCount)
        {
            // restrict (nearly 10 seconds) if file is too large
            _dummyAudioFrameSetsCount = cMaxAudioSetsCount;
        }

        QFile file(audioFileFullName);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray buffer = file.readAll();

            // retrive the file bitrate and calculate data chunk size
            int sampleRate = (((int)buffer.at(25) & 0xff) << 8) + ((int)buffer.at(24) & 0xff);
            _audioParams.SetSampleRate(sampleRate);
            _audioParams.SetFrameSampleRate(sampleRate / cAudioFps);

            for (int i = 0; i < _dummyAudioFrameSetsCount; ++i)
            {
                _audioFrames.push_back(std::make_shared<AudioFrameData>(_audioParams, buffer, i));
            }
            isAudioFileValid = true;
        }
    }

    if (!isAudioFileValid)
    {
        _audioParams.SetSampleRate(cAudioSampleRate);
        _audioParams.SetFrameSampleRate(cAudioSampleRate / cAudioFps);
        // failed to read sample file, generate dummy noize frames
        for (int i = 0; i < _dummyAudioFrameSetsCount; ++i)
        {
            _audioFrames.push_back(std::make_shared<AudioFrameData>(_audioParams, i));
        }
    }
    _silentAudioFrame = std::make_shared<AudioFrameData>(_audioParams);
}

std::string InitialScreen::GetVideoFormatName (const VideoSettings& videoSettings)
{
  std::string format;
  switch (videoSettings.videoFormatType)
  {
    case kBGRx:
      format = "BGRx";
      break;
    case kGRAY8:
      format = "GRAY8";
      break;
    case kUYVY:
      format = "UYVY";
      break;
    case kYUY2:
      format = "YUY2";
      break;
    case kYVYU:
      format = "YVYU";
      break;
    case kI420:
      format = "I420";
      break;
    case kI420_10LE:
      format = "I420_10LE";
      break;
    case kNV12:
      format = "NV12";
      break;
    case kRGBA:
    default:
      format = "RGBA";
      break;
  }

  return format;
}
