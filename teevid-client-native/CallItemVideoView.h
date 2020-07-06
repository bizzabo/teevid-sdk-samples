#ifndef CALLITEMVIDEOVIEW_H
#define CALLITEMVIDEOVIEW_H

#include "teevid_sdk/IStreamSubscriber.h"

#include <QFrame>

#include <alsa/asoundlib.h>
#include <mutex>

class QImage;
class VideoQualityDialog;

namespace Ui {
class CallItemVideoView;
}

using namespace teevid_sdk;

class CallItemVideoView : public QFrame, public IStreamSubscriber
{
    Q_OBJECT

public:
    explicit CallItemVideoView(QWidget *parent = nullptr);
    ~CallItemVideoView();

    void setStreamId(long id);
    long getStreamId() const;

    void setImage(QImage image);
    void clear();

    void setAudioMuted(bool muted);
    void setVideoMuted(bool muted);

    void setAudioSampleRate(int rate);

    // IStreamSubscriber
    virtual void OnVideoFrame(unsigned char *data, size_t size, size_t stride) override;
    virtual void OnAudioFrame(unsigned char *data, size_t size, int channels, int bps) override;
    virtual void OnVideoSizeChanged(const std::string& participantId, const Resolution& res) override;

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

    bool initializeAudioRendererIfNeeded(int channels);

signals:
    void lowQualitySelected(long);
    void highQualitySelected(long);

protected slots:
    void onLowQualitySelected();
    void onHighQualitySelected();

private:
    Ui::CallItemVideoView *ui;
    long _streamId = 0;
    QImage _image;
    VideoQualityDialog* _qualityDialog = nullptr;

    bool _audioMuted = false;
    bool _videoMuted = false;

    int _audioSampleRate = 48000;
    unsigned int _rate;
    snd_pcm_t *_pcm_handle;
    snd_pcm_hw_params_t *_params;
    snd_pcm_uframes_t _frames;
    std::atomic<bool> _audioInitialized;

    std::mutex mt_audio;
    std::mutex mt_video;
};

#endif // CALLITEMVIDEOVIEW_H
