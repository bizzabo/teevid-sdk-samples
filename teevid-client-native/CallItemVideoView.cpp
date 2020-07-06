#include "CallItemVideoView.h"
#include "ui_CallItemVideoView.h"

#include "VideoQualityDialog.h"

#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QDateTime>

const int cDummyVideoFrameWidth = 1920;
const int cDummyVideoFrameHeight = 1080;

// each pixel is coded via 4 bytes: red, green, blue and alpha channel
const int cBytesPerPixel = 4;
const size_t cDummyVideoSize = cDummyVideoFrameWidth * cDummyVideoFrameHeight * cBytesPerPixel;

#define PCM_DEVICE "default"

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
}

CallItemVideoView::~CallItemVideoView()
{
    if (_audioInitialized)
    {
        snd_pcm_drain(_pcm_handle);
        snd_pcm_close(_pcm_handle);
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
    }
}

long CallItemVideoView::getStreamId() const
{
    return _streamId;
}

void CallItemVideoView::OnVideoFrame(unsigned char *data, size_t size, size_t stride)
{
    // TODO: check whether this could be parallel
    if (_videoMuted || !data)
        return;

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
    }
}

void CallItemVideoView::OnAudioFrame(unsigned char *data, size_t size, int channels, int bps)
{
    if (_audioMuted || !data || *data == 0)
        return;

    std::lock_guard<std::mutex> lock(mt_audio);

    //qDebug() << size << channels << bps;

    if (!initializeAudioRendererIfNeeded(channels))
        return;

    // TODO: handle channels count change

    int result = snd_pcm_writei(_pcm_handle, data, _frames);
    if (result == -EPIPE)
    {
        snd_pcm_prepare(_pcm_handle);
    }
    else if (result < 0)
    {
        printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(result));
    }
    // else - ignore any other codes
}

void CallItemVideoView::OnVideoSizeChanged(const std::string &participantId, const Resolution &res)
{

}

void CallItemVideoView::setImage(QImage image)
{
    _image = image;
    ui->frameContainer->update();
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

bool CallItemVideoView::initializeAudioRendererIfNeeded(int channels)
{
    if (!_audioInitialized)
    {
        _rate = _audioSampleRate;

         /* Open the PCM device in playback mode */
        int result = snd_pcm_open(&_pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
        if (result < 0)
        {
            printf("ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(result));
            return false;
        }

        /* Allocate parameters object and fill it with default values*/
        snd_pcm_hw_params_alloca(&_params);
        snd_pcm_hw_params_any(_pcm_handle, _params);

        /* Set parameters */
        if ((snd_pcm_hw_params_set_access(_pcm_handle, _params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) ||
            (snd_pcm_hw_params_set_format(_pcm_handle, _params, SND_PCM_FORMAT_S16_LE) < 0) ||
            (snd_pcm_hw_params_set_channels(_pcm_handle, _params, channels) < 0) ||
            (snd_pcm_hw_params_set_rate_near(_pcm_handle, _params, &_rate, 0) < 0))
        {
            printf("ERROR: failed to set PCM handle parameters\n");
            return false;
        }

        /* Write parameters */
        if (snd_pcm_hw_params(_pcm_handle, _params) < 0)
        {
            printf("ERROR: failed to write PCM handle parameters\n");
            return false;
        }

        /* Allocate buffer to hold single period */
        if (snd_pcm_hw_params_get_period_size(_params, &_frames, 0) < 0)
        {
            printf("ERROR: failed allocate buffer\n");
            return false;
        }

        _audioInitialized = true;
    }
    return _audioInitialized;
}

void CallItemVideoView::onLowQualitySelected()
{
    emit lowQualitySelected(_streamId);
}

void CallItemVideoView::onHighQualitySelected()
{
    emit highQualitySelected(_streamId);
}
