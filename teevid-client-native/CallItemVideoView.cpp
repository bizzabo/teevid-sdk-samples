#include "CallItemVideoView.h"
#include "ui_CallItemVideoView.h"

#include "VideoQualityDialog.h"

#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <mutex>

const int cDummyVideoFrameWidth = 1920;
const int cDummyVideoFrameHeight = 1080;

// each pixel is coded via 4 bytes: red, green, blue and alpha channel
const int cBytesPerPixel = 4;
const size_t cDummyVideoSize = cDummyVideoFrameWidth * cDummyVideoFrameHeight * cBytesPerPixel;

std::mutex mt;

CallItemVideoView::CallItemVideoView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CallItemVideoView)
{
    ui->setupUi(this);

    ui->labelParticipant->clear();
    ui->labelSize->clear();
}

CallItemVideoView::~CallItemVideoView()
{
    delete ui;
}

void CallItemVideoView::setStreamId(long id)
{
    _streamId = id;
    if (_streamId == 0)
    {
        setImage(QImage());
        ui->labelSize->clear();
    }
}

long CallItemVideoView::getStreamId() const
{
    return _streamId;
}

void CallItemVideoView::OnVideoFrame(unsigned char *data, size_t size, size_t stride)
{
    // TODO: check whether this could be parallel
    if (!data)
        return;

    std::unique_lock<std::mutex> lock(mt, std::defer_lock);
    lock.lock();

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
    lock.unlock();
}

void CallItemVideoView::OnAudioFrame(unsigned char *data, size_t size, int channels, int bps)
{

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
