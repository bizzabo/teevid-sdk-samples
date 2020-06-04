#ifndef CALLITEMVIDEOVIEW_H
#define CALLITEMVIDEOVIEW_H

#include "teevid_sdk/IStreamSubscriber.h"

#include <QFrame>

class QImage;
class VideoQualityDialog;

using namespace teevid_sdk;

class CallItemVideoView : public QFrame, public IStreamSubscriber
{
    Q_OBJECT
public:
    explicit CallItemVideoView(QWidget *parent = nullptr);

    void setStreamId(long id);
    long getStreamId() const;

    void setImage(QImage image);

    // IStreamSubscriber
    virtual void OnVideoFrame(unsigned char *data, size_t size, size_t stride) override;
    virtual void OnAudioFrame(unsigned char *data, size_t size, int channels, int bps) override;
    virtual void OnVideoSizeChanged(const std::string& participantId, const Resolution& res) override;

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

signals:
    void lowQualitySelected(long);
    void highQualitySelected(long);

protected slots:
    void onLowQualitySelected();
    void onHighQualitySelected();

private:
    long _streamId = 0;
    QImage _image;
    VideoQualityDialog* _qualityDialog = nullptr;
};

#endif // CALLITEMVIDEOVIEW_H
