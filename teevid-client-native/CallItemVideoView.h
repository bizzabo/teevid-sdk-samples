#ifndef CALLITEMVIDEOVIEW_H
#define CALLITEMVIDEOVIEW_H

#include "teevid_sdk/IStreamSubscriber.h"
#include "teevid_sdk/MediaSettings.h"

#include <QFrame>

#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>

#include <mutex>

class QImage;
class VideoQualityDialog;
class ExternalVideoContainer;

namespace Ui {
class CallItemVideoView;
}

using namespace teevid_sdk;

class CallItemVideoView : public QFrame, public IStreamSubscriber
{
    Q_OBJECT

    enum eVideoQuality
    {
        Low = 0,
        High = 1
    };

public:
    explicit CallItemVideoView(QWidget *parent = nullptr);
    ~CallItemVideoView();

    void setStreamId(long id);
    long getStreamId() const;

    void setVideoFormat(VideoFormatType type);
    VideoFormatType getVideoFormat() const;

    void setImage(QImage image);
    void clear();

    void setAudioMuted(bool muted);
    void setVideoMuted(bool muted);

    void setParticipantOrder(int order);
    void setParticipantName(QString name);

    void setAudioSampleRate(int rate);

    // IStreamSubscriber
    virtual void OnVideoFrame(unsigned char *data, size_t size, size_t stride, VideoOrientation orientation) override;
    virtual void OnAudioFrame(unsigned char *data, size_t size, int channels, int bps) override;
    virtual void OnVideoSizeChanged(const std::string& participantId, const Resolution& res) override;

    void EnableFramesLogs(bool enable);

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

    size_t GetWidthFromStride (VideoFormatType videoFormatType, size_t stride);

signals:
    void lowQualitySelected(long);
    void highQualitySelected(long);

    void imageUpdated();
    void audioStarted(int, int);

protected slots:
    void onLowQualitySelected();
    void onHighQualitySelected();

    void onImageUpdated();
    void onAudioStarted(int channels, int bps);

    void onLargeContainerClosed();

private:
    bool _printLogs = false;
    Ui::CallItemVideoView *ui;
    long _streamId = 0;
    QImage _image;
    VideoQualityDialog* _qualityDialog = nullptr;

    ExternalVideoContainer* _largeContainer = nullptr;
    eVideoQuality _videoQuality = eVideoQuality::Low;

    // store previous video parameters to adjust container if needed
    eVideoQuality _prevVideoQuality = eVideoQuality::Low;
    int _prevWidth = 0;
    int _prevHeight = 0;

    bool _audioMuted = false;
    bool _videoMuted = false;

    VideoFormatType _videoFormatType = kRGBA;

    int _audioSampleRate = 48000;
    std::atomic<bool> _audioInitialized;
    QAudioOutput *_audioOutput = nullptr;
    QAudioFormat _audioFormat;
    QAudioDeviceInfo _audioDeviceInfo;
    QIODevice* _audioBuffer = nullptr;     // IODevice to connect to m_AudioOutput

    // store participant name here and display only if there's an active stream
    QString _participantName;

    std::mutex mt_audio;
    std::mutex mt_video;
};

#endif // CALLITEMVIDEOVIEW_H
