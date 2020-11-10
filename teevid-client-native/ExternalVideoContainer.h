#ifndef EXTERNALVIDEOCONTAINER_H
#define EXTERNALVIDEOCONTAINER_H

#include <QFrame>
#include <mutex>

class ExternalVideoContainer : public QFrame
{
    Q_OBJECT
public:
    explicit ExternalVideoContainer(QWidget *parent = nullptr);

    void setImage(const QImage& image);
    void clear();

signals:
    void windowClosed();

protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QImage _image;
    std::mutex mt;
};

#endif // EXTERNALVIDEOCONTAINER_H
