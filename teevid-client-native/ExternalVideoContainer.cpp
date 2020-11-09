#include "ExternalVideoContainer.h"

#include <QPainter>
#include <QDebug>

ExternalVideoContainer::ExternalVideoContainer(QWidget *parent) : QFrame(parent)
{
}

void ExternalVideoContainer::setImage(QImage image)
{
    _image = image;
    repaint();
}

void ExternalVideoContainer::clear()
{
    // set an empty image
    setImage(QImage());
}

void ExternalVideoContainer::paintEvent(QPaintEvent *event)
{
    QPainter widgetPainter(this);
    widgetPainter.drawImage(rect(), _image);

    QFrame::paintEvent(event);
}

void ExternalVideoContainer::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    QFrame::closeEvent(event);
}
