#ifndef EXTERNALVIDEOCONTAINER_H
#define EXTERNALVIDEOCONTAINER_H

#include <QFrame>

class ExternalVideoContainer : public QFrame
{
    Q_OBJECT
public:
    explicit ExternalVideoContainer(QWidget *parent = nullptr);

    void setImage(QImage image);
    void clear();

signals:
    void windowClosed();

protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QImage _image;
};

#endif // EXTERNALVIDEOCONTAINER_H
