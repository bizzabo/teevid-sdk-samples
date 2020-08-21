#include "DeviceVideoManager.h"

#include <QDebug>
#include <thread>

DeviceVideoManager::DeviceVideoManager(QObject *parent) : QObject(parent)
{
    _timer.setSingleShot(false);
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    DeviceVideoManager *self = (DeviceVideoManager *)data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
    {
        fprintf(stderr, "End of stream\n");
        self->QuitMainLoop();
    } break;

    case GST_MESSAGE_ERROR:
    {
        gchar *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);
        self->HandleError(error->message);
        g_error_free(error);

        self->QuitMainLoop();
    } break;

    case GST_MESSAGE_STEP_DONE:
    case GST_MESSAGE_ASYNC_DONE:
    {
        self->StartVideo();
    } break;

    default:
        break;
    }

    return TRUE;
}

bool DeviceVideoManager::Start(int fps, int width, int height, const std::string &format)
{
    _fps = fps;
    _width = width;
    _height = height;

    _timer.setInterval(1000 / _fps);

    gst_init(NULL, NULL);

    std::string pipelineStr = "v4l2src device=/dev/video0 ! tee name=t ! queue ";
               pipelineStr += " ! videoconvert ! videoscale ! video/x-raw,format=" +
                              format + ",framerate=" +
                              std::to_string(_fps) + "/1,width=" +
                              std::to_string(_width) + ",height=" + std::to_string(_height) +
                              " ! appsink name=appsink0 t. ! queue " +
                              " ! videoconvert ! videoscale ! video/x-raw,format=RGBA,framerate=" +
                              std::to_string(_fps) + "/1,width=" +
                              std::to_string(_width) + ",height=" + std::to_string(_height) +
                              " ! appsink name=appsink1";

    //qDebug() << QString::fromStdString(pipelineStr);

    _pipeline = gst_parse_launch(pipelineStr.c_str(), NULL);
    if (_pipeline == NULL)
    {
        emit videoError("Erroneous pipeline");
        return false;
    }

    _appsinkPublish = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink0");
    _appsinkInternal = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink1");

    GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        emit videoError("Failed to start pipeline");
        gst_object_unref(_pipeline);
        _pipeline = NULL;
        return false;
    }

    _loop = g_main_loop_new(NULL, FALSE);

    // add a message handler
    _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
    _bus_watch_id = gst_bus_add_watch(_bus, bus_call, this);
    gst_object_unref(_bus);

    std::thread t(&DeviceVideoManager::GstTimerFunc, this);
    t.detach();

    return true;
}

void DeviceVideoManager::Stop()
{
    if (_timer.isActive())
    {
        _timer.stop();
    }

    if (_pipeline == NULL || _loop == NULL)
    {
        // to ensure the stop is not called several times
        return;
    }

    gst_element_set_state(_pipeline, GST_STATE_NULL);
    g_main_loop_quit(_loop);

    gst_object_unref(GST_OBJECT(_pipeline));
    g_source_remove(_bus_watch_id);
    g_main_loop_unref(_loop);

    _pipeline = NULL;
    _loop = NULL;
    _bus_watch_id = 0;
}

void DeviceVideoManager::StartVideo()
{
    if (!_timer.isActive())
    {
        _timer.start();
    }
}

void DeviceVideoManager::QuitMainLoop()
{
    if (_loop != NULL)
    {
        g_main_loop_quit(_loop);
    }
}

void DeviceVideoManager::HandleError(QString error)
{
    if (_timer.isActive())
    {
        _timer.stop();
    }

    emit videoError(error);
}

void DeviceVideoManager::OnTimer()
{
    PullBuffer(eVideoType::Publish);
    PullBuffer(eVideoType::Internal);
}

void DeviceVideoManager::GstTimerFunc()
{
    if (_loop != NULL)
    {
        g_main_loop_run(_loop);
    }
}

void DeviceVideoManager::PullBuffer(eVideoType type)
{
    GstElement* appsink = (type == eVideoType::Publish) ? _appsinkPublish : _appsinkInternal;

    // Will block until sample is ready. In our case "sample" is encoded picture.
    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 0);
    if (sample == NULL)
    {
        //printf("gst_app_sink_pull_sample returned null\n");
        return;
    }

    // Actual compressed image is stored inside GstSample.
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ) == false)
    {
        //printf("gst_buffer_map null\n");
        return;
    }

    if (type == eVideoType::Publish)
    {
        emit publishVideoFrame(map.data, map.size, map.size / _height);
    }
    else
    {
        emit internalVideoFrame(map.data, map.size, map.size / _height);
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
}
