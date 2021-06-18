//
//  TeeVidClient.h
//  TeeVid
//
//  Copyright © 2017-2019 cloudAYI. All rights reserved.
//

/**
 @header TeeVidClient.h
 @abstract TeeVid client header.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/**
 Name of participant attribute containing user name.
 
 Value of this attribute is a string.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_USERNAME;

/**
 Name of participant attribute containing flag indicating whether participant has audio stream.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_AUDIO;

/**
 Name of participant attribute containing flag indicating whether participant has video stream.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_VIDEO;

/**
 Name of participant attribute containing flag indicating whether participant has data stream.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_DATA;

/**
 Name of participant attribute containing flag indicating whether participant shares screen.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_SCREEN_SHARING;

/**
 Name of participant attribute containing flag indicating whether participant is a lecturer.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_LECTURE;

/**
 Name of participant attribute containing flag indicating whether participant is a local participant.
 
 Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_LOCAL;

/**
 Name of participant attribute containing quality of experience.
 
 Value of this attribute is a number between 1 and 5.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_QOE;

/**
 Name of participant attribute containing dictionary of flags indicating mute status of media channels.
 
 Each key in such dictionary represents media channed (audio or video), and value is a boolean indicating whether given channel is muted.
 
 @note This dictionary migh also contain special entry which contains another dictionary of mute flags indicating which channels were muted by moderator (vs muted locally by user).
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_IS_MUTED;

/**
 Name of participant attribute containing dictionary of flags indicating mute status of media channels when they muted by moderator.
 
 Each key in such dictionary represents media channed (audio or video), and value is a boolean indicating whether given channel is muted.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ATTRIBUTE_BY_MODERATOR;

/**
 Conference room not found error message.
 
 Error message indicating that conference room with requested id does not exist on target server.
 */
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ERR_ROOM_NOT_FOUND;

/**
Bad access pin error message.
 
Error message indicating that user provided incorrect room access pin.
*/
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ERR_BAD_ACCESS_PIN;

/**
No slots available error message.

Error message indicating current participants number reached "maximum participants" room's setting.
*/
FOUNDATION_EXPORT NSString *_Nonnull const TEEVID_ERR_NO_SLOTS;


/**
 Enumeration of local video view positions.
 
 This enumeration defines available positions for local video view in a parent view. Default position is TeeVidClientLocalViewPositionBottomRight.
 
 @note This enum is only being used if video layout is managed by TeeVidClient. If video layout is managed by application, value of corresponding property will be TeeVidClientLocalViewPositionUnknown.
 */
typedef NS_ENUM(NSInteger, TeeVidClientLocalViewPosition) {
    /// Unknown
    TeeVidClientLocalViewPositionUnknown,
    /// TopLeft
    TeeVidClientLocalViewPositionTopLeft,
    /// TopRight
    TeeVidClientLocalViewPositionTopRight,
    /// BottomLeft
    TeeVidClientLocalViewPositionBottomLeft,
    ///BottomRight
    TeeVidClientLocalViewPositionBottomRight
};

/**
Enumeration of recording state.
 
This enumeration defines recording state.
*/
typedef NS_ENUM(NSInteger, TeeVidClientRecordingState) {
    /// Inactive
    TeeVidClientRecordingStateInactive,
    /// Starting
    TeeVidClientRecordingStateStarting,
    /// Recording
    TeeVidClientRecordingStateRecording,
    /// Stopping
    TeeVidClientRecordingStateStopping,
    /// Stopped
    TeeVidClientRecordingStateStopped,
    /// Error
    TeeVidClientRecordingStateError
};


/**
Enumeration of meeting type.

This enumeration defines type of meeting user is trying to conect to.
*/
typedef NS_ENUM(NSInteger, MeetingType) {
    /// Default
    MeetingTypeDefault,
    /// RegularRoom
    MeetingTypeRegularRoom,
    /// Attendee
    MeetingTypeEventAttendee,
    /// Panelist
    MeetingTypeEventPanelist
};



@class TeeVidClient;
/**
 TeeVidClient delegate used to notify application about various events.
 
 @note This delegate is required and must be implemented by application which utilizes TeeVidClient. Client will use delegate methods to inform application when specific phases of call setup reached and/or conference state changes.
 */
@protocol TeeVidClientDelegate <NSObject>


/**
 Notifies application that conference room requires access PIN.
 
 Upon receiving this call, application might prompt user for conference room owner or guest PIN, or simply notify user about unability to connect to requested room. When application receives PIN, it should re-initiate connection using connectTo method with PIN parameter.
 
 @note This method will also be called in provided PIN is invalid.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didRequestAccessPIN:(nonnull NSString *)roomId;

/**
 Notifies application that client entered waiting room and is waiting for conference owner to connect.
 
 Upon receiving this call, application might ask user if he/she is the conference room owner and prompt to enter owner PIN. Alternatively, application can simply wait until owner joins the room. If user provides owner PIN, application should re-initate connection using connectTo method with PIN parameter.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didEnterWaitingRoom:(nonnull NSString *)roomId;

/**
 Notifies application that client left waiting room.
 
 Uplon receiving this call, application should clear everything waiting room related (e.g. dismiss owner PIN prompt).

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didLeaveWaitingRoom:(nonnull NSString *)roomId;

/**
 Notifies application that client successfully connected to requested conference room.
 
 Upon receiving this call, application can make any appropriate UI transition and enable call control items (such mute/unmute button, etc.)

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didConnect:(nonnull NSString *)roomId;

/**
 Notifies application that client has disconnected from conference room it was connected to.
 
 Upon receiving this call, application can make any appropriate UI transition and disable call control items (such as mute/unmute button, etc.)

 @param client instance of the client this call came from
 @param roomId conference room id
 @param reason text explanation of disconnect action
 */
- (void)client:(nonnull TeeVidClient *)client didDisconnect:(nonnull NSString *)roomId reason:(nullable NSString *)reason;

/**
 Notifies application about SmartJoin page needed to be shown.
 
 Upon receiving this call, application should show SmartJoin page where user can see themself, and can set camera and mic settigns they want (this is the required part of regular room connection flow).

 @param client instance of the client this call came from
 @param publisherCount number of already conected participants who are publishing their audio/video stream
 */
- (void)client:(nonnull TeeVidClient *)client showSmartJoinWithPublisherCount:(NSInteger)publisherCount;

/**
 Notifies application about Event type of room appication is trying to connect to.
 
 Upon receiving this call, application should show pre-Event page according to current status details received (instead of regular SmartJoin page).

 @param client instance of the client this call came from
 @param params dictionary with Event details
 @param credentialRequirements dictionary with credential requirements details
 */
- (void)client:(nonnull TeeVidClient *)client showPreEventScreenWithParams:(nonnull NSDictionary *)params credentialRequirements:(nonnull NSArray <NSString *> *)credentialRequirements;

/**
 Notifies application about changed Event state details.
 
 Upon receiving this call, application can update pre-Event page with changed status (Event started, terminated, etc).

 @param client instance of the client this call came from
 @param params dictionary with Event details
 */
- (void)client:(nonnull TeeVidClient *)client didUpdateEventStateWithParams:(nonnull NSDictionary *)params;

/**
 Notifies application that attendee or panelist pin verification passed.
 
 Upon receiving this call, application can allow attendee or panelist join to Event.

 @param client instance of the client this call came from
 */
- (void)clientEventPersonVerificationPassed:(nonnull TeeVidClient *)client;

/**
 Notifies application that client received a poll updates information.
 
 Upon receiving this call, application can show screen or popup with poll information (topic and options) - and user can vote.

 @param client instance of the client this call came from
 @param params dictionary with poll details
 */
- (void)client:(nonnull TeeVidClient *)client didUpdatePollWithParams:(nonnull NSDictionary *)params;

/**
 Notifies application that client received an error after being connected connected to conference room.
 
 Typically receiving an error would trigger disconnection from conference room. Upon receiving this call, application might present error message to the user.
 
 @note Error messages are typically in free text format and are not localized. Subset of well defined error message can be found in constant section of this class.

 @param client instance of the client this call came from
 @param error error message
 */
- (void)client:(nonnull TeeVidClient *)client didReceiveError:(nonnull NSString *)error;

/**
 Notifies application that layout refresh is requested.
 
 Application is required to react on this message by calling client's refreshLayout method. This call can be wrapped into any kind of animation or any other effects applicable to a view passed to the client during its initialization.
 
 @note Reason parameters is passed for informational purposes only.
 @note Client will send this notification only when video layout is managed by client.

 @param client instance of the client this call came from
 @param reason reason for layout refresh request
 */
- (void)client:(nonnull TeeVidClient *)client didRequestLayoutRefresh:(nonnull NSString *)reason;

/**
 Notifies application that client entered lecture mode.
 
 When conference room is in lecture mode, client automatically mutes itself (stop sending both audio and video) and changes video layout to a single video if lecturer presentation or video stream. While this behavior cannot be overridden, upon receiving this call, application can take additional steps appropriate to lecture mode UI

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didEnterLectureMode:(nonnull NSString *)roomId;

/**
 Notifies application that client left lecture mode.
 
 Upon receving this call, application can take any steps to transition from lecture mode to regular conference room UI

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didLeaveLectureMode:(nonnull NSString *)roomId;

/**
 Notifies application that conference room is in lecture mode, but lectured did not join the room yet.
 
 Upon receiving this call, application can take any appropriate action (e.g. display "waiting" message). When lecturer arrives, client will notify application by didEnterLectureMode call.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didEnterWaitingForLecturer:(nonnull NSString *)roomId;

/**
 Notifies application that conference room is in lecture mode, but lecturer has left the room.
 
 Upon receiving this call, application can promt user for appropriate action (e.g. stay in the call without lecturer, or leave).

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didReceiveLecturerDisconnectEvent:(nonnull NSString *)roomId;

/**
 Notifies application that moderator wants to unmute current participant
 
 @param client instance of the client this call came from
 @param request dictionary contains information about the request. Describes what exactly should be unmuted a video or audio input
 @param completionHandler should be called when a user makes a choice about approving or discard the request. allowUnmute is a BOOL value that represent a user's answer
 */
- (void)client:(nonnull TeeVidClient *)client didRecieveUnmuteRequest:(nonnull NSDictionary*)request completionHandler:(nonnull void (^)(BOOL allowUnmute))completionHandler;

@optional

/**
 Notifies application that number of remote video streams received by client has changed.
 
 Reported number of streams does not necesserily match number of participants - some participants might not send video, some might send two video streams (video and presentation). Also, reported number of streams does not include local video stream.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param numberOfVideoStreams number of received video streams
 */
- (void)client:(nonnull TeeVidClient *)client didUpdateNumberOfVideoStreams:(int)numberOfVideoStreams;

/**
 Notifies application that new participant(s) joined room this client is connected to.
 
 See TEEVID_ATTRIBUTE constants for list of supported participant attributes.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param participantsData dictionary vith keys corresponding to participant ids and values to participant attributes (see TEEVID_ATTRIBUTE constants for list of supported participant attributes)
 */
- (void)client:(nonnull TeeVidClient *)client didAddParticipants:(nonnull NSDictionary *)participantsData;

/**
 Notifies application that new participant was added to the room client is connected to.
 
 See TEEVID_ATTRIBUTE constants for list of supported participant attributes.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param participantId participant id
 @param attributes participant attributes
 */
- (void)client:(nonnull TeeVidClient *)client didUpdateParticipant:(nonnull NSString *)participantId withAttributes:(nonnull NSDictionary *)attributes;

/**
 Notifies application that participant left room this client is connected to.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param participantId participant id
 */
- (void)client:(nonnull TeeVidClient *)client didRemoveParticipant:(nonnull NSString *)participantId;

/**
 Notifies application that size of video sent by specified participant has changed.
 
 When video layout if managed by application, application must use this event to deal with possible changes in aspect ratio.
 
 @note This selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param videoSize new video size
 @param participantId participant id
 @param view view where participant video is being rendered
 */
- (void)client:(nonnull TeeVidClient *)client didChangeVideoSize:(CGSize)videoSize forParticipant:(nonnull NSString *)participantId inView:(nonnull UIView *)view;

/**
 Notifies application that participant video view was removed.
 
 Typically view is removed when participant closes its audio and video streams, but stays connected to the conference room (remains an observer). When video layout if managed by application, application must use this event to remove view from video layout.
 
 @note This selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param view view where participant video was being rendered
 @param participantId participant id
 */
- (void)client:(nonnull TeeVidClient *)client didRemoveVideoView:(nonnull UIView *)view forParticipant:(nonnull NSString *)participantId;

/**
 Notifies application that client was muted by moderator or lecturer.
 
 Upon receiving this call, application can change the microphone icon to enabled or disabled state.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param muteAttributes variable that represents the muted state
 */
- (void)client:(nonnull TeeVidClient *)client didUpdateMuteAttributes:(nonnull NSDictionary *)muteAttributes;


/**
Notifies application that client was muted by system - HardMute.

Upon receiving this call, application can change the microphone icon to enabled or disabled state.

@note This selector is optional.

@param client instance of the client this call came from
@param muteAttributes variable that represents the muted state
*/
- (void)client:(nonnull TeeVidClient *)client didUpdateHardMuteAttributes:(nonnull NSDictionary *)muteAttributes;

/**
 The user mute mode was changed due to the use of raise hand feature.
 
 Upon receiving this call, application can handle a case with (un) muting current participant. Both current user and moderator can discard raised hand.
 
 @note This selector is optional.
 
 @param client instance of the client this call came from
 @param allowed whether raise hand request was approved by moderator and client was allowed to unmute microphone and/or start sending video
 */
- (void)client:(nonnull TeeVidClient *)client didUpdateRaiseHandStatus:(BOOL)allowed;

/**
 Notifies application that conference room entered into storyboard presentation mode.
 
 When storyboard is presented, it is being rendered in an extra view which is not associated with any of participant (though, technically, it is being presented by lecturer). Application can obtain that view similarly to another other view by using dedicated participant id passed in this notification.
 
 @note This selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param roomId conference room id
 @param participantId dedicated participant id associated with presented storyboard
 */
- (void)client:(nonnull TeeVidClient *)client didEnterStoryboardMode:(nonnull NSString *)roomId withDedicatedParticipantId:(nonnull NSString *)participantId;

/**
 Notifies application that conference room left storyboard presentation mode.
 
 When storyboard presentation has ended, presentation should release view associated with it (if such view was previousely obtained).
 
 @note This selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(nonnull TeeVidClient *)client didLeaveStoryboardMode:(nonnull NSString *)roomId;

/**
Notifies application about measured audio input level.

@note This selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).

@param client instance of the client this call came from
@param linearInputLevel audio input linear level in float value
*/
- (void)client:(nonnull TeeVidClient *)client didMeasureAudioInputLevel:(float)linearInputLevel;


/**
Notifies application about recording state (started / stopped recording).

@note This selector is optional.

@param client instance of the client this call came from
@param state recording state enum value
*/
- (void)client:(nonnull TeeVidClient *)client didUpdateRecordingState:(TeeVidClientRecordingState)state;


/**
Notifies application about updated user image (avatar thumbnail).

@note This selector is optional.

@param client instance of the client this call came from
@param userImage prepared user avatar thumbnail
*/
- (void)client:(nonnull TeeVidClient *)client didUpdateUserImage:(nullable UIImage *)userImage;


/**
Asks for already logged in user token (retrieved earlier with Middlware REST API).

@note This selector is optional.

@param client instance of the client this call came from
@param completion a callback  which delegate must call after fetching the user token
*/
- (void)client:(nonnull TeeVidClient *)client getLoggedInUserTokenWithCompletion:(nonnull  void (^)(NSString *_Nullable token))completion;

@end



/**
 TeeVid video conferencing client.
 
 This client provides ability to connect to conference rooms hosted on TeeVid servers.
 
 To support various call flows driven by configuration settings of specific conference room, consumer of this client must provide a delegate
 which will be called when specific phases of call setup reached and/or conference state changes.
 
 Typicall call flow would include:
 
 - Instantiate new instance of TeeVidClient (passing a delegate)
 
 - Initiate connection using one of the "connectTo" methods
 
 - React on various callbacks to a delegate
 
 - Disconnect from conference room when call is done.
 
 Depending on application design, client instance can be re-used for multiple calls, or, new instance can be created for each call.
 
 TeeVidClient can be instantiated in two different modes: in first mode video layout is controlled by client, in second mode - by application.
 
 When video layout is controlled by client, video of conference room participants is rendered in a view provided by application. Application is in control of size and position of that view, but, layout of multiple videos in the provided view is controlled by the client itself. Application can choose one of the two video layouts:
 
 - Automatic. In this layout, client will render video from multiple participants and change layout automatically based on number of participants
 
 - Single video. In this layout, client will display video from only one participants, and, application has control on whose video is displayed.
 
 Application can also hide/show local video view and control its position.
 
 In addition to layouts above, client supports so-called "lecture mode". When conference room is in lecture mode, client becomes muted (stops sending both audio and video), and, only lecturer's presentation video stream is diplayed. Application cannot change video layout or unmute client while in lecture mode.
 
 When video layout is controlled by application, application can obtain views where video of each participant is being rendered, and, then, have full control of their position, size and aspect ratio. To instantiate client in such mode, application should use overloaded init method without passing view to render participants video in.

 Below is an example of initializing TeeVidClient in ViewController and initiating connection to a conference room.
 ````
 - (void)viewDidLoad {
     [super viewDidLoad];
     
     // Initialize TeeVidClient in a mode when video layout is managed by client
     // Pass whatever view you want video to be rendered in
     CGRect bounds = self.view.frame;
     CGRect frame = CGRectMake(bounds.origin.x, bounds.origin.y + 20, bounds.size.width, bounds.size.height - 64);
     UIView *conferenceView = [[UIView alloc] initWithFrame:frame];
     [self.view addSubview:conferenceView];
     teeVidClient = [[TeeVidClient alloc] initWithView:conferenceView server:serverAddress room:roomId userName:userName options:nil andDelegate:self];
 }
 ````
 */
@interface TeeVidClient : NSObject

/**
 Contains address (typically DNS name) of the server hosting target conference room.
 */
@property (nonatomic, readonly) NSString *_Nonnull serverAddress;

/**
 Contains version of MCU that is deployed on the server
 */
@property (nonatomic, readonly) NSString *_Nonnull serverMCUVersion;

/**
 Contains version of the server hosting target conference room.
 */
@property (nonatomic, readonly) NSString *_Nonnull serverMiddlewareVersion;

/**
 Contains version of the moderation server for target conference room.
 */
@property (nonatomic, readonly) NSString *_Nonnull serverModerationVersion;

/**
 Contains id of the conference room to connect to.
 */
@property (nonatomic, readonly) NSString *_Nonnull roomId;

/**
 Contains participant name.
 */
@property (nonatomic, readonly) NSString *_Nonnull userName;

/**
 Contains connection options.
 */
@property (nonatomic, readonly) NSDictionary *_Nonnull options;

/**
 Contains number of remote video streams client is receiving.
 
 @note Reported number of streams does not necesserily match number of participants - some participants might not send video, some might send two video streams (video and screen sharing).
 @note Reported number of streams does not include local video stream.
 */
@property (nonatomic, readonly) int numberOfVideoStreams;

/**
 Contains index of video stream currently shown in a single stream layout.
 
 @note If current layout is automatic, the value of this index would be -1.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be -1.
 */
@property (nonatomic, readonly) int singleStreamLayoutIndex;

/**
 Reports whether video view is currently in automatic layout.
 @note When layout is in automatic mode, client will render up to 6 participant videos in a view provided by application.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be FALSE.
 */
@property (nonatomic, readonly) BOOL automaticLayout;

/**
 Reports whether client is currently connected to a conference room.
 */
@property (nonatomic, readonly) BOOL connected;

/**
 Reports whether client's audio is currently muted.
 */
@property (nonatomic, readonly) BOOL muted;

/**
 Reports whether client's video is currently stopped (client not sending video).
 */
@property (nonatomic, readonly) BOOL videoStopped;

/**
 Reports whether client is currently in lecture mode.
 
 @note While in lecture mode, client does not send audio and video, and, videw view contains single video of lecturer's presentation or video stream.
 */
@property (nonatomic, readonly) BOOL lectureMode;

/**
 Reports whether client is currently in lecture mode, but, lecturer did not join the room yet.
 
 @note While is such state, client does not send audio and video, and, video view does not render any video stream.
 */
@property (nonatomic, readonly) BOOL waitingForLecturer;

/**
 Contains reference to a view this client was initialized with.
 
 @note Client will render one or multiple participant videos in this view.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be nil.
 */
@property (nonatomic, strong, readonly) UIView *_Nullable view;

/**
 Contains reference to a local video view,.
 
 @note When video layout is managed by client, this view is a subview of a parent view this client was initialized with. Application can use this property to register gesture recognizer for local video view.
 @note Local video view is only available when client is connected to remote room.
 @note Local video view is hidden when client's video is stopped.
 @note This property is still available if video layout is managed by application itself, but, in this mode client does not control local video view position, size and aspect ratio.
 */
@property (nonatomic, strong, readonly) UIView *_Nonnull localView;

/**
 Contains position of a local video view in a parent view.
 
 @note Local view position is not applicable while there is only local client connected - in this case local video view fills parent view frame completely.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be TeeVidClientLocalViewPositionUnknown.
 */
@property (nonatomic, assign) TeeVidClientLocalViewPosition localViewPosition;

/**
 Reports whether local video is currently hidden.
 
 @note Application can hide local video using corresponding selector. While hidden, it will be replaced by semi-transparent image which allows to process gesture events, e.g. show local video after tap.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be always FALSE.
 */
@property (nonatomic, readonly) BOOL localVideoHidden;

/**
 Contains placeholder image for local video to be shown when local video is hidden.
 
 @note Application can hide local video using corresponding selector. While hidden, it will be replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap. This property contains custom image to be used as local video placeholer.
 
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value set to this property is ingored.
 */
@property (nonatomic, strong) UIImage *_Nullable localVideoPlaceholder;

/**
 Reports whether video layoyt is managed by TeeVidClient.
 
 @note Application can instantiate TeeVidClient in two different ways: provide client with a view to render participants video in and let client manage video layout, or, have application reposible for all aspects of video layoyut management. This property reports which mode TeeVidClient was instantiated with.
 */
@property (nonatomic, readonly) BOOL managingVideoLayout;

/**
Contains prepared user avatar thumbnail.

@note Application can use this image to tisplay user image thumbnail.
*/
@property (strong, nonatomic, readonly) UIImage *_Nullable localUserAvatar;

/**
Indicates whether local user is room owner or has moderator role.
 
@note Application can use this flag to show additional functionality available for room owner or room moderator.
*/
@property (assign, nonatomic, readonly, getter=isUserOwnerOrModerator) BOOL userIsOwnerOrModerator;

/**
Creates new instance of TeeVidClient.
 
 If  view is not nil than TeeVidClient will manage participants video layout inside of a view provided by application. Application will be able to control only certain elements of video layout: switch between mutliple and single video layouts, iterate video streams in single video layout, control position of local video.

 If application needs more fine grained control of video layout, pass nil for view parameter. In such mode, application can obtain views where participant video is being rendered for individual participants, and then size and position them on the screen on its own.
 
 @note Delegate parameter is required.
 @note options parameter is reserved for future use.
 
 @param viewOrNil view to render conference room participants videos in or nil
 @param server server address (typically DNS name)
 @param roomId conference room id
 @param userName userName participant name
 @param options conference room options (reserved for future use)
 @param delegate TeeVidClient delegate
 @return new istance of TeeVidClient
 */
- (nonnull instancetype)initWithView:(nullable UIView *)viewOrNil server:(nonnull NSString *)server room:(nonnull NSString *)roomId userName:(nullable NSString *)userName options:(nullable NSDictionary *)options andDelegate:(nonnull id <TeeVidClientDelegate>)delegate;

/**
Connects to a conference room.
 
This method is typically used when user initiates connection. Connection is performed asynchronously after all parameters are validated. Any change in connection state (including errors) will be reported to application using appropiate delegate methods.
 
@note If user does not know whether room is PIN protected, connection can be initiated without PIN. In such case, if client detects that room requires PIN to connect, it will request PIN by calling approrpiate delegate method.

@param server server address (typically DNS name)
@param roomId conference room id
@param userName userName participant name
@param meetingType enum to specify type of meeting user is connecting to
@param pin optional conference room owner or guest PIN
*/
- (void)connectToServer:(nonnull NSString *)server room:(nonnull NSString *)roomId asUser:(nonnull NSString *)userName meetingType:(MeetingType)meetingType withAccessPin:(nullable NSString *)pin;

/**
 Connects to a conference room.
 
 This method is typically used when application receives invitation link or other type of invite containing invitation token. Passing valid invitation token allows application to connect to PIN protected room without knowing its PIN. Connection is performed asynchronously after all parameters are validated. Any change in connection state (including errors) will be reported to application using appropiate delegate methods.
 
 @warning Each invitation token can be only used one time.
 
 @param server server address (typically DNS name)
 @param roomId conference room id
 @param userName userName participant name
 @param meetingType enum to specify type of meeting user is connecting to
 @param invitationToken invitation token
 */
- (void)connectToServer:(nonnull NSString *)server room:(nonnull NSString *)roomId asUser:(nonnull NSString *)userName meetingType:(MeetingType)meetingType withInvitationToken:(nullable NSString *)invitationToken;


/**
Connects to a conference via intermediate SmartJoin room.

 This method is typically used to show user the intermediate room (before being connected to actual conference). Here user can set up and check audio level, local camera picture, change camera (front <> back), turn off/on camera or video. After SmartJoin room the main "connectTo:" method should be used to actually connect to the conference room.

@param userName userName participant name
*/
- (void)connectToSmartJoinScreenAs:(nonnull NSString *)userName;

/**
 Creates invitation token.
 
 This method can be used to generate invitation token in order to invite someone to a conference room. Application then is responsible to deliver such invitation using out of band channel (instant message, email, etc.)
 
 @note In order to generate invitation token, client must be successfully connected to a conference room first.

 @return invitation token
 */
- (nullable NSString *)createInvitationToken;

/**
Creates invitation token.

This method can be used to generate invitation token in order to invite someone to a conference room. Application then is responsible to deliver such invitation using out of band channel (instant message, email, etc.)

@note In order to generate invitation token, client must be successfully connected to a conference room first.

@param completion callback which is called after the invitation token (cintained in invitationToken prameter) is generated
*/
- (void)generateInvitationTokenWithCompletion:(nonnull void (^)(NSString *_Nullable invitationToken))completion;

/**
 Disconnects from the conference room.
 
 Application should use this method to disconnect client from the conference room.
 
 @warning Application must make all efforts to disconnect client gracefully, even when terminated abruply.
 */
- (void)disconnect;

/**
 Mutes client's audio.
 
 Application can use this method to mute local audio.
 */
- (void)mute;

/**
 Unmutes client's audio.
 
 Application can use this method to unmute local audio.
 
 @warning When client is in lecture mode, this functionality is not avalilable.
 */
- (void)unmute;

/**
 Stops sending video.
 
 Application can use this method to stop sending video to the conference room.
 */
- (void)stopVideo;

/**
 Resumes sending video.
 
 Application can use this method to resume sending video to the conference room.
 
 @warning When client is in lecture mode, this functionality is not avalilable.
 */
- (void)resumeVideo;

/**
 Switches front and back cameras.
 
 Application can use this method to switch video from front to back camera (or from bacl to front camera). Initially from camera is being used. If device has only one camera (or no cameras), no action is performed.
 */
- (void)switchCamera;

/**
 Allows a user to request unmuting during a lecture mode.
 
 Sends action named "raise-hand" and the participant's id.
 
 @param raise wether hand is rised or not
 */
- (void)raiseHand:(BOOL)raise;

/**
 Switches video view to show single video stream layout of given index.
 
 When video layout is managed by client, application can use this method to switch video layout from automatic to single stream. If participant with provided stream index leaves the conference, client will automatically switch to display video of the stream with next available index. If there is no stream with target index, first video stream from participant list will be displayed.
 
 @warning This functionality is not avalilable when video layout is managed by application itself, or when client is in lecture mode.
 
 @param streamIndex streamIndex index of video stream to show
 */
- (void)switchToSingleStreamLayout:(int)streamIndex;

/**
 Switches video view from single video to automatic layout.
 
 When video layout is managed by client and it is in automatic layout mode, client will render video from multiple participants and change layout automatically based on number of participants.
 
 @warning This functionality is not avalilable when video layout is managed by application itself, or when client is in lecture mode.
 */
- (void)switchToAutomaticLayout;

/**
 Refreshes layout of videos rendered in view view.
 
 When video layout is managed by client, application will be notified by client's delegate didRequestLayoutRefresh method every time when video layout changes. Upon receving such notification, application must call refreshLayout method. It can wrap this call into any desired animation or use any other effects to make layout transition smooth.
 
 @warning Application must always call refreshLayout method when notified about layout change. Failure to call this method will result client rendering obsolete layout in video view.
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)refreshLayout;

/**
 Hides local video.
 
 When video layout is managed by client, application can use this method to hide local video. While hidden, it will be replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap.
 
 @warning Before hiding local video, application must set property containing placeholder image to be shown in place of local video.
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)hideLocalVideo;

/**
 Shows local video view.
 
 When video layout is managed by client, application can use this method to show local video after it was hidden.
 
 @note While hidden, local video is replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap.
 
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)showLocalVideo;

/**
 Retrieves view where video of specified participant is rendered.
 
 When video layout is managed by client, this view is a a subview of a parent view this client was initialized with. Otherwise, new view is created from video stream, and, then, aplication has full control on its position and size.
 
 @note One participant can send two vdieo streams simultaneously - actual video stream, and screen sharing (aka presentation) stream. To retrieve view where screen sharing stream is being rendered, video parameter should be set to FALSE.
 
 @warning When video layoout is managed by client, application must not attempt changing position or size of individual participant video views.
 
 @param participantId participantId id of participant to retrieve video of
 @param video whether to retrieve video view (vs screen sharing)
 @return reference to a view where video of individual participant is being rendered in, or nil if specified participant does not send video stream
 */
- (nullable UIView *)getViewForParticipant:(nonnull NSString *)participantId video:(BOOL)video;

/**
 Retrieves ids of participants selected by lecturer to be included in video layout during lecture mode.
 
 Non-empty list indicates that lecturer selected specific video layout with given number of participants in it. Some slots might be empty, it is indicated by special entry in the list - "stub". This list might or might not include lecturer itself.
 Empty list indicates "automatic" layout during lecture mode. In such case, lecturer is always included. Other participants might be also included if lecturer accepted their "raise hand" request.
 
 @note While in lecture mode, lecturer can designate some participants to be included in video layout.
 
 @return list of participant ids to included in video layout during lecture mode
 */
- (nullable NSArray *)getLecturerSelectedParticipants;

/**
 Mutes client's audio in SmartJoin room.
 
 Application can use this method to mute local audio in SmartJoin room. This state will be used furhter after user connected to the meeting
*/
- (void)smartJoinMuteMic;

/**
 Unmutes client's audio in SmartJoin room.
 
 Application can use this method to unmute local audio in SmartJoin room. This state will be used furhter after user connected to the meeting
*/
- (void)smartJoinUnmuteMic;

/**
 Stops client's video in SmartJoin room.
 
 Application can use this method to stop local video in SmartJoin room. This state will be used furhter after user connected to the meeting
*/
- (void)smartJoinStopVideo;

/**
 Resumes client's video in SmartJoin room.
 
 Application can use this method to resume local video in SmartJoin room. This state will be used furhter after user connected to the meeting
*/
- (void)smartJoinResumeVideo;

/**
 Switches front and back cameras in SmartJoin room.
 
 Application can use this method to switch video from front to back camera (or backward). Initially from camera is being used. This selected camera will be used furhter after user connected to the meeting
*/
- (void)smartJoinSwitchCamera;

/**
 Notifies client about request to update user image thumbnail
 
 Application can use this method to request user image update. Actual avatar update will no be performed if local video is stopped.
 The resulted image thumbnail will be returned via TeeVidClientDelegate protocol's method [client: didUpdateUserImage:]
*/
- (void)userImageUpdateRequested;

/**
 Sends user vote
 
 Application can use this method to send user's vote with specific option they picked.

 @param pollId current poll id user is voting for
 @param optionName specific option user is voting with
 */
- (void)voteInPoll:(nonnull NSString *)pollId withOptionName:(nonnull NSString *)optionName;

/**
 Sends attendee pin for verification
 
 Application can use this method to send attendee's access pin for Event meeting to be verified on server.

 @param pin access pin value in String format
 */
- (void)verifyAttendeePin:(nonnull NSString *)pin;

/**
 Sends panelist pin for verification
 
 Application can use this method to send panelist's access pin for Event meeting to be verified on server.

 @param pin access pin value in String format
 */
- (void)verifyPanelistsPin:(nonnull NSString *)pin;

@end
