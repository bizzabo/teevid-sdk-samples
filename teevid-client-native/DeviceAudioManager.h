#ifndef DEVICEAUDIOMANAGER_H
#define DEVICEAUDIOMANAGER_H

#include <QObject>

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

    void QuitMainLoop();
    void HandleError(QString error);

    void PullBuffer();

signals:
    void audioFrame(unsigned char* data, long size);
    void audioError(QString message);

protected:
    void GstTimerFunc();

private:
    int _fps = 30;
    int _frames = 48000;
    int _channels = 2;

    GMainLoop* _loop = NULL;
    GstElement* _pipeline = NULL;
    GstElement* _appsink = NULL;
    GstBus* _bus = NULL;
    guint _bus_watch_id = 0;
};

#endif // DEVICEAUDIOMANAGER_H
