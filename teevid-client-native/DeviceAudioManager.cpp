#include "DeviceAudioManager.h"

#include <QDebug>
#include <thread>

DeviceAudioManager::DeviceAudioManager(QObject *parent) : QObject(parent)
{
    _timer.setSingleShot(false);
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    DeviceAudioManager *self = (DeviceAudioManager *)data;

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
        self->StartAudio();
    } break;

    default:
        break;
    }

    return TRUE;
}

bool DeviceAudioManager::Start(int fps, int frames, int channels, const std::string& format)
{
    _fps = fps;
    _frames = frames;
    _channels = channels;

    _timer.setInterval(1000 / _fps);

    gst_init(NULL, NULL);

    std::string pipelineStr = "alsasrc ! queue ! audioconvert ! audio/x-raw,format=" +
                              format + ",rate=" +
                              std::to_string(_frames) + ",channels=" + std::to_string(_channels) +
                              " ! audiobuffersplit name=splitter ! appsink name=appsink_audio";

    qDebug() << QString::fromStdString(pipelineStr);

    _pipeline = gst_parse_launch(pipelineStr.c_str(), NULL);
    if (_pipeline == NULL)
    {
        emit audioError("Erroneous pipeline");
        return false;
    }

    GstElement* splitter = gst_bin_get_by_name(GST_BIN(_pipeline), "splitter");
    if (splitter != NULL)
    {
        g_object_set(splitter, "output-buffer-duration", 1, _fps, NULL);
    }

    _appsink = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink_audio");

    GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        emit audioError("Failed to start pipeline");
        gst_object_unref(_pipeline);
        _pipeline = NULL;
        return false;
    }

    _loop = g_main_loop_new(NULL, FALSE);

    // add a message handler
    _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
    _bus_watch_id = gst_bus_add_watch(_bus, bus_call, this);
    gst_object_unref(_bus);

    std::thread t(&DeviceAudioManager::GstTimerFunc, this);
    t.detach();

    return true;
}

void DeviceAudioManager::Stop()
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

void DeviceAudioManager::StartAudio()
{
    if (!_timer.isActive())
    {
        _timer.start();
    }
}

void DeviceAudioManager::QuitMainLoop()
{
    if (_loop != NULL)
    {
        g_main_loop_quit(_loop);
    }
}

void DeviceAudioManager::HandleError(QString error)
{
    if (_timer.isActive())
    {
        _timer.stop();
    }

    emit audioError(error);
}

void DeviceAudioManager::OnTimer()
{
    PullBuffer();
}

void DeviceAudioManager::GstTimerFunc()
{
    if (_loop != NULL)
    {
        g_main_loop_run(_loop);
    }
}

void DeviceAudioManager::PullBuffer()
{
    // Will block until sample is ready. In our case "sample" is encoded picture.
    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(_appsink), 0);
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

    emit audioFrame(map.data, map.size);

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
}
