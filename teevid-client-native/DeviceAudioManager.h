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
    bool Start(const std::string& format);
    void Stop();

    void QuitMainLoop();
    void HandleError(QString error);

    void PullBuffer();

signals:
    void audioFrame(unsigned char* data, long size, bool screenSharing);
    void audioError(QString message, bool screenSharing);

protected:
    void GstTimerFunc();
    bool StartInternal(const std::string& pipelineStr);

private:
    GMainLoop* _loop = NULL;
    GstElement* _pipeline = NULL;
    GstElement* _appsink = NULL;
    GstBus* _bus = NULL;
    guint _bus_watch_id = 0;
    bool _screenSharing = false;
};

#endif // DEVICEAUDIOMANAGER_H
