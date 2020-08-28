#ifndef DEVICEAUDIOMANAGER_H
#define DEVICEAUDIOMANAGER_H

#include <QObject>
#include <QTimer>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

class DeviceAudioManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceAudioManager(QObject *parent = nullptr);

    bool Start(int fps, int frames, int channels, const std::string& format);
    void Stop();

    void StartAudio();
    void QuitMainLoop();
    void HandleError(QString error);

signals:
    void audioFrame(unsigned char* data, size_t size);
    void audioError(QString message);

protected slots:
    void OnTimer();

protected:
    void GstTimerFunc();
    void PullBuffer();

private:
    int _fps = 30;
    int _frames = 48000;
    int _channels = 2;

    QTimer _timer;

    GMainLoop* _loop = NULL;
    GstElement* _pipeline = NULL;
    GstElement* _appsink = NULL;
    GstBus* _bus = NULL;
    guint _bus_watch_id = 0;
};

#endif // DEVICEAUDIOMANAGER_H
