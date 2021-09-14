//
// Created by root on 20.05.20.
//

#ifndef TEEVID_SDK_ITEEVIDCLIENT_H
#define TEEVID_SDK_ITEEVIDCLIENT_H

#endif //TEEVID_SDK_ITEEVIDCLIENT_H
#include <string>
#include <memory>
#include <vector>
#include "ITeeVidClientObserver.h"
#include "IStreamSubscriber.h"
#include "SourceType.h"
#include "LocalVideoMode.h"
#include "TeeVidSettings.h"
#include "LogLevel.h"
#include "RoomParameters.h"
#include "SourceMode.h"
#include "IFrameModifier.h"
#include "DesktopShare.h"
#include "FileInfo.h"

namespace teevid_sdk {
    struct ITeeVidClient {
        virtual ~ITeeVidClient() = default;

        // Initialize SDK with token, provided by TeeVid support. Also need to provide
        // DNS name of media server and pointer to Observer class
        virtual bool Initialize(const std::string& token, const std::string& server, LogLevel logLevel,
            ITeeVidClientObserver* clientObserver, SourceMode sourceMode = kInternalSourceMode) = 0;

        // This method should be called if client need to change default settings
        virtual void Configure(const TeeVidSettings&) = 0;

        // This method performs connection to the room. Need to provide:
        // roomName - uniq name of room, room should be already created from admin panel
        // name - name of caller
        // password - caller's password
        // accessPin - pin for entering the room (can be empty)
        // ownerPin - pin for obtaining owner role (can be empty)
        // sendAudio - should audio be sent or mute it (true dy default)
        // sendVideo - should video be sent or mute it (true dy default)
        virtual void ConnectTo(const std::string& roomName, const std::string& name, const std::string& password,
                               int accessPin, int ownerPin, bool sendAudio, bool sendVideo, IFrameModifier* frameModifier) = 0;

        // By this method, client can enter the room by invitation token received from another client
        // Need to provide invitation token and own name
        virtual void ConnectTo(const std::string& invitationToken, const std::string& roomName,
                const std::string& name, const std::string& password, bool sendAudio, bool sendVideo, IFrameModifier* frameModifier) = 0;

        // Each stream which return by OnStreamAdded callback can e subscribed by client's application
        // Subscriber provides callback interface which wil be used to deliver media data

        // videoSettings parameter contains the resolution to which incoming video should be resized
        // to prevent resizing (receiving video as is) videoWidth and videoHeight fields in videoSettings should be set to 0
        virtual bool Subscribe(long streamId, MediaSettings& mediaSettings, IStreamSubscriber* streamSubscriber) = 0;
        virtual void Unsubscribe(long streamId) = 0;

        // Allows to change settings (e.g. video format) for the video from already subscribed stream - without stopping
        virtual void SetStreamMediaSettings(long streamId, MediaSettings& mediaSettings) = 0;

        virtual void ApplyTransform(long streamId, MediaSettings& settings) = 0;

        virtual void UpdateVideoCaps(int width, int height, int fps) = 0;

        // Disconnect from room and stop receiving/sending media
        virtual bool Disconnect() = 0;

        // Client is sending video by default.
        // This method allow to stop sending video but still receiving video from others
        virtual bool SendVideo(bool) = 0; // publish / un-publish

        // Client is sending audio by default.
        // This method allow to mute own audio but still receiving audio from others
        virtual bool SendAudio(bool) = 0;

        // In lecture mode, send special signal to the lecturer to start speaking
        virtual bool RaiseHand(bool) = 0;

        // By default, first video/audio  source is using for publishing.
        // Client can select media sources from the list of available sources.
        // This method allows to show list of available resources
        virtual std::vector<SourceInfo> GetSources(SourceType) = 0;

        // This method allows to select source from the list, returned by GetSources()
        virtual bool ChangeSource(SourceType, int deviceId) = 0;

        // Those methods take raw media data and it's size to deliver it to the server
        // Application should provide data in format which has been configured by Configure method
        // In case of video frames the source can be webcam video or desktop sharing
        virtual bool PutVideoFrame(unsigned char *data, size_t size, size_t stride, StreamType eSourceType = eWebCam) = 0;
        virtual bool PutAudioFrame(unsigned char *data, size_t size, StreamType eSourceType = eWebCam) = 0;

        // Screen sharing alongside webcam video publishing
        // When using an external pipeline no need to specify screen sharing options as they are already applied
        virtual void StartScreenSharing (const MediaSettings& media_settings) = 0;

        // When internal pipeline is used it is required to specify the sharing settings
        virtual void StartScreenSharing (const MediaSettings& media_settings, const DesktopShareOptions& options) = 0;

        virtual void StopScreenSharing () = 0;

        // Personal repository methods
        virtual std::vector<FileInfo> GetUserFileList () = 0;
    };

    typedef std::shared_ptr<ITeeVidClient> ITeeVidClientPtr;

    struct TeeVidFactory {
        static ITeeVidClientPtr CreateTeeVidClient();
    };
}