#include "CallItemVideoView.h"
#include "ui_CallItemVideoView.h"

#include "VideoQualityDialog.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QDateTime>
#include <qmath.h>

const int cDummyVideoFrameWidth = 1920;
const int cDummyVideoFrameHeight = 1080;

// each pixel is coded via 4 bytes: red, green, blue and alpha channel
const int cBytesPerPixel = 4;
const size_t cDummyVideoSize = cDummyVideoFrameWidth * cDummyVideoFrameHeight * cBytesPerPixel;

CallItemVideoView::CallItemVideoView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CallItemVideoView),
    _audioInitialized(false)
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
}

CallItemVideoView::~CallItemVideoView()
{
    if (_audioInitialized && _audioOutput)
    {
        _audioOutput->stop();
        _audioInitialized = false;

    }
    delete ui;
}

void CallItemVideoView::setStreamId(long id)
{
    _streamId = id;
    if (_streamId == 0)
    {
        setImage(QImage());
        ui->labelSize->clear();
        ui->labelAudio->clear();
        ui->labelVideo->clear();
        ui->labelOrder->clear();
    }
}

long CallItemVideoView::getStreamId() const
{
    return _streamId;
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
void CallItemVideoView::OnVideoFrame(unsigned char *data, size_t size, size_t stride)
{
    // TODO: check whether this could be parallel
    if (_videoMuted || !data || *data == 0)
    {
        //printf("DEBUG. OnVideoFrame: _videoMuted || !data");
        return;
    }

    std::lock_guard<std::mutex> lock(mt_video);

    int width = stride / cBytesPerPixel;
    int height = size / stride;

    if (_streamId > 0)
    {
        QImage image(data, width, height, stride, QImage::Format_RGBA8888);
        image = image.scaled(ui->frameContainer->size());
        setImage(image);
        QString sizeStr = QString::number(width) + "x" + QString::number(height);
        ui->labelSize->setText(sizeStr);
    }
    else
    {
        setImage(QImage());
        ui->labelSize->clear();
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

void CallItemVideoView::setAudioSampleRate(int rate)
{
    _audioSampleRate = rate;
}

void CallItemVideoView::paintEvent(QPaintEvent *event)
{
    QPainter widgetPainter(this);
    widgetPainter.drawImage(ui->frameContainer->rect(), _image);

    QFrame::paintEvent(event);
}

bool CallItemVideoView::event(QEvent *event)
{
    if (event && event->type() == QEvent::MouseButtonPress)
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
    emit lowQualitySelected(_streamId);
}

void CallItemVideoView::onHighQualitySelected()
{
    emit highQualitySelected(_streamId);
}

void CallItemVideoView::onImageUpdated()
{
    ui->frameContainer->update();
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
