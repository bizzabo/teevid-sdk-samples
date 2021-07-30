#include "DeviceAudioManager.h"

#include <QDebug>
#include <thread>

DeviceAudioManager::DeviceAudioManager(QObject *parent) : QObject(parent)
{
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

    default:
        break;
    }

    return TRUE;
}

static GstFlowReturn new_sample_audio (GstElement *sink, gpointer data)
{
    DeviceAudioManager *self = (DeviceAudioManager *)data;
    self->PullBuffer();
}

bool DeviceAudioManager::Start(int fps, int frames, int channels, const std::string& format)
{
    _screenSharing = false;
    std::string pipelineStr = "alsasrc ! queue ! audioconvert ! audio/x-raw,format=" +
                              format + ",rate=" +
                              std::to_string(frames) + ",channels=" + std::to_string(channels) +
                              " ! appsink name=appsink_audio";

    return StartInternal(pipelineStr);
}

bool DeviceAudioManager::Start(const std::string &format)
{
    _screenSharing = true;
    std::string pipelineStr = "audiotestsrc volume=0 ! queue ! audioconvert ! audio/x-raw,format=" + format +
                              " ! appsink name=appsink_audio";

    return StartInternal(pipelineStr);
}

void DeviceAudioManager::Stop()
{
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

void DeviceAudioManager::QuitMainLoop()
{
    if (_loop != NULL)
    {
        g_main_loop_quit(_loop);
    }
}

void DeviceAudioManager::HandleError(QString error)
{
    emit audioError(error, _screenSharing);
}

void DeviceAudioManager::GstTimerFunc()
{
    if (_loop != NULL)
    {
        g_main_loop_run(_loop);
    }
}

bool DeviceAudioManager::StartInternal(const std::string &pipelineStr)
{
    gst_init(NULL, NULL);

    qDebug() << QString::fromStdString(pipelineStr);

    _pipeline = gst_parse_launch(pipelineStr.c_str(), NULL);
    if (_pipeline == NULL)
    {
        emit audioError("Erroneous pipeline", _screenSharing);
        return false;
    }

    // BE AWARE: for now audiobuffersplit item is not used.
    // If you use it (see commented pipeline) then un-comment the code below:

//    GstElement* splitter = gst_bin_get_by_name(GST_BIN(_pipeline), "splitter");
//    if (splitter != NULL)
//    {
//        g_object_set(splitter, "output-buffer-duration", 1, _fps, NULL);
//    }

    _appsink = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink_audio");

    GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        emit audioError("Failed to start pipeline", _screenSharing);
        gst_object_unref(_pipeline);
        _pipeline = NULL;
        return false;
    }

    _loop = g_main_loop_new(NULL, FALSE);

    // add a message handler
    _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
    _bus_watch_id = gst_bus_add_watch(_bus, bus_call, this);
    gst_object_unref(_bus);

    g_object_set (G_OBJECT (_appsink), "emit-signals", TRUE, NULL);
    g_signal_connect (_appsink, "new-sample", G_CALLBACK (new_sample_audio), this);

    std::thread t(&DeviceAudioManager::GstTimerFunc, this);
    t.detach();

    return true;
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

    emit audioFrame(map.data, map.size, _screenSharing);

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
}
