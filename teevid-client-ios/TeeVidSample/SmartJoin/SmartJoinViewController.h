//
//  SmartJoinViewController.h
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import <UIKit/UIKit.h>
@class SmartJoinViewController;

@protocol SmartJoinViewControllerDelegate <NSObject>

- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen deviceRotationRefreshRequested:(id)sender;
- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen userImageRefreshRequested:(id)sender;
- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen closeButtonTapped:(id)sender;

- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen joinButtonTapped:(id)sender;

- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen switchCamButtonTapped:(id)sender;
- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen camButtonTapped:(id)sender;
- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen micButtonTapped:(id)sender;

- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen closeAndDisconnect:(id)sender;
- (void)smartJoinScreen:(SmartJoinViewController *)smartJoinScreen connectWithPin:(NSString *)pin;

@end




@interface SmartJoinViewController : UIViewController

@property (weak, nonatomic) id<SmartJoinViewControllerDelegate> delegate;

- (void)setRoomName:(NSString *)roomName;

- (void)muteCamera;
- (void)unmuteCamera;

- (void)muteMicrophone;
- (void)unmuteMicrophone;

- (void)updateMicLevelIndicator:(float)micLevelValue;

- (void)showMessageAndDisconnect:(NSString *)message;

- (void)showAccessPinPrompt;
- (void)showWaitingRoomPrompt;
- (void)dismissWaitingRoomPrompt;

- (void)showMuteTextRecommendation;

- (void)showJoinButtonAnimation;
- (void)stopJoinButtonAnimation;

@end


