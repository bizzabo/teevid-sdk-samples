#ifndef DEVICEVIDEOMANAGER_H
#define DEVICEVIDEOMANAGER_H

#include <QObject>
#include <QTimer>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

class DeviceVideoManager : public QObject
{
    Q_OBJECT

protected:
    enum eVideoType
    {
        Publish = 0,
        Internal = 1
    };

public:    
    explicit DeviceVideoManager(QObject *parent = nullptr);

    // currently Start method initalizes GStreamer components and launches the timer for pipeline data retrieval
    // Stop - ends the timer and destroys GStreamer components

    /* BE AWARE:
    *  As the data produced by the pipeline is retained by SDK - be sure you disconnect from the SDK BEFORE calling Stop method
    *  Otherwise split it into 2 methods and just stop the timer BEFORE disconnecting from the SDK but destroy the pipeline and main loop AFTER disconnection from SDK!
    */
    bool Start(int fps, int width, int height, const std::string& format);
    void Stop();

    void StartVideo();
    void QuitMainLoop();
    void HandleError(QString error);

signals:
    void publishVideoFrame(unsigned char* data, size_t size, int stride);
    void internalVideoFrame(unsigned char* data, size_t size, int stride);
    void videoError(QString message);

protected slots:
    void OnTimer();

protected:
    void GstTimerFunc();
    void PullBuffer(eVideoType type);

private:
    int _fps = 30;
    int _width = 640;
    int _height = 480;

    QTimer _timer;

    GMainLoop* _loop = NULL;
    GstElement* _pipeline = NULL;
    GstElement* _appsinkPublish = NULL;
    GstElement* _appsinkInternal = NULL;
    GstBus* _bus = NULL;
    guint _bus_watch_id = 0;
};

#endif // DEVICEVIDEOMANAGER_H
