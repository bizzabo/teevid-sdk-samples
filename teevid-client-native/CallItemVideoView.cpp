#include "CallItemVideoView.h"
#include "ui_CallItemVideoView.h"

#include "VideoQualityDialog.h"
#include "TransformSettingsDialog.h"
#include "ExternalVideoContainer.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QDateTime>
#include <QDesktopWidget>
#include <qmath.h>

const int cDummyVideoFrameWidth = 1920;
const int cDummyVideoFrameHeight = 1080;
const int cAudioSubscribeSampleRate = 48000;

CallItemVideoView::CallItemVideoView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CallItemVideoView),
    _audioInitialized(false),
    _directPreview(false)
{
    ui->setupUi(this);

    ui->labelParticipant->clear();
    ui->labelSize->clear();
    ui->labelAudio->clear();
    ui->labelVideo->clear();
    ui->labelOrder->clear();

    // signal-slot connection to itself because data is received from different thread
    connect(this, SIGNAL(imageUpdated()), this, SLOT(onImageUpdated()));
    connect(this, SIGNAL(audioStarted(int,int)), this, SLOT(onAudioStarted(int,int)));
    connect(ui->btnTransformSettings, SIGNAL(pressed()), this, SLOT(onTransformSettingsPressed()));

    QRect rec = QApplication::desktop()->screenGeometry();
    _screenWidth = rec.width();
    _screenHeight = rec.height();

    _subscribeSettings.videoSettings.videoFormatType = VideoFormatType::kRGBA;
    _subscribeSettings.videoSettings.videoWidth = 0;
    _subscribeSettings.videoSettings.videoHeight = 0;

    _subscribeSettings.sinkElement = eGStreamer;

    _subscribeSettings.audioSettings.audioChannels = kStereo;
    _subscribeSettings.audioSettings.audioSampleRate = cAudioSubscribeSampleRate;
    _subscribeSettings.audioSettings.audioBpsType = kS16LE;
}

CallItemVideoView::~CallItemVideoView()
{
    if (_audioInitialized && _audioOutput)
    {
        _audioOutput->stop();
        _audioInitialized = false;

    }

    if (_largeContainer)
    {
        _largeContainer->deleteLater();
    }

    delete ui;
}

void CallItemVideoView::setStreamId(long id)
{
    _streamId = id;
    if (_streamId == 0)
    {
        clear();
    }
}

long CallItemVideoView::getStreamId() const
{
    return _streamId;
}

void CallItemVideoView::setVideoFormat(VideoFormatType type)
{
    _videoFormatType = type;
}

VideoFormatType CallItemVideoView::getVideoFormat() const
{
    return _videoFormatType;
}

void printLog(const char* text)
{    
    char buffer[26];
    int millisec;
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
      millisec -=1000;
      tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
    printf("%s.%03d: %s\n", buffer, millisec, text);

}
void CallItemVideoView::OnVideoFrame(unsigned char *data, size_t size, size_t stride, VideoOrientation orientation)
{
    // TODO: check whether this could be parallel
    if (_videoMuted || !data || stride <= 0)
    {
        //printf("DEBUG. OnVideoFrame: _videoMuted || !data");
        return;
    }

    std::lock_guard<std::mutex> lock(mt_video);

    int width = GetWidthFromStride(_videoFormatType, stride);
    int height = size / stride;

    if (_streamId > 0)
    {
        if (_videoQuality == eVideoQuality::High && _largeContainer != nullptr)
        {
            if (_prevWidth != width || _prevHeight != height)
            {
                // adjust window size if needed
                if (_restrictToScreenResolution && (width > _screenWidth || height > _screenHeight))
                {
                    _largeContainer->setFixedSize(_screenWidth, _screenHeight);
                }
                else
                {
                    _largeContainer->setFixedSize(width, height);
                }
            }
        }

        QImage image(data, width, height, stride, QImage::Format_RGBA8888);

        if (_videoQuality == eVideoQuality::High)
        {
            _largeContainer->setImage(image);

            if (_prevVideoQuality != _videoQuality)
            {
                // if video quallity has been switched from low to high - clear default container
                _largeContainer->show();
                setImage(QImage());
                _prevVideoQuality = _videoQuality;
            }
        }
        else
        {
            image = image.scaled(ui->frameContainer->size());
            setImage(image);
        }

        QString sizeStr = QString::number(width) + "x" + QString::number(height) + ", orient: " + QString::number((int) orientation) ;
        ui->labelSize->setText(sizeStr);
        ui->labelParticipant->setText(_participantName);

        _prevWidth = width;
        _prevHeight = height;
    }
    else
    {
        if (_videoQuality == eVideoQuality::High)
        {
            _largeContainer->setImage(QImage());
        }
        else
        {
            setImage(QImage());
        }
        ui->labelSize->clear();
        ui->labelParticipant->clear();
#ifdef QT_DEBUG
        if (_printLogs)
        {
            printLog("DEBUG. OnVideoFrame: clear image");
        }
#endif
    }
}

void CallItemVideoView::OnAudioFrame(unsigned char *data, size_t size, int channels, int bps)
{
    if (_audioMuted || !data || *data == 0)
        return;

    std::lock_guard<std::mutex> lock(mt_audio);

    //qDebug() << size << channels << bps;

    emit audioStarted(channels, bps);

    if (_audioOutput && _audioBuffer)
    {
        int periodSize = _audioOutput->periodSize(); // Check the ideal chunk size, in bytes
        _audioBuffer->write((const char*) data, periodSize);
    }
}

void CallItemVideoView::OnVideoSizeChanged(const std::string &participantId, const Resolution &res)
{
#ifdef QT_DEBUG
    qDebug() << "DEBUG. OnVideoSizeChanged";
#endif
}

void CallItemVideoView::EnableFramesLogs(bool enable)
{
    _printLogs = enable;
}

void CallItemVideoView::setDirectVideoRendering(bool direct)
{
    _directPreview = direct;
    _subscribeSettings.previewWindowId = direct ? ui->frameContainer->winId() : 0;
}

TransformSettings CallItemVideoView::GetTransformSettings() const
{
    return _transformSettings;
}

void CallItemVideoView::setImage(QImage image)
{
    _image = image;

    // do not update UI directly, just send signal to itself
    emit imageUpdated();
}

void CallItemVideoView::clear()
{
    // set an empty image
    setImage(QImage());
    ui->labelSize->clear();
    ui->labelAudio->clear();
    ui->labelVideo->clear();
    ui->labelOrder->clear();
    ui->labelParticipant->clear();
}

void CallItemVideoView::setAudioMuted(bool muted)
{
    std::unique_lock<std::mutex> lock(mt_audio, std::defer_lock);
    lock.lock();

    _audioMuted = muted;
    ui->labelAudio->setText(muted ? "Audio OFF" : "Audio ON");

    lock.unlock();
}

void CallItemVideoView::setVideoMuted(bool muted)
{
#ifdef QT_DEBUG
    qDebug() << "DEBUG. setVideoMuted";
#endif
    std::unique_lock<std::mutex> lock(mt_video, std::defer_lock);
    lock.lock();
    _videoMuted = muted;
    ui->labelVideo->setText(muted ? "Video OFF" : "Video ON");
    if (_videoMuted)
    {
        setImage(QImage(":/muted.png"));
    }

    lock.unlock();
}

void CallItemVideoView::setParticipantOrder(int order)
{
    ui->labelOrder->setText(QString::number(order));
}

void CallItemVideoView::setParticipantName(QString name)
{
    _participantName = name;
}

void CallItemVideoView::setAudioSampleRate(int rate)
{
    _audioSampleRate = rate;
}

void CallItemVideoView::setExternalVideoSize(int width, int height)
{
    _prevWidth = width;
    _prevHeight = height;
}

int CallItemVideoView::getContainerWindowId()
{
    return ui->frameContainer->winId();
}

int CallItemVideoView::getExternalContainerWindowId()
{
    return _largeContainer ? _largeContainer->winId() : 0;
}

void CallItemVideoView::paintEvent(QPaintEvent *event)
{
    QPainter widgetPainter(this);
    widgetPainter.drawImage(ui->frameContainer->rect(), _image);

    QFrame::paintEvent(event);
}

bool CallItemVideoView::event(QEvent *event)
{
    if (event && event->type() == QEvent::MouseButtonPress && _streamId > 0)
    {
        if (!_qualityDialog)
        {
            _qualityDialog = new VideoQualityDialog(this);
            connect(_qualityDialog, SIGNAL(lowQualitySelected()), this, SLOT(onLowQualitySelected()));
            connect(_qualityDialog, SIGNAL(highQualitySelected()), this, SLOT(onHighQualitySelected()));
        }
        if (_qualityDialog->isHidden())
        {
            _qualityDialog->show();
        }
    }
    return QFrame::event(event);
}

void CallItemVideoView::onLowQualitySelected()
{
    _videoQuality = eVideoQuality::Low;
    if (_largeContainer)
    {
        _largeContainer->close();
    }
    _subscribeSettings.previewWindowId = _directPreview ? ui->frameContainer->winId() : 0;

    emit lowQualitySelected(_streamId);
}

void CallItemVideoView::onHighQualitySelected()
{
    std::lock_guard<std::mutex> lock(mt_video);
    _prevVideoQuality = eVideoQuality::Low;
    _videoQuality = eVideoQuality::High;

    // create separate video displaying window with the previous received size
    if (_largeContainer == nullptr)
    {
        _largeContainer = new ExternalVideoContainer();

        if (_prevWidth > 0 && _prevHeight > 0)
        {
            if (_restrictToScreenResolution && (_prevWidth > _screenWidth || _prevHeight > _screenHeight))
            {
                _largeContainer->setFixedSize(_screenWidth, _screenHeight);
            }
            else
            {
                _largeContainer->setFixedSize(_prevWidth, _prevHeight);
            }
        }

        _largeContainer->setWindowTitle(_participantName);
        connect(_largeContainer, SIGNAL(windowClosed()), this, SLOT(onLargeContainerClosed()));
    }
    _largeContainer->raise();
    _largeContainer->show();

    _subscribeSettings.previewWindowId = _directPreview ? _largeContainer->winId() : 0;

    emit highQualitySelected(_streamId);
}

void CallItemVideoView::onImageUpdated()
{
    ui->frameContainer->repaint();
}

void CallItemVideoView::onAudioStarted(int channels, int bps)
{
    if (!_audioInitialized)
    {
        _audioFormat.setSampleRate(_audioSampleRate);
        _audioFormat.setChannelCount(channels);
        _audioFormat.setSampleSize(bps);
        _audioFormat.setCodec("audio/pcm");
        _audioFormat.setByteOrder(QAudioFormat::LittleEndian);

        _audioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();

        if (!_audioDeviceInfo.isFormatSupported(_audioFormat))
        {
            //qDebug() << "Audio format is not supported";
            return;
        }

        _audioOutput = new QAudioOutput(_audioDeviceInfo, _audioFormat, this);
        _audioBuffer = _audioOutput->start();
        _audioInitialized = true;
    }
}

void CallItemVideoView::onLargeContainerClosed()
{
    std::lock_guard<std::mutex> lock(mt_video);

    // treat external video container close as switching back to low quality
    _qualityDialog->reset();
    _videoQuality = eVideoQuality::Low;

    _subscribeSettings.previewWindowId = _directPreview ? ui->frameContainer->winId() : 0;

    emit lowQualitySelected(_streamId);
}

size_t CallItemVideoView::GetWidthFromStride (VideoFormatType videoFormatType, size_t stride)
{
    if (stride <= 0)
        return 0;

    int width;

    switch (videoFormatType)
    {
    case kGRAY8:
        width = stride;
        break;
    case kUYVY:
    case kYUY2:
    case kYVYU:
        width = stride / 2;
        break;
    case kI420:
    case kI420_10LE:
    case kNV12:
        // if width is impair the stride should contain 2 "odd bytes", multipying by 2 and integer division by 3 gives 1
        width = stride * 2 / 3;
        break;
    case kBGRx:
    case kRGBA:
    default:
        width = stride / 4;
        break;
    }

    return width;
}

void CallItemVideoView::onTransformSettingsPressed()
{
    if (!_transformSettingsDialog)
    {
        _transformSettingsDialog = new TransformSettingsDialog(this);
        connect(_transformSettingsDialog, SIGNAL(settingsApplied()), this, SLOT(onTransformSettingsApplied()));
    }
    if (_transformSettingsDialog->isHidden())
    {
        _transformSettingsDialog->setTransformSettings(_transformSettings);
        _transformSettingsDialog->show();
    }
}

void CallItemVideoView::onTransformSettingsApplied()
{
    if (_transformSettingsDialog)
    {
        _transformSettings.cropLeft = _transformSettingsDialog->GetCropLeft();
        _transformSettings.cropTop = _transformSettingsDialog->GetCropTop();
        _transformSettings.cropRight = _transformSettingsDialog->GetCropRight();
        _transformSettings.cropBottom = _transformSettingsDialog->GetCropBottom();

        _transformSettings.isResizeActivated = _transformSettingsDialog->IsResizeActivated();
        _transformSettings.resizeWidth = _transformSettingsDialog->GetResizeWidth();
        _transformSettings.resizeHeight = _transformSettingsDialog->GetResizeHeight();

        _transformSettings.rotateType = _transformSettingsDialog->GetRotateType();

        _subscribeSettings.videoSettings.cropSettings.left = _transformSettingsDialog->GetCropLeft();
        _subscribeSettings.videoSettings.cropSettings.top = _transformSettingsDialog->GetCropTop();
        _subscribeSettings.videoSettings.cropSettings.right = _transformSettingsDialog->GetCropRight();
        _subscribeSettings.videoSettings.cropSettings.bottom = _transformSettingsDialog->GetCropBottom();

        _subscribeSettings.videoSettings.isResize = _transformSettingsDialog->IsResizeActivated();
        _subscribeSettings.videoSettings.videoWidth = _transformSettingsDialog->GetResizeWidth();
        _subscribeSettings.videoSettings.videoHeight = _transformSettingsDialog->GetResizeHeight();

        _subscribeSettings.videoSettings.flipMethod = (FlipMethod)_transformSettingsDialog->GetRotateType();

        emit transformSettingsUpdated(_streamId);
    }
}
