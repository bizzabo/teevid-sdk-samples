#include "DeviceVideoManager.h"

#include <QDebug>
#include <thread>

const int cDefaultWidth = 1280;
const int cDefaultHeight = 720;
const int cRetryCount = 2;

DeviceVideoManager::DeviceVideoManager(QObject *parent) : QObject(parent), _retryCount(cRetryCount)
{
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
        self->QuitMainLoop();
        if (self->RemainingRetryCount() > 0)
        {
            // if there's remaining retries - try with smaller video resolution
            self->Retry();
        }
        else
        {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);
            self->HandleError(error->message);
            g_error_free(error);
        }
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

static GstPadProbeReturn
sink_pad_probe (GstPad *pad, GstPadProbeInfo *info, gpointer data)
{
  if (GST_PAD_PROBE_INFO_TYPE (info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM)
  {
    GstEvent *event = GST_PAD_PROBE_INFO_EVENT (info);

    if (GST_EVENT_TYPE (event) == GST_EVENT_CAPS)
    {
      GstCaps *event_caps = NULL;
      gst_event_parse_caps (event, &event_caps);

      int width = 0;
      int height = 0;
      int fps_numerator = 0;
      int fps_denominator = 1;

      GstStructure *convert_caps_struct = gst_caps_get_structure((GstCaps *) event_caps, 0);
      gst_structure_get_int(convert_caps_struct, "width", &width);
      gst_structure_get_int(convert_caps_struct, "height", &height);
      gst_structure_get_fraction(convert_caps_struct, "framerate", &fps_numerator, &fps_denominator);

      DeviceVideoManager *self = (DeviceVideoManager *)data;
      int fps_to_send = (fps_denominator > 0) ? (fps_numerator / fps_denominator) : 0;
      self->UpdateCaps(width, height, (fps_numerator / fps_denominator));
    }
  }
  return GST_PAD_PROBE_OK;
}

static GstFlowReturn new_sample_publish (GstElement *sink, gpointer data)
{
    DeviceVideoManager *self = (DeviceVideoManager *)data;
    self->PullBuffer(DeviceVideoManager::eVideoType::Publish);
    return GST_FLOW_OK;
}

static GstFlowReturn new_sample_internal (GstElement *sink, gpointer data)
{
    DeviceVideoManager *self = (DeviceVideoManager *)data;
    self->PullBuffer(DeviceVideoManager::eVideoType::Internal);
    return GST_FLOW_OK;
}

bool DeviceVideoManager::Start(int width, int height, const std::string &format)
{
    _width = width;
    _height = height;
    _publishFormat = format;
    _screenSharing = false;

//    std::string pipelineStr = "v4l2src device=/dev/video0 ! tee name=t ! queue ";
//               pipelineStr += " ! videoconvert ! videoscale ! video/x-raw,format=" +
//                              format + ",framerate=" +
//                              std::to_string(_fps) + "/1,width=" +
//                              std::to_string(_width) + ",height=" + std::to_string(_height) +
//                              " ! appsink name=appsink0 t. ! queue " +
//                              " ! videoconvert ! videoscale ! video/x-raw,format=RGBA,framerate=" +
//                              std::to_string(_fps) + "/1,width=" +
//                              std::to_string(_width) + ",height=" + std::to_string(_height) +
//                              " ! appsink name=appsink1";

    std::string pipelineStr = "v4l2src device=/dev/video0 ! video/x-raw,width=";
                    pipelineStr += std::to_string(_width) + ",height=" + std::to_string(_height) +
                    " ! videoconvert n-threads=4 ! video/x-raw,format=" + _publishFormat +
                    " ! appsink drop=true max-buffers=60 name=appsink0";



    return StartInternal(pipelineStr);
}

bool DeviceVideoManager::Start(const std::string& format, const DesktopShareOptions& desktopOptions)
{
    _publishFormat = format;
    _desktopOptions = desktopOptions;
    _screenSharing = true;

    std::string pipelineStr = "ximagesrc startx=";
                pipelineStr += std::to_string(desktopOptions.start_x) + " starty=" + std::to_string(desktopOptions.start_y) + " endx=" +
                std::to_string(desktopOptions.end_x) + " endy=" + std::to_string(desktopOptions.end_y) + " ! video/x-raw,framerate=" + std::to_string(desktopOptions.fps) + "/1" +
                " ! videoconvert n-threads=4 ! video/x-raw,format=" + _publishFormat +
                " ! appsink drop=true max-buffers=60 name=appsink0";

    return StartInternal(pipelineStr);
}

void DeviceVideoManager::Stop()
{
    StopInternal();
    _retryCount = cRetryCount;
}

void DeviceVideoManager::StartVideo()
{
    emit videoStarted(_width, _height, _screenSharing);
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
    emit videoError(error, _screenSharing);
}

void DeviceVideoManager::UpdateCaps(int width, int height, int fps)
{
    _width = width;
    _height = height;
    _fps = fps;

    emit capsUpdated(width, height, fps, _screenSharing);
}

int DeviceVideoManager::RemainingRetryCount()
{
    return _retryCount--;
}

void DeviceVideoManager::GstTimerFunc()
{
    if (_loop != NULL)
    {
        g_main_loop_run(_loop);
    }
}

bool DeviceVideoManager::StartInternal(const std::string &pipelineStr)
{
    gst_init(NULL, NULL);

    qDebug() << QString::fromStdString(pipelineStr);

    _pipeline = gst_parse_launch(pipelineStr.c_str(), NULL);
    if (_pipeline == NULL)
    {
        emit videoError("Erroneous pipeline", _screenSharing);
        return false;
    }

    _appsinkPublish = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink0");
//    _appsinkInternal = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink1");

    GstPad *sink_pad = gst_element_get_static_pad(_appsinkPublish, "sink");
    gst_pad_add_probe (sink_pad, (GstPadProbeType) (GST_PAD_PROBE_TYPE_QUERY_DOWNSTREAM | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
                       (GstPadProbeCallback) sink_pad_probe, this, NULL);

    gst_object_unref (sink_pad);

    GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        emit videoError("Failed to start pipeline", _screenSharing);
        gst_object_unref(_pipeline);
        _pipeline = NULL;
        return false;
    }

    _loop = g_main_loop_new(NULL, FALSE);

    // add a message handler
    _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
    _bus_watch_id = gst_bus_add_watch(_bus, bus_call, this);
    gst_object_unref(_bus);

    g_object_set (G_OBJECT (_appsinkPublish), "emit-signals", TRUE, NULL);
    g_signal_connect (_appsinkPublish, "new-sample", G_CALLBACK (new_sample_publish), this);

//    g_object_set (G_OBJECT (_appsinkInternal), "emit-signals", TRUE, NULL);
//    g_signal_connect (_appsinkInternal, "new-sample", G_CALLBACK (new_sample_internal), this);

    std::thread t(&DeviceVideoManager::GstTimerFunc, this);
    t.detach();

    return true;
}

void DeviceVideoManager::StopInternal()
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

DesktopShareOptions DeviceVideoManager::desktopOptions() const
{
    return _desktopOptions;
}

void DeviceVideoManager::setDesktopOptions(const DesktopShareOptions &desktopOptions)
{
    _desktopOptions = desktopOptions;
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
        emit publishVideoFrame(map.data, map.size, map.size / _height, _screenSharing);
    }
    else
    {
        emit internalVideoFrame(map.data, map.size, map.size / _height, _screenSharing);
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
}

void DeviceVideoManager::Retry()
{
    StopInternal();
    if (_screenSharing)
    {
        Start(_publishFormat, _desktopOptions);
    }
    else
    {
        Start(cDefaultWidth, cDefaultHeight, _publishFormat);
    }
}
