//
//  TeeVidClient.h
//  TeeVid
//
//  Copyright © 2017-2019 cloudAYI. All rights reserved.
//

/*!
 @header TeeVidClient.h
 @abstract TeeVid client header.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/*!
 @brief Name of participant attribute containing user name.
 @discussion Value of this attribute is a string.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_USERNAME;

/*!
 @brief Name of participant attribute containing flag indicating whether participant has audio stream.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_AUDIO;

/*!
 @brief Name of participant attribute containing flag indicating whether participant has video stream.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_VIDEO;

/*!
 @brief Name of participant attribute containing flag indicating whether participant has data stream.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_DATA;

/*!
 @brief Name of participant attribute containing flag indicating whether participant shares screen.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_SCREEN_SHARING;

/*!
 @brief Name of participant attribute containing flag indicating whether participant is a lecturer.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_LECTURE;

/*!
 @brief Name of participant attribute containing flag indicating whether participant is a local participant.
 @discussion Value of this attribute is a boolean.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_LOCAL;

/*!
 @brief Name of participant attribute containing quality of experience.
 @discussion Value of this attribute is a number between 1 and 5.
 @constant TEEVID_ATTRIBUTE_QOE
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_QOE;

/*!
 @brief Name of participant attribute containing dictionary of flags indicating mute status of media channels.
 @discussion Each key in such dictionary represents media channed (audio or video), and value is a boolean indicating whether given channel is muted.
 
 Note that this dictionary migh also contain special entry which contains another dictionary of mute flags indicating which channels were muted by moderator (vs muted locally by user).
 @constant TEEVID_ATTRIBUTE_IS_MUTED
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_IS_MUTED;

/*!
 @brief Name of participant attribute containing dictionary of flags indicating mute status of media channels when they muted by moderator.
 @discussion Each key in such dictionary represents media channed (audio or video), and value is a boolean indicating whether given channel is muted.
 @constant TEEVID_ATTRIBUTE_BY_MODERATOR
 */
FOUNDATION_EXPORT NSString *const TEEVID_ATTRIBUTE_BY_MODERATOR;

/*!
 @brief Conference room not found error message.
 @discussion Error message indicating that conference room with requested id does not exist on target server.
 */
FOUNDATION_EXPORT NSString *const TEEVID_ERR_ROOM_NOT_FOUND;


/*!
 @brief Enumeration of local video view positions.
 @discussion This enumeration defines available positions for local video view in a parent view. Default position is TeeVidClientLocalViewPositionBottomRight.
 
 Note that this enum is only being used if video layout is managed by TeeVidClient. If video layout is managed by application, value of corresponding property will be TeeVidClientLocalViewPositionUnknown.
 */
typedef enum {
    TeeVidClientLocalViewPositionUnknown,
    TeeVidClientLocalViewPositionTopLeft,
    TeeVidClientLocalViewPositionTopRight,
    TeeVidClientLocalViewPositionBottomLeft,
    TeeVidClientLocalViewPositionBottomRight
} TeeVidClientLocalViewPosition;


@class TeeVidClient;

/*!
 @brief TeeVidClient delegate used to notify application about various events.
 @discussion This delegate is required and must be implemented by application which utilizes TeeVidClient. Client will use delegate methods to inform application when specific phases of call setup reached and/or conference state changes.
 */
@protocol TeeVidClientDelegate <NSObject>

/*!
 @brief Notifies application that conference room requires access PIN.
 @discussion Upon receiving this call, application might prompt user for conference room owner or guest PIN, or simply notify user about unability to connect to requested room. When application receives PIN, it should re-initiate connection using connectTo method with PIN parameter.
 
 Note that this method will also be called in provided PIN is invalid.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didRequestAccessPIN:(NSString *)roomId;

/*!
 @brief Notifies application that client entered waiting room and is waiting for conference owner to connect.
 @discussion Upon receiving this call, application might ask user if he/she is the conference room owner and prompt to enter owner PIN. Alternatively, application can simply wait until owner joins the room. If user provides owner PIN, application should re-initate connection using connectTo method with PIN parameter.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didEnterWaitingRoom:(NSString *)roomId;

/*!
 @brief Notifies application that client left waiting room.
 @discussion Uplon receiving this call, application should clear everything waiting room related (e.g. dismiss owner PIN prompt).

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didLeaveWaitingRoom:(NSString *)roomId;

/*!
 @brief Notifies application that client successfully connected to requested conference room.
 @discussion Upon receiving this call, application can make any appropriate UI transition and enable call control items (such mute/unmute button, etc.)

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didConnect:(NSString *)roomId;

/*!
 @brief Notifies application that client has disconnected from conference room it was connected to.
 @discussion Upon receiving this call, application can make any appropriate UI transition and disable call control items (such as mute/unmute button, etc.)

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didDisconnect:(NSString *)roomId;

/*!
 @brief Notifies application that client received an error after being connected connected to conference room.
 @discussion Typically receiving an error would trigger disconnection from conference room. Upon receiving this call, application might present error message to the user.
 
 Note that error messages are typically in free text format and are not localized. Subset of well defined error message can be found in constant section of this class.

 @param client instance of the client this call came from
 @param error error message
 */
- (void)client:(TeeVidClient *)client didReceiveError:(NSString *)error;

/*!
 @brief Notifies application that layout refresh is requested.
 @discussion Application is required to react on this message by calling client's refreshLayout method. This call can be wrapped into any kind of animation or any other effects applicable to a view passed to the client during its initialization.
 
 Note that reason parameters is passed for informational purposes only.
 Note also that client will send this notification only when video layout is managed by client.

 @param client instance of the client this call came from
 @param reason reason for layout refresh request
 */
- (void)client:(TeeVidClient *)client didRequestLayoutRefresh:(NSString *)reason;

/*!
 @brief Notifies application that client entered lecture mode.
 @discussion When conference room is in lecture mode, client automatically mutes itself (stop sending both audio and video) and changes video layout to a single video if lecturer presentation or video stream. While this behavior cannot be overridden, upon receiving this call, application can take additional steps appropriate to lecture mode UI

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didEnterLectureMode:(NSString *)roomId;

/*!
 @brief Notifies application that client left lecture mode.
 @discussion Upon receving this call, application can take any steps to transition from lecture mode to regular conference room UI

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didLeaveLectureMode:(NSString *)roomId;

/*!
 @brief Notifies application that conference room is in lecture mode, but lectured did not join the room yet.
 @discussion Upon receiving this call, application can take any appropriate action (e.g. display "waiting" message). When lecturer arrives, client will notify application by didEnterLectureMode call.

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didEnterWaitingForLecturer:(NSString *)roomId;

/*!
 @brief Notifies application that conference room is in lecture mode, but lecturer has left the room.
 @discussion Upon receiving this call, application can promt user for appropriate action (e.g. stay in the call without lecturer, or leave).

 @param client instance of the client this call came from
 @param roomId conference room id
 */
- (void)client:(TeeVidClient *)client didReceiveLecturerDisconnectEvent:(NSString *)roomId;

/*!
 @brief Notifies application that moderator wants to unmute current participant
 
 @param request dictionary contains information about the request. Describes what exactly should be unmuted a video or audio input
 @param completionHandler should called when a user make a choice about approving or discard the request. allowUnmute is a BOOL value that represent a user's answer
 */
- (void)client:(TeeVidClient *)client didRecieveUnmuteRequest:(NSDictionary*)request completionHandler:(void (^)(BOOL allowUnmute))completionHandler;

@optional

/*!
 @brief Notifies application that number of remote video streams received by client has changed.
 @discussion Reported number of streams does not necesserily match number of participants - some participants might not send video, some might send two video streams (video and presentation). Also, reported number of streams does not include local video stream.
 
 Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param numberOfVideoStreams number of received video streams
 */
- (void)client:(TeeVidClient *)client didUpdateNumberOfVideoStreams:(int)numberOfVideoStreams;

/*!
 @brief Notifies application that new participant joined room this client is connected to.
 @discussion See TEEVID_ATTRIBUTE constants for list of supported participant attributes.
 
 Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param participantId participant id
 @param attributes participant attributes
 */
- (void)client:(TeeVidClient *)client didAddParticipant:(NSString *)participantId withAttributes:(NSDictionary *)attributes;

/*!
 @brief Notifies application that new participant was added to the room client is connected to.
 @discussion See TEEVID_ATTRIBUTE constants for list of supported participant attributes.
 
 Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param participantId participant id
 @param attributes participant attributes
 */
- (void)client:(TeeVidClient *)client didUpdateParticipant:(NSString *)participantId withAttributes:(NSDictionary *)attributes;

/*!
 @brief Notifies application that participant left room this client is connected to.
 @discussion Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param participantId participant id
 */
- (void)client:(TeeVidClient *)client didRemoveParticipant:(NSString *)participantId;

/*!
 @brief Notifies application that size of video sent by specified participant has changed.
 @discussion When video layout if managed by application, application must use this event to deal with possible changes in aspect ratio.
 
 Note that this selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param videoSize new video size
 @param participantId participant id
 @param view view where participant video is being rendered
 */
- (void)client:(TeeVidClient *)client didChangeVideoSize:(CGSize)videoSize forParticipant:(NSString *)participantId inView:(UIView *)view;

/*!
 @brief Notifies application that participant video view was removed.
 @discussion Typically view is removed when participant closes its audio and video streams, but stays connected to the conference room (remains an observer). When video layout if managed by application, application must use this event to remove view from video layout.
 
 Note that this selector is optional and is being fired only when video layout is managed by application (and not by TeeVidClient itself).
 
 @param client instance of the client this call came from
 @param view view where participant video was being rendered
 @param participantId participant id
 */
- (void)client:(TeeVidClient *)client didRemoveVideoView:(UIView *)view forParticipant:(NSString *)participantId;

/*!
 @brief Notifies application that client was muted by moderator or lecturer.
 @discussion Upon receiving this call, application can change the microphone icon to enabled or disabled state.
 
 Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param muteAttributes variable that represents the muted state
 */
- (void)client:(TeeVidClient *)client didUpdateMuteAttributes:(NSDictionary *)muteAttributes;

/*!
 @brief The user mute mode was changed due to the use of raise hand feature.
 @discussion Upon receiving this call, application can handle a case with (un) muting current participant. Both current user and moderator can discard raised hand.
 
 Note that this selector is optional.
 
 @param client instance of the client this call came from
 @param allowed whether raise hand request was approved by moderator and client was allowed to unmute microphone and/or start sending video
 */
- (void)client:(TeeVidClient *)client didUpdateRaiseHandStatus:(BOOL)allowed;

@end


/*!
 @brief TeeVid video conferencing client.
 @discussion This client provides ability to connect to conference rooms hosted on TeeVid servers.
 
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
 @code
 - (void)viewDidLoad {
     [super viewDidLoad];
     // Do any additional setup after loading the view, typically from a nib
 
     // Initialize TeeVidClient in a mode when video layout is managed by client
     // Pass whatever view you want video to be rendered in
     CGRect bounds = self.view.frame;
     CGRect frame = CGRectMake(bounds.origin.x, bounds.origin.y + 20, bounds.size.width, bounds.size.height - 64);
     conferenceView = [[UIView alloc] initWithFrame:frame];
     [self.view addSubview:conferenceView];
     teeVidClient = [[TeeVidClient alloc] initWithView:conferenceView options:nil andDelegate:self];
 
     // Initiate conference room connection
     [teeVidClient connectTo:_roomId at:_serverAddress as:_participantName withPin:nil];
 }
 @endcode
 */
@interface TeeVidClient : NSObject

/*!
 @brief Contains address (typically DNS name) of the server hosting target conference room.
 */
@property (nonatomic, readonly) NSString *serverAddress;

/*!
 @brief Contains version of MCU that is deployed on the server
 */
@property (nonatomic, readonly) NSString *serverMCUVersion;

/*!
 @brief Contains version of the server hosting target conference room.
 */
@property (nonatomic, readonly) NSString *serverMiddlewareVersion;

/*!
 @brief Contains version of the moderation server for target conference room.
 */
@property (nonatomic, readonly) NSString *serverModerationVersion;

/*!
 @brief Contains id of the conference room to connect to.
 */
@property (nonatomic, readonly) NSString *roomId;

/*!
 @brief Contains participant name.
 */
@property (nonatomic, readonly) NSString *userName;

/*!
 @brief Contains connection options.
 */
@property (nonatomic, readonly) NSDictionary *options;

/*!
 @brief Contains number of remote video streams client is receiving.
 @discussion Reported number of streams does not necesserily match number of participants - some participants might not send video, some might send two video streams (video and screen sharing). Note also that reported number of streams does not include local video stream.
 */
@property (nonatomic, readonly) int numberOfVideoStreams;

/*!
 @brief Contains index of video stream currently shown in a single stream layout.
 @discussion If current layout is automatic, the value of this index would be -1.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be -1.
 */
@property (nonatomic, readonly) int singleStreamLayoutIndex;

/*!
 @brief Reports whether video view is currently in automatic layout.
 @discussion When layout is in automatic mode, client will render up to 6 participant videos in a view provided by application.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be FALSE.
 */
@property (nonatomic, readonly) BOOL automaticLayout;

/*!
 @brief Reports whether client is currently connected to a conference room.
 */
@property (nonatomic, readonly) BOOL connected;

/*!
 @brief Reports whether client's audio is currently muted.
 */
@property (nonatomic, readonly) BOOL muted;

/*!
 @brief Reports whether client's video is currently stopped (client not sending video).
 */
@property (nonatomic, readonly) BOOL videoStopped;

/*!
 @brief Reports whether client is currently in lecture mode.
 @discussion While in lecture mode, client does not send audio and video, and, videw view contains single video of lecturer's presentation or video stream.
 */
@property (nonatomic, readonly) BOOL lectureMode;

/*!
 @brief Reports whether client is currently in lecture mode, but, lecturer did not join the room yet.
 @discussion While is such state, client does not send audio and video, and, video view does not render any video stream.
 */
@property (nonatomic, readonly) BOOL waitingForLecturer;

/*!
 @brief Contains reference to a view this client was initialized with.
 @discussion Client will render one or multiple participant videos in this view.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be nil.
 */
@property (nonatomic, strong, readonly) UIView *view;

/*!
 @brief Contains reference to a local video view,.
 @discussion When video layout is managed by client, this view is a subview of a parent view this client was initialized with. Application can use this property to register gesture recognizer for local video view.
 
 Note that local video view is only available when client is connected to remote room. Also, note that local video view is hidden when client's video is stopped.
 
 Note also that this property is still available if video layout is managed by application itself, but, in this mode client does not control local video view position, size and aspect ratio.
 */
@property (nonatomic, strong, readonly) UIView *localView;

/*!
 @brief Contains position of a local video view in a parent view.
 @discussion Note local view position is not applicable while there is only local client connected - in this case local video view fills parent view frame completely.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be TeeVidClientLocalViewPositionUnknown.
 */
@property (nonatomic, assign) TeeVidClientLocalViewPosition localViewPosition;

/*!
 @brief Reports whether local video is currently hidden.
 @discussion Application can hide local video using corresponding selector. While hidden, it will be replaced by semi-transparent image which allows to process gesture events, e.g. show local video after tap.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value of this property will be always FALSE.
 */
@property (nonatomic, readonly) BOOL localVideoHidden;

/*!
 @brief Contains placeholder image for local video to be shown when local video is hidden.
 @discussion Application can hide local video using corresponding selector. While hidden, it will be replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap. This property contains custom image to be used as local video placeholer.
 @warning This property is only applicable if video layout is managed by TeeVidClient. If video layout is managed by application, value set to this property is ingored.
 */
@property (nonatomic, strong) UIImage *localVideoPlaceholder;

/*!
 @brief Reports whether video layoyt is managed by TeeVidClient.
 @discussion Application can instantiate TeeVidClient in two different ways: provide client with a view to render participants video in and let client manage video layout, or, have application reposible for all aspects of video layoyut management. This property reports which mode TeeVidClient was instantiated with.
 */
@property (nonatomic, readonly) BOOL managingVideoLayout;


/*!
 @brief Creates new instance of TeeVidClient.
 @discussion View and delegate parameters are required. TeeVidClient created by this method will manage participants video layout inside of a view provided by application. Application will be able to control only certain elements of video layout: switch between mutliple ans single video layouts, iterate video streams in single video layout, control position of local video.
 If application needs more fine grained control of video layout, it should instantiate TeeVidClient using overloaded init method without passing view to render video layout in. In such mode, application can obtain views where participant video is being rendered for individual participants, and, then size and position them on the screen on its own.
 
 Note that options parameter is reserved for future use.
 
 @param view view to render conference room participants videos in
 @param options conference room options (reserved for future use)
 @param delegate TeeVidClient delegate
 @return new istance of TeeVidClient
 */
- (instancetype)initWithView:(UIView *)view options:(NSDictionary *)options andDelegate:(id<TeeVidClientDelegate>)delegate;

/*!
 @brief Creates new instance of TeeVidClient.
 @discussion Delegate parameter is required. TeeVidClient created by this method will leave application responsible for managing participants video layout. Application should obtain views where participant video is being rendered, and, then size and position them on the screen.
 If application does not need fine grained control of video layout, it should instantiate TeeVidClient using overloaded init method and pass a view to be used by TeeVidClient to render participants video in. In such mode, TeeVidClient will manage position, size and aspect ration of each participant video in the layout. Application will be able to control only certain elements of video layout: switch between mutliple ans single video layouts, iterate video streams in single video layout, control position of local video.
 
 Note that options parameter is reserved for future use.
 
 @param options conference room options (reserved for future use)
 @param delegate TeeVidClient delegate
 @return new istance of TeeVidClient
 */
- (instancetype)initWithOptions:(NSDictionary *)options andDelegate:(id<TeeVidClientDelegate>)delegate;

/*!
 @brief Connects to a conference room.
 @discussion This method is typically used when user initiates connection. Connection is performed asynchronously after all parameters are validated. Any change in connection state (including errors) will be reported to application using appropiate delegate methods.
 
 Note that if user does not know whether room is PIN protected, connection can be initiated without PIN. In such case, if client detects that room requires PIN to connect, it will request PIN by calling approrpiate delegate method.

 @param roomId conference room id
 @param serverAddress address server address (typically DNS name)
 @param userName userName participant name
 @param pin optional conference room owner or guest PIN
 */
- (void)connectTo:(NSString *)roomId at:(NSString *)serverAddress as:(NSString *)userName withPin:(NSString *)pin;

/*!
 @brief Connects to a conference room.
 @discussion This method is typically used when application receives invitation link or other type of invite containing invitation token. Passing valid invitation token allows application to connect to PIN protected room without knowing its PIN. Connection is performed asynchronously after all parameters are validated. Any change in connection state (including errors) will be reported to application using appropiate delegate methods.
 @warning Each invitation token can be only used one time.

 @param roomId conference room id
 @param serverAddress address server address (typically DNS name)
 @param userName userName participant name
 @param invitationToken invitation token
 */
- (void)connectTo:(NSString *)roomId at:(NSString *)serverAddress as:(NSString *)userName withInvitationToken:(NSString *)invitationToken;

/*!
 @brief Creates invitation token.
 @discussion This method can be used to generate invitation token in order to invite someone to a conference room. Application then is responsible to deliver such invitation using out of band channel (instant message, email, etc.)
 
 Note that in order to generate invitation token, client must be successfully connected to a conference room first.

 @return invitation token
 */
- (NSString *)createInvitationToken;

/*!
 @brief Disconnects from the conference room.
 @discussion Application should use this method to disconnect client from the conference room.
 @warning Application must make all efforts to disconnect client gracefully, even when terminated abruply.
 */
- (void)disconnect;

/*!
 @brief Mutes client's audio.
 @discussion Application can use this method to mute local audio.
 */
- (void)mute;

/*!
 @brief Unmutes client's audio.
 @discussion Application can use this method to unmute local audio.
 @warning When client is in lecture mode, this functionality is not avalilable.
 */
- (void)unmute;

/*!
 @brief Stops sending video.
 @discussion Application can use this method to stop sending video to the conference room.
 */
- (void)stopVideo;

/*!
 @brief Resumes sending video.
 @discussion Application can use this method to resume sending video to the conference room.
 @warning When client is in lecture mode, this functionality is not avalilable.
 */
- (void)resumeVideo;

/*!
 @brief Switches front and back cameras.
 @discussion Application can use this method to switch video from front to back camera (or from bacl to front camera). Initially from camera is being used. If device has only one camera (or no cameras), no action is performed.
 */
- (void)switchCamera;

/*!
 @brief Allows a user to request unmuting during a lecture mode.
 @discussion Sends action named "raise-hand" and the participant's id.
 */
- (void)raiseHand:(BOOL)raise;

/*!
 @brief Switches video view to show single video stream layout of given index.
 @discussion When video layout is managed by client, application can use this method to switch video layout from automatic to single stream. If participant with provided stream index leaves the conference, client will automatically switch to display video of the stream with next available index. If there is no stream with target index, first video stream from participant list will be displayed.
 @warning This functionality is not avalilable when video layout is managed by application itself, or when client is in lecture mode.
 
 @param streamIndex streamIndex index of video stream to show
 */
- (void)switchToSingleStreamLayout:(int)streamIndex;

/*!
 @brief Switches video view from single video to automatic layout.
 @discussion When video layout is managed by client and it is in automatic layout mode, client will render video from multiple participants and change layout automatically based on number of participants.
 @warning This functionality is not avalilable when video layout is managed by application itself, or when client is in lecture mode.
 */
- (void)switchToAutomaticLayout;

/*!
 @brief Refreshes layout of videos rendered in view view.
 @discussion When video layout is managed by client, application will be notified by client's delegate didRequestLayoutRefresh method every time when video layout changes. Upon receving such notification, application must call refreshLayout method. It can wrap this call into any desired animation or use any other effects to make layout transition smooth.
 @warning Application must always call refreshLayout method when notified about layout change. Failure to call this method will result client rendering obsolete layout in video view.
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)refreshLayout;

/*!
 @brief Hides local video.
 @discussion When video layout is managed by client, application can use this method to hide local video. While hidden, it will be replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap.
 @warning Before hiding local video, application must set property containing placeholder image to be shown in place of local video.
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)hideLocalVideo;

/*!
 @brief Shows local video view.
 @discussion When video layout is managed by client, application can use this method to show local video after it was hidden. Note that while hidden, local video is replaced by a placeholder image which allows to process gesture events, e.g. show local video after tap.
 @warning This functionality is not avalilable when video layout is managed by application itself.
 */
- (void)showLocalVideo;

/*!
 @brief Retrieves view where video of specified participant is rendered.
 @discussion When video layout is managed by client, this view is a a subview of a parent view this client was initialized with. Otherwise, new view is created from video stream, and, then, aplication has full control on its position and size.
 
 Note that one participant can send two vdieo streams simultaneously - actual video stream, and screen sharing (aka presentation) stream. To retrieve view where screen sharing stream is being rendered, video parameter should be set to FALSE.
 @warning When video layoout is managed by client, application must not attempt changing position or size of individual participant video views.
 
 @param participantId participantId id of participant to retrieve video of
 @param video whether to retrieve video view (vs screen sharing)
 @return reference to a view where video of individual participant is being rendered in, or nil if specified participant does not send video stream
 */
- (UIView *)getViewForParticipant:(NSString *)participantId video:(BOOL)video;

/*!
 @brief Retrieves ids of participants selected by lecturer to be included in video layout during lecture mode.
 @discussion While in lecture mode, lecturer can designate some participants to be included in video layout.
 
 Non-empty list indicates that lecturer selected specific video layout with given number of participants in it. Some slots might be empty, it is indicated by special entry in the list - "stub". This list might or might not include lecturer itself.
 Empty list indicates "automatic" layout during lecture mode. In such case, lecturer is always included. Other participants might be also included if lecturer accepted their "raise hand" request.
 
 @return list of participant ids to included in video layout during lecture mode
 */
- (NSArray *)getLecturerSelectedParticipants;

@end
