//
// Created by root on 20.05.20.
//

#ifndef TEEVID_SDK_ITEEVIDCLIENT_H
#define TEEVID_SDK_ITEEVIDCLIENT_H

#endif //TEEVID_SDK_ITEEVIDCLIENT_H
#include <string>
#include <memory>
#include "ITeeVidClientObserver.h"
#include "IStreamSubscriber.h"
#include "SourceType.h"
#include "LocalVideoMode.h"
#include "TeeVidSettings.h"

namespace teevid_sdk {
    struct ITeeVidClient {
        virtual ~ITeeVidClient() = default;

        // Initialize SDK with token, provided by TeeVid support. Also need to provide
        // DNS name of media server and pointer to Observer class
        virtual bool Initialize(const std::string& token, const std::string& server,
            ITeeVidClientObserver* clientObserver) = 0;

        // This method should be called if client need to change default settings
        virtual void Configure(const TeeVidSettings&) = 0;

        // This method performs connection to the room. Need to provide:
        // roomName - uniq name of room, room should be already created from admin panel
        // name - name of caller
        // accessPin - pin for entering the room (can be empty)
        // ownerPin - pin for obtaining owner role (can be empty)
        virtual void ConnectTo(const std::string& roomName, const std::string& name,
                               int accessPin, int ownerPin) = 0;

        // By this method, client can enter the room by invitation token received from another client
        // Need to provide invitation token and own name
        virtual void ConnectTo(const std::string& invitationToken, const std::string& name) = 0;

        // Each stream which return by OnStreamAdded callback can e subscribed by client's application
        // Subscriber provides callback interface which wil be used to deliver media data
        virtual void Subscribe(long streamId, IStreamSubscriber* streamSubscriber) = 0;
        virtual void Unsubscribe(long streamId) = 0;

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

        // Client can show or hide local video. Also client can choose size of this video
        virtual bool ShowLocalVideo(LocalVideoMode) = 0;

        // By default, first video/audio  source is using for publishing.
        // Client can select media sources from the list of available sources.
        // This method allows to show list of available resources
        virtual std::vector<std::string> GetSources(SourceType) = 0;

        // This method allows to select source from the list, returned by GetSources()
        virtual bool ChangeSource(SourceType, int deviceId) = 0;
    };

    typedef std::shared_ptr<ITeeVidClient> ITeeVidClientPtr;

    struct TeeVidFactory {
        static ITeeVidClientPtr CreateTeeVidClient();
    };
}