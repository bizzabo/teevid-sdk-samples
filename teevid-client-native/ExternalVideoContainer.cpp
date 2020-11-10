#include "ExternalVideoContainer.h"

#include <QPainter>
#include <QDebug>

ExternalVideoContainer::ExternalVideoContainer(QWidget *parent) : QFrame(parent)
{
}

void ExternalVideoContainer::setImage(const QImage& image)
{
    std::lock_guard<std::mutex> lock(mt);
    _image = image.copy();
    this->update();
}

void ExternalVideoContainer::clear()
{
    // set an empty image
    setImage(QImage());
}

void ExternalVideoContainer::paintEvent(QPaintEvent *event)
{
    std::lock_guard<std::mutex> lock(mt);
    QPainter widgetPainter(this);

    if (!_image.isNull())
      widgetPainter.drawImage(rect(), _image);

    QFrame::paintEvent(event);
}

void ExternalVideoContainer::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    QFrame::closeEvent(event);
}
