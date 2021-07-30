#ifndef DEVICEVIDEOMANAGER_H
#define DEVICEVIDEOMANAGER_H

#include <QObject>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include "teevid_sdk/DesktopShare.h"

class DeviceVideoManager : public QObject
{
    Q_OBJECT

public:
    enum eVideoType
    {
        Publish = 0,
        Internal = 1
    };

    explicit DeviceVideoManager(QObject *parent = nullptr);

    // currently Start method initalizes GStreamer components and launches the timer for pipeline data retrieval
    // Stop - ends the timer and destroys GStreamer components

    /* BE AWARE:
    *  As the data produced by the pipeline is retained by SDK - be sure you disconnect from the SDK BEFORE calling Stop method
    *  Otherwise split it into 2 methods and just stop the timer BEFORE disconnecting from the SDK but destroy the pipeline and main loop AFTER disconnection from SDK!
    */
    bool Start(int width, int height, const std::string& format);
    bool Start(const std::string& format, const DesktopShareOptions& desktopOptions);
    void Stop();

    void StartVideo();
    void QuitMainLoop();
    void HandleError(QString error);
    void UpdateCaps(int width, int height, int fps);

    int RemainingRetryCount();

    void PullBuffer(eVideoType type);
    void Retry();

    DesktopShareOptions desktopOptions() const;
    void setDesktopOptions(const DesktopShareOptions &desktopOptions);

signals:
    void publishVideoFrame(unsigned char* data, long size, int stride, bool screenSharing);
    void internalVideoFrame(unsigned char* data, long size, int stride, bool screenSharing);
    void videoError(QString message, bool screenSharing);
    void videoStarted(int, int, bool);
    void capsUpdated(int, int, int, bool);

protected:
    void GstTimerFunc();
    bool StartInternal(const std::string &pipelineStr);
    void StopInternal();

private:
    int _fps = 30;
    int _width = 1920;
    int _height = 1080;
    std::string _publishFormat = "RGBA";
    DesktopShareOptions _desktopOptions;
    int _retryCount = 2;
    bool _screenSharing = false;

    GMainLoop* _loop = NULL;
    GstElement* _pipeline = NULL;
    GstElement* _appsinkPublish = NULL;
    GstElement* _appsinkInternal = NULL;
    GstBus* _bus = NULL;
    guint _bus_watch_id = 0;
};

#endif // DEVICEVIDEOMANAGER_H
