#include "CallItemVideoView.h"
#include "VideoQualityDialog.h"

#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <mutex>

const int cDummyVideoFrameWidth = 1920;
const int cDummyVideoFrameHeight = 1080;
const int cBytesPerPixel = 3;
const size_t cDummyVideoSize = cDummyVideoFrameWidth * cDummyVideoFrameHeight * cBytesPerPixel;

std::mutex mt;

CallItemVideoView::CallItemVideoView(QWidget *parent) : QFrame(parent)
{
    setFixedSize(256, 192);
}

void CallItemVideoView::setStreamId(long id)
{
    _streamId = id;
    if (_streamId == 0)
    {
        setImage(QImage());
    }
}

long CallItemVideoView::getStreamId() const
{
    return _streamId;
}

void CallItemVideoView::OnVideoFrame(unsigned char *data, size_t size, size_t stride)
{
    // TODO: check whether this could be parallel
    std::unique_lock<std::mutex> lock(mt, std::defer_lock);
    lock.lock();
    uchar udata[cDummyVideoSize];
    memcpy(udata, data, sizeof(udata));

    int width = stride / cBytesPerPixel;
    int height = size / stride;

    if (_streamId > 0)
    {
        QImage image(udata, width, height, stride, QImage::Format_RGB888);
        image = image.scaled(this->size());
        setImage(image);
    }
    else
    {
        setImage(QImage());
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
    update();
}

void CallItemVideoView::paintEvent(QPaintEvent *event)
{
    QPainter widgetPainter(this);
    widgetPainter.drawImage(0, 0, _image);

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
