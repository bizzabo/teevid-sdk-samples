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

const int cAudioPublishSampleRate = 48000;
const int cAudioSubscribeSampleRate = 48000;
const int cMaxAudioSetsCount = 512;

const std::string cSpeechKey = "a9a8a2051e9c48c68842988d30b3b345";
const std::string cSubscriptionRegion = "chinaeast2";

const QString cSourceDir = "teevid-client-native";

// choose audio sample file
const QString cAudioSampleFile = "audio-sample-48000.wav";
//const QString cAudioSampleFile = "audio-sample-44100.wav";

InitialScreen::InitialScreen(QWidget *parent) : QWidget(parent), ui(new Ui::InitialScreen)
{
    _webcamPublishSettings.videoSettings.videoFormatType = VideoFormatType::kUYVY; // consider using I420 for 4K webcam
    _webcamPublishSettings.videoSettings.videoFps = cVideoFps;
    _webcamPublishSettings.videoSettings.allowMJPG = false;

//    _webcamPublishSettings.videoSettings.useWatermark = true;
//    _webcamPublishSettings.videoSettings.watermarkOptions.imageFileName = "/home/username/watermark.png";
//    _webcamPublishSettings.videoSettings.watermarkOptions.width = 270;
//    _webcamPublishSettings.videoSettings.watermarkOptions.height = 240;
//    _webcamPublishSettings.videoSettings.watermarkOptions.offset_x = 20;
//    _webcamPublishSettings.videoSettings.watermarkOptions.offset_y = 20;
//    _webcamPublishSettings.videoSettings.watermarkOptions.position = eWatermarkPosition::BottomLeft;
//    _webcamPublishSettings.videoSettings.watermarkOptions.alpha = 0.5;

//    _webcamPublishSettings.videoSettings.sourceWidth = 1920;
//    _webcamPublishSettings.videoSettings.sourceHeight = 1080;

    _webcamPublishSettings.audioSettings.audioChannels = kStereo;
    _webcamPublishSettings.audioSettings.audioBpsType = kS16LE;
    _webcamPublishSettings.audioSettings.audioSampleRate = cAudioPublishSampleRate;

    // _sourceMode is used inside InitUI, so initialize it's value before the InitUI call
    _webcamPublishSettings.sourceMode = kInternalSourceMode;

    _screenPublishSettings = _webcamPublishSettings;
    _screenPublishSettings.sourceMode = kInternalSourceMode;

    _speechSettings.sdk_speech_key = cSpeechKey;
    _speechSettings.sdk_subscribe_region = cSubscriptionRegion;

    InitUI();

    //_webcamPublishSettings.previewWindowId = ui->frameCallPart_Local->_subscribeSettings.previewWindowId;
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
    if(_screenPublishSettings.sourceMode == kExternalSourceMode)
    {
        _screenVideoMgr.Stop();
        _screenAudioMgr.Stop();
    }

    if(_webcamPublishSettings.sourceMode == kExternalSourceMode)
    {
        _webcamVideoMgr.Stop();
        _webcamAudioMgr.Stop();
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

        // TODO: set desired logging level
        // if you need no SDK signalling traces - set any level except DEBUG
        teeVidClient_->Initialize(validationToken, teevidServer, LogLevel::INFO, (ITeeVidClientObserver*)this);

        // TODO: uncomment this if default configuration is required
//        TeeVidSettings settings;
//        settings.media_settings = _webcamPublishSettings;
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
                         "QPushButton#btnScreenShare { background-color: transparent; background-image: url(:/screen_share_start.png); } "
                         "QPushButton#btnScreenShare[turn_on=\"true\"] { background-color: transparent; background-image: url(:/screen_share_stop.png); } "
                         "QPushButton#btnScreenShare:disabled { background-color: transparent; background-image: url(:/screen_share_start_disabled.png); } "
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

    ui->comboBoxMyLanguage->addItem("English", "en-US");
    ui->comboBoxMyLanguage->addItem("Chinese", "zh-CN");

    ui->comboBoxToLanguage->addItem("English", "en");
    ui->comboBoxToLanguage->addItem("Chinese", "zh");

    connect(_modeGroup, SIGNAL(buttonClicked(int)), this, SLOT(onModeSelected(int)));
    connect(ui->checkBoxCamera, SIGNAL(stateChanged(int)), this, SLOT(onCameraChecked(int)));
    connect(ui->checkBoxTranslate, SIGNAL(stateChanged(int)), this, SLOT(onTanslationChecked(int)));
    connect(ui->btnInvite, SIGNAL(pressed()), this, SLOT(onInvitePressed()));
    connect(ui->btnServerSimulation, SIGNAL(pressed()), this, SLOT(onServerSimulationPressed()));

    ui->btnMicrophone->setProperty("turn_on", true);
    ui->btnCamera->setProperty("turn_on", true);
    ui->btnScreenShare->setProperty("turn_on", false);

    ui->btnScreenShare->setEnabled(false);

    connect(ui->btnEndCall, SIGNAL(pressed()), this, SLOT(onBtnEndCallPressed()));
    connect(ui->btnMicrophone, SIGNAL(pressed()), this, SLOT(onBtnMicrophonePressed()));
    connect(ui->btnCamera, SIGNAL(pressed()), this, SLOT(onBtnCameraPressed()));
    connect(ui->btnScreenShare, SIGNAL(pressed()), this, SLOT(onBtnScreenSharePressed()));

    ui->listViewFriends->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->frameCallPart_Local->setAudioSampleRate(cAudioPublishSampleRate);

    // dummy participants names
    ui->frameCallPart_Local->setParticipantName("Local preview");
    ui->frameCallPart_1->setParticipantName("Participant #1");
    ui->frameCallPart_2->setParticipantName("Participant #2");
    ui->frameCallPart_3->setParticipantName("Participant #3");
    ui->frameCallPart_4->setParticipantName("Participant #4");

    // add all video items except local
    callItems_.push_back(ui->frameCallPart_1);
    callItems_.push_back(ui->frameCallPart_2);
    callItems_.push_back(ui->frameCallPart_3);
    callItems_.push_back(ui->frameCallPart_4);

    for (auto iterCallItem = callItems_.begin(); iterCallItem != callItems_.end(); ++iterCallItem)
    {
        CallItemVideoView* itemView = *iterCallItem;
        connect(itemView, SIGNAL(lowQualitySelected(long)), this, SLOT(onLowQualitySelected(long)));
        connect(itemView, SIGNAL(highQualitySelected(long)), this, SLOT(onHighQualitySelected(long)));
        connect(itemView, SIGNAL(transformSettingsUpdated(long)), this, SLOT(onTransformSettingsUpdated(long)));
    }

    connect(ui->frameCallPart_Local, SIGNAL(lowQualitySelected(long)), this, SLOT(onLowQualitySelected(long)));
    connect(ui->frameCallPart_Local, SIGNAL(highQualitySelected(long)), this, SLOT(onHighQualitySelected(long)));
    connect(ui->frameCallPart_Local, SIGNAL(transformSettingsUpdated(long)), this, SLOT(onTransformSettingsUpdated(long)));

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

    ui->frameCallPart_Local->setDirectVideoRendering(false);
    _webcamPublishSettings.previewWindowId = ui->frameCallPart_Local->_subscribeSettings.previewWindowId;

    ui->frameCallPart_1->setDirectVideoRendering(false);
    ui->frameCallPart_2->setDirectVideoRendering(false);
    ui->frameCallPart_3->setDirectVideoRendering(false);
    ui->frameCallPart_4->setDirectVideoRendering(false);

    if(_webcamPublishSettings.sourceMode == kExternalSourceMode)
    {
        connect(&_webcamVideoMgr, SIGNAL(publishVideoFrame(unsigned char*, long, int, bool)), this, SLOT(OnPublishVideoFrame(unsigned char*, long, int, bool)));
        connect(&_webcamVideoMgr, SIGNAL(internalVideoFrame(unsigned char*, long, int, bool)), this, SLOT(OnInternalVideoFrame(unsigned char*, long, int, bool)));
        connect(&_webcamVideoMgr, SIGNAL(videoError(QString, bool)), this, SLOT(OnVideoError(QString, bool)));
        connect(&_webcamVideoMgr, SIGNAL(videoStarted(int, int, bool)), this, SLOT(OnVideoStarted(int, int, bool)));
        connect(&_webcamVideoMgr, SIGNAL(capsUpdated(int, int, int, bool)), this, SLOT(OnVideoCapsUpdated(int, int, int, bool)));

        connect(&_webcamAudioMgr, SIGNAL(audioFrame(unsigned char*,long,bool)), this, SLOT(OnAudioFrame(unsigned char*,long,bool)));
        connect(&_webcamAudioMgr, SIGNAL(audioError(QString, bool)), this, SLOT(OnAudioError(QString, bool)));
    }

    if(_screenPublishSettings.sourceMode == kExternalSourceMode)
    {
        connect(&_screenVideoMgr, SIGNAL(publishVideoFrame(unsigned char*, long, int, bool)), this, SLOT(OnPublishVideoFrame(unsigned char*, long, int, bool)));
        connect(&_screenVideoMgr, SIGNAL(internalVideoFrame(unsigned char*, long, int, bool)), this, SLOT(OnInternalVideoFrame(unsigned char*, long, int, bool)));
        connect(&_screenVideoMgr, SIGNAL(videoError(QString, bool)), this, SLOT(OnVideoError(QString, bool)));
        connect(&_screenVideoMgr, SIGNAL(videoStarted(int, int, bool)), this, SLOT(OnVideoStarted(int, int, bool)));
        connect(&_screenVideoMgr, SIGNAL(capsUpdated(int, int, int, bool)), this, SLOT(OnVideoCapsUpdated(int, int, int, bool)));

        connect(&_screenAudioMgr, SIGNAL(audioFrame(unsigned char*, long, bool)), this, SLOT(OnAudioFrame(unsigned char*, long, bool)));
        connect(&_screenAudioMgr, SIGNAL(audioError(QString, bool)), this, SLOT(OnAudioError(QString, bool)));
    }
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

    ui->btnScreenShare->setEnabled(true);
}

void InitialScreen::ChangeVideoSource()
{
    if (teeVidClient_ && _webcamPublishSettings.sourceMode == kInternalSourceMode)
    {
        std::vector<SourceInfo> info = teeVidClient_->GetSources(eVideo);
        for (int i = 0; i < info.size (); i++)
        {
            std::cout << "Device: " << info[i].name << " - " <<  info[i].deviceId << "\n";
        }

        if(info.size() > 0){
            teeVidClient_->ChangeSource(eVideo, info[info.size() - 1].deviceId);
        }

        info = teeVidClient_->GetSources(eAudio);
        for (int i = 0; i < info.size (); i++)
        {
            std::cout << "Device: " << info[i].name << " - " <<  info[i].deviceId << "\n";
        }

        if(info.size() > 0){
            teeVidClient_->ChangeSource(eAudio, info[info.size() - 1].deviceId);
        }
    }
}

void InitialScreen::OnRoomConnected(const RoomParameters &roomParameters)
{
    std::vector<FileInfo> files = teeVidClient_->GetUserFileList();
    if (!files.empty())
    {
        for (auto iter = files.begin(); iter != files.end(); ++iter)
        {
            FileInfo info = *iter;
            qDebug() << "*** FILE INFO:";
            qDebug() << "filename:" << QString::fromStdString(info.fileName);
            qDebug() << "type:" << QString::fromStdString(info.type);
            qDebug() << "MIME type:" << QString::fromStdString(info.mimeType);
            qDebug() << "size:" << info.size;
            qDebug() << "created ts:" << info.createdTs;
            qDebug() << "owner:" << QString::fromStdString(info.owner);
            qDebug() << "filepath:" << QString::fromStdString(info.filepath);
            qDebug() << "URL:" << QString::fromStdString(info.downloadUrl) << "\n";
        }
    }
    else
    {
        qDebug() << "Looks like you don't have any files in your personal repository";
    }

    int width = roomParameters.video_resolution_.width;
    int height = roomParameters.video_resolution_.height;

    emit roomConnectReceived(width, height);

    _webcamPublishSettings.videoSettings.videoWidth = width;
    _webcamPublishSettings.videoSettings.videoHeight = height;

    UpdatePublishSettings();

    // BE AWARE
    // for now incoming video is set to be resized to the resolution from the room settings
    // to prevent resizing just comment (or remove these 2 lines below):
    ui->frameCallPart_Local->_subscribeSettings.videoSettings.videoWidth = width;
    ui->frameCallPart_Local->_subscribeSettings.videoSettings.videoHeight = height;

    for (auto iterCallItem = callItems_.begin(); iterCallItem != callItems_.end(); ++iterCallItem)
    {
        CallItemVideoView* itemView = *iterCallItem;
        itemView->_subscribeSettings.videoSettings.videoWidth = width;
        itemView->_subscribeSettings.videoSettings.videoHeight = height;
    }

    if (teeVidClient_)
    {
        _speechSettings.show_subtitles = ui->checkBoxTranslate->isChecked();
        _speechSettings.speak_lang = ui->comboBoxMyLanguage->currentData().toString().toStdString();
        _speechSettings.read_lang = ui->comboBoxToLanguage->currentData().toString().toStdString();

        TeeVidSettings settings;
        settings.webcam_media_settings = _webcamPublishSettings;
        settings.screen_media_settings = _screenPublishSettings;
        settings.speech_settings = _speechSettings;
        teeVidClient_->Configure(settings);
        ChangeVideoSource();
    }
}

void InitialScreen::OnStreamAdded (long streamId, const std::string& name, const std::string& participantId, StreamType type, bool isLocal, int order, const Participant::Status &status)
{
    if (isLocal)
    {
        // subscribe only to webcam video stream but not to screen sharing
        if (type == eWebCam)
        {
            ui->frameCallPart_Local->setStreamId(streamId);
            ui->frameCallPart_Local->setParticipantOrder(order);
            ui->frameCallPart_Local->setVideoFormat(kRGBA);
            ui->frameCallPart_Local->setAudioSampleRate(cAudioSubscribeSampleRate);

            teeVidClient_->Subscribe(streamId, ui->frameCallPart_Local->_subscribeSettings, ui->frameCallPart_Local);
        }
    }
    else
    {
        CallItemVideoView* callItem = GetVacantVideoView();
        if (teeVidClient_ && callItem)
        {
            callItem->setStreamId(streamId);
            callItem->setParticipantOrder(order);
            callItem->setVideoFormat(kRGBA);
            callItem->setAudioSampleRate(cAudioSubscribeSampleRate);

            teeVidClient_->Subscribe(streamId, callItem->_subscribeSettings, callItem);
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
    ui->btnScreenShare->setEnabled(false);
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
            teeVidClient_->Subscribe(streamId, callItem->_subscribeSettings, callItem);
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

void InitialScreen::OnScreenStarted()
{

}

void InitialScreen::OnScreenStopped(const std::string &reason)
{

}

void InitialScreen::OnRecognizedTextReceived(const std::string &participant, const std::string &original_text, const std::string &translated_text)
{
    qDebug() << "OnRecognizedTextReceived : participant:" << QString::fromStdString(participant)
             << "\noriginal text:" << QString::fromStdString(original_text) << "\ntranslated text:" << QString::fromStdString(translated_text);
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
    ui->comboBoxToLanguage->setEnabled(state == Qt::Checked);
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
        teeVidClient_->ConnectTo(room, user, password, accessPin, 0, sendAudio, sendVideo, this);
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

    if(_screenPublishSettings.sourceMode == kExternalSourceMode)
    {
        // Please note: this should be called AFTER disconnection from TeeVidClient !!!
        _screenVideoMgr.Stop();
        _screenAudioMgr.Stop();
    }

    if(_webcamPublishSettings.sourceMode == kExternalSourceMode)
    {
        // Please note: this should be called AFTER disconnection from TeeVidClient !!!
        _webcamVideoMgr.Stop();
        _webcamAudioMgr.Stop();
    }

    // this is a clean-up - just in case some artifacts were left
    ui->frameCallPart_Local->clear();
    for (auto iter = callItems_.begin(); iter != callItems_.end(); ++iter)
    {
        CallItemVideoView* callItem = *iter;
        callItem->clear();
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

void InitialScreen::onBtnScreenSharePressed()
{
    bool enabled = ui->btnScreenShare->property("turn_on").toBool();
    enabled = !enabled;

    ui->btnScreenShare->setProperty("turn_on", enabled);
    style()->polish(ui->btnScreenShare);

    // TODO: uncomment when SDK part is ready
    if (teeVidClient_)
    {
        if (enabled)
        {
            DesktopShareOptions options;
            options.fps = 5;
            if (_screenPublishSettings.sourceMode == kInternalSourceMode)
            {
                teeVidClient_->StartScreenSharing(_screenPublishSettings, options);
            }
            else
            {
                std::string videoFormat = GetVideoFormatName(_screenPublishSettings.videoSettings);
                if(_screenPublishSettings.sourceMode == kExternalSourceMode)
                {
                    _screenVideoMgr.Start(videoFormat, options);
                    _screenAudioMgr.Start(GetAudioFormatName(_webcamPublishSettings.audioSettings));
                }

                teeVidClient_->StartScreenSharing(_screenPublishSettings);
            }
        }
        else
        {
            teeVidClient_->StopScreenSharing();
        }
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
            std::string user = _connectParamsDialog->GetUser().toStdString();
            std::string password = _connectParamsDialog->GetPassword().toStdString();
            bool sendAudio = isMicrophoneOn();
            bool sendVideo = isCameraOn();
            teeVidClient_->ConnectTo(inviteParams.token_, inviteParams.room_, user, password, sendAudio, sendVideo, this);
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
    CallItemVideoView *callItem = nullptr;
    if (streamId == ui->frameCallPart_Local->getStreamId())
    {
        callItem = ui->frameCallPart_Local;
    }
    else
    {
        callItem = GetVideoViewById(streamId);
    }

    if (callItem != nullptr)
    {
        teeVidClient_->Subscribe(streamId, callItem->_subscribeSettings, callItem);
    }
}

void InitialScreen::onHighQualitySelected(long streamId)
{
    CallItemVideoView *callItem = nullptr;
    if (streamId == ui->frameCallPart_Local->getStreamId())
    {
        callItem = ui->frameCallPart_Local;
    }
    else
    {
        callItem = GetVideoViewById(streamId);
    }

    if (callItem != nullptr)
    {
        if (!teeVidClient_->Subscribe(streamId, callItem->_subscribeSettings, callItem))
        {
            QMessageBox mb(QMessageBox::Critical, "Error", "Failed to update window ID for nVidia sink element");
            mb.exec();
            return;
        }

        // clear small preview
        callItem->setImage(QImage());
    }
}

void InitialScreen::onTransformSettingsUpdated(long streamId)
{
    MediaSettings mediaSettings;
    if (streamId == ui->frameCallPart_Local->getStreamId())
    {
        mediaSettings = ui->frameCallPart_Local->_subscribeSettings;

        TeeVidSettings settings;
        settings.webcam_media_settings = _webcamPublishSettings;
        settings.screen_media_settings = _screenPublishSettings;
        settings.speech_settings = _speechSettings;

        teeVidClient_->Configure(settings);
    }
    else
    {
        CallItemVideoView* callItem = GetVideoViewById(streamId);
        mediaSettings = callItem->_subscribeSettings;
    }
    teeVidClient_->SetStreamMediaSettings(streamId, mediaSettings);
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

    ui->frameCallPart_Local->setExternalVideoSize(videoWidth, videoHeight);

    for (auto iterCallItem = callItems_.begin(); iterCallItem != callItems_.end(); ++iterCallItem)
    {
        CallItemVideoView* callItem = *iterCallItem;
        callItem->setExternalVideoSize(videoWidth, videoHeight);
    }

    std::string videoFormat = GetVideoFormatName(_webcamPublishSettings.videoSettings);
    if(_webcamPublishSettings.sourceMode == kExternalSourceMode)
    {
        _webcamVideoMgr.Start(videoWidth, videoHeight, videoFormat);
        _webcamAudioMgr.Start(cAudioFps,
                              _webcamPublishSettings.audioSettings.audioSampleRate,
                              _webcamPublishSettings.audioSettings.audioChannels,
                              GetAudioFormatName(_webcamPublishSettings.audioSettings));
    }
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

void InitialScreen::OnPublishVideoFrame(unsigned char *data, long size, int stride, bool screenSharing)
{
    if (teeVidClient_)
    {
        StreamType streamType = screenSharing ? eScreen : eWebCam;
        teeVidClient_->PutVideoFrame(data, size, stride, streamType);
    }
}

void InitialScreen::OnInternalVideoFrame(unsigned char *data, long size, int stride, bool screenSharing)
{
    Q_UNUSED(screenSharing);
    ui->frameCallPart_Local->OnVideoFrame(data, size, stride, VideoOrientation::kOrientationIdentity);
}

void InitialScreen::OnVideoError(QString message, bool screenSharing)
{
    QMessageBox mb(QMessageBox::Critical, "Video error", message);
    mb.exec();
}

void InitialScreen::OnVideoStarted(int width, int height, bool screenSharing)
{
    if (width == _webcamPublishSettings.videoSettings.videoWidth && height == _webcamPublishSettings.videoSettings.videoHeight)
    {
        // the settings are the same as original - no need to reconfigure
        return;
    }

    _webcamPublishSettings.videoSettings.videoWidth = width;
    _webcamPublishSettings.videoSettings.videoHeight = height;

    UpdatePublishSettings();

    if (ui->frameCallPart_Local->getStreamId() > 0)
    {
        // we already have our stream so we need to reconfigure it
        teeVidClient_->SetStreamMediaSettings(ui->frameCallPart_Local->getStreamId(), _webcamPublishSettings);
    }
    else
    {
        // not published yet, so just update settings
        TeeVidSettings settings;
        settings.webcam_media_settings = _webcamPublishSettings;
        settings.screen_media_settings = _screenPublishSettings;
        settings.speech_settings = _speechSettings;
        teeVidClient_->Configure(settings);
        ChangeVideoSource();
    }
}

void InitialScreen::OnVideoCapsUpdated(int width, int height, int fps, bool screenSharing)
{
    if (teeVidClient_ && _webcamPublishSettings.sourceMode == kExternalSourceMode)
    {
        teeVidClient_->UpdateVideoCaps(width, height, fps);
    }
}

void InitialScreen::OnAudioFrame(unsigned char *data, long size, bool screenSharing)
{
    if (teeVidClient_)
    {
        StreamType streamType = screenSharing ? eScreen : eWebCam;
        teeVidClient_->PutAudioFrame(data, size, streamType);
    }
}

void InitialScreen::OnAudioError(QString message, bool screenSharing)
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
        _audioParams.SetSampleRate(cAudioPublishSampleRate);
        _audioParams.SetFrameSampleRate(cAudioPublishSampleRate / cAudioFps);
        // failed to read sample file, generate dummy noize frames
        for (int i = 0; i < _dummyAudioFrameSetsCount; ++i)
        {
            _audioFrames.push_back(std::make_shared<AudioFrameData>(_audioParams, i));
        }
    }
    _silentAudioFrame = std::make_shared<AudioFrameData>(_audioParams);
}

void InitialScreen::UpdatePublishSettings()
{
    TransformSettings publishTransformSettings = ui->frameCallPart_Local->GetTransformSettings();

    if (publishTransformSettings.resizeWidth > 0 && publishTransformSettings.resizeHeight > 0)
    {
        _webcamPublishSettings.videoSettings.videoWidth = publishTransformSettings.resizeWidth;
        _webcamPublishSettings.videoSettings.videoHeight = publishTransformSettings.resizeHeight;
    }

    _webcamPublishSettings.videoSettings.cropSettings.left = publishTransformSettings.cropLeft;
    _webcamPublishSettings.videoSettings.cropSettings.top = publishTransformSettings.cropTop;
    _webcamPublishSettings.videoSettings.cropSettings.right = publishTransformSettings.cropRight;
    _webcamPublishSettings.videoSettings.cropSettings.bottom = publishTransformSettings.cropBottom;

    _webcamPublishSettings.videoSettings.flipMethod = (FlipMethod)publishTransformSettings.rotateType;
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

std::string InitialScreen::GetAudioFormatName(const AudioSettings &audioSettings)
{
    std::string format;
    switch (audioSettings.audioBpsType)
    {
    case kF64LE:
        format = "F64LE";
        break;
    case kF64BE:
        format = "F64BE";
        break;
    case kF32LE:
        format = "F32LE";
        break;
    case kF32BE:
        format = "F32BE";
        break;
    case kS32LE:
        format = "S32LE";
        break;
    case kS32BE:
        format = "S32BE";
        break;
    case kU32LE:
        format = "U32LE";
        break;
    case kU32BE:
        format = "U32BE";
        break;
    case kS24_32BE:
        format = "S24_32BE";
        break;
    case kU24_32LE:
        format = "U24_32LE";
        break;
    case kU24_32BE:
        format = "U24_32BE";
        break;
    case kS24LE:
        format = "S24LE";
        break;
    case kS24BE:
        format = "S24BE";
        break;
    case kU24LE:
        format = "U24LE";
        break;
    case kU24BE:
        format = "U24BE";
        break;
    case kS20LE:
        format = "S20LE";
        break;
    case kS20BE:
        format = "S20BE";
        break;
    case kU20LE:
        format = "U20LE";
        break;
    case kU20BE:
        format = "U20BE";
        break;
    case kS18LE:
        format = "S18LE";
        break;
    case kS18BE:
        format = "S18BE";
        break;
    case kU18LE:
        format = "kU18LE";
        break;
    case kU18BE:
        format = "U18BE";
        break;
    case kS16BE:
        format = "S16BE";
        break;
    case kU16LE:
        format = "U16LE";
        break;
    case kU16BE:
        format = "U16BE";
        break;
    case kS8:
        format = "S8";
        break;
    case kU8:
        format = "U8";
        break;
    case kS16LE:
    default:
        format = "S16LE";
        break;
    }

    return format;
}


void InitialScreen::OnVideoSourceFrame (unsigned char *data, size_t size, size_t stride)
{
    // comment this to remove a blu line on top of the video
    int index = 0;
    for (int j = 0; j < size / 3; j += 4)
    {
      if (j < size / 3)
      {
        data[j] = 0x00;
        data[j + 1] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
        data[j + 2] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
      }
      else if (j < size * 2 / 3)
      {
        data[j] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
        data[j + 1] = 0x00;
        data[j + 2] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
      }
      else
      {
        data[j] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
        data[j + 1] = (index < 16) ? 0xff - (index * 4) : 0xff - (32 - index) * 4;
        data[j + 2] = 0x00;
      }
    }
}

void InitialScreen::OnAudioSourceFrame (unsigned char *data, size_t size, int channels, int bps) {
// not implemented yet
}
