#include "InitialScreen.h"
#include "ui_InitialScreen.h"

#include "FriendItem.h"
#include "CallItemVideoView.h"
#include "ServerSimulationDialog.h"
#include "ConnectParamsDialog.h"
#include "InvitationManager.h"

#include <QButtonGroup>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <memory>

using namespace teevid_sdk;

const int cVideoFps = 30;
const int cVideoTimerInterval = 1000 / cVideoFps;
const int cDummyFrameSetsCount = 32;

const int cAudioSampleRate = 48000;

InitialScreen::InitialScreen(QWidget *parent) : QWidget(parent), ui(new Ui::InitialScreen)
{
    InitUI();
    //InitSDK();
}

InitialScreen::~InitialScreen()
{
    _dummyFramesTimer.stop();
    UnsubscribeFromVideo();

    if (teeVidClient_)
    {
        teeVidClient_->Disconnect();
    }

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
        teeVidClient_->Initialize(validationToken, teevidServer, (ITeeVidClientObserver*)this);

        TeeVidSettings settings;
        settings.media_settings.audioSettings.audioChannels = kStereo;
        settings.media_settings.audioSettings.audioBpsType = kS16LE;
        settings.media_settings.audioSettings.audioSampleRate = cAudioSampleRate;
        settings.media_settings.videoSettings.videoFormatType = VideoFormatType::kRGBA;
        settings.media_settings.videoSettings.videoFps = cVideoFps;
        teeVidClient_->Configure(settings);
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

    connect(this, SIGNAL(sdkOnConnectedRecieved(QString)), this, SLOT(OnSdkOnConnectedReceived(QString)));

    _connectParamsDialog = new ConnectParamsDialog(this);
    _connectParamsDialog->show();

    connect(_connectParamsDialog, SIGNAL(paramsApplied()), this, SLOT(onConnectParamsApplied()));
    connect(_connectParamsDialog, SIGNAL(paramsCancelled()), this, SLOT(onConnectParamsCancelled()));

    // TODO: should it be called only at first successful publish?
    GenerateDummyVideoFrames();
    GenerateDummyAudioFrames();

    _dummyFramesTimer.setInterval(cVideoTimerInterval);
    _dummyFramesTimer.setSingleShot(false);
    connect(&_dummyFramesTimer, SIGNAL(timeout()), this, SLOT(OnDummyFrameTimer()));
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
    return ui->btnMicrophone->property("enabled").toBool();
}

bool InitialScreen::isCameraOn() const
{
    return ui->btnCamera->property("enabled").toBool();
}

void InitialScreen::OnConnected (long streamId, const std::string& invitationToken)
{
    // invitation token receive notification is implemented via SIGNAL-SLOT connection because this logic is in different thread (not GUI)
    QString token = QString::fromStdString(invitationToken);
    emit sdkOnConnectedRecieved(token);
}

void InitialScreen::OnConnectionError (const std::string& )
{
}

void InitialScreen::OnStreamAdded (long streamId, const std::string& name, const std::string& participantId, int type, bool isLocal, int order, const Participant::Status &status)
{
    if (isLocal)
    {
        ui->frameCallPart_Local->setStreamId(streamId);
        if (ui->checkBoxLocalVideo->isChecked())
        {
            teeVidClient_->Subscribe(streamId, ui->frameCallPart_Local);
        }
    }
    else
    {
        CallItemVideoView* callItem = GetVacantVideoView();
        if (teeVidClient_ && callItem)
        {
            callItem->setStreamId(streamId);
            teeVidClient_->Subscribe(streamId, callItem);
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

void InitialScreen::OnDisconnected (){
}
void InitialScreen::OnRoomModeChanged (RoomMode ){
}
void InitialScreen::OnError (const std::string& ){
}
void InitialScreen::OnParticipantRemoved (const std::string& ){
}

void InitialScreen::OnParticipantMute(long streamId, bool audioMuted, bool videoMuted)
{
    CallItemVideoView* callItem = GetVideoViewById(streamId);
    if (!callItem)
    {
        callItem = ui->frameCallPart_Local;
        if (callItem->getStreamId() != streamId)
        {
            return;
        }
    }

    if (teeVidClient_)
    {
        if (audioMuted && videoMuted)
        {
            teeVidClient_->Unsubscribe(streamId);
        }
        else
        {
            teeVidClient_->Subscribe(streamId, callItem);
        }
    }

    callItem->setAudioMuted(audioMuted);
    callItem->setVideoMuted(videoMuted);
}

int InitialScreen::OnAccessPinRequested (){
    return 12345;
}
void InitialScreen::OnActiveSpeakerChanged (const std::map<long, int> &order){
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
         _dummyFramesTimer.start();
        teeVidClient_->ConnectTo(room, user, password, accessPin, 0);
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
    _dummyFramesTimer.stop();
    _dummyTimerIteration = 0;
    ui->textEditInvitationToken->clear();

    UnsubscribeFromVideo();
    if (teeVidClient_)
    {
        teeVidClient_->Disconnect();
    }

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
            _dummyFramesTimer.start();
            std::string user = _connectParamsDialog->GetUser().toStdString();
            std::string password = _connectParamsDialog->GetPassword().toStdString();
            teeVidClient_->ConnectTo(inviteParams.token_, inviteParams.room_, user, password);
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

void InitialScreen::OnDummyFrameTimer()
{
    if (_videoFrames.empty() || !_audioFrame)
        return;

    if (++_dummyTimerIteration == cDummyFrameSetsCount)
    {
        _dummyTimerIteration = 0;
    }
    if (teeVidClient_)
    {
        std::shared_ptr<VideoFrameData> video_frame = _videoFrames[_dummyTimerIteration];

        size_t stride = video_frame->GetWidth() * video_frame->GetBytesPerPixel();

        // play dummy local video
        if (ui->checkBoxLocalVideo->isChecked())
        {
            // only local video, local audoi should not be rendered
            ui->frameCallPart_Local->OnVideoFrame(video_frame->GetData(), video_frame->GetSize(), stride);
        }

        teeVidClient_->PutVideoFrame(video_frame->GetData(), video_frame->GetSize(), stride);
        teeVidClient_->PutAudioFrame(_audioFrame->GetData(), _audioFrame->GetSize());
    }
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


void InitialScreen::GenerateDummyVideoFrames()
{
    _videoFrames.clear();
    for (unsigned char i = 0; i < cDummyFrameSetsCount; ++i)
    {
        _videoFrames.push_back(std::make_shared<VideoFrameData>(i));
    }
}

void InitialScreen::GenerateDummyAudioFrames()
{
    _audioFrame = std::make_shared<AudioFrameData>();
}
