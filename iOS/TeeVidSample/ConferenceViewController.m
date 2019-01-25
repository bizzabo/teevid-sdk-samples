//
//  ConferenceViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2019 cloudAYI. All rights reserved.
//

#import "ConferenceViewController.h"

@interface ConferenceViewController ()

@end

@implementation ConferenceViewController {
@private
    UIView *conferenceView;
    TeeVidClient *teeVidClient;
    NSMutableDictionary *videoViews;
    BOOL clientManagedLayout;
    BOOL disconnecing;
}

@synthesize serverAddress, roomId;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib
    
    // Initialize TeeVidClient
    // Two modes are available: video layout is managed by client, or by application
    // To use mode when video layout is managed by application, set clientManagedLayout to NO
    clientManagedLayout = NO;
    if (clientManagedLayout) {
        // Pass whatever view you want video to be rendered in
        CGRect bounds = self.view.frame;
        CGRect frame = CGRectMake(bounds.origin.x, bounds.origin.y + 20, bounds.size.width, bounds.size.height - 64);
        conferenceView = [[UIView alloc] initWithFrame:frame];
        conferenceView.autoresizingMask = self.view.autoresizingMask;
        [self.view addSubview:conferenceView];
        teeVidClient = [[TeeVidClient alloc] initWithView:conferenceView options:nil andDelegate:self];
    } else {
        // Create client without passing view, instead get and manage individual participant video views directly
        teeVidClient = [[TeeVidClient alloc] initWithOptions:nil andDelegate:self];
        videoViews = [[NSMutableDictionary alloc] init];
    }
    
    // Initiate conference room connection
    [teeVidClient connectTo:roomId at:serverAddress as:@"TeeVidSample iOS" withPin:nil];
    
    // Prevent screen from dimming while in a call
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"DisconnectSegue"]) {
        // Disconnect button was tapped
        disconnecing = YES;
        [teeVidClient disconnect];
        
        // Restore application idle timer
        [[UIApplication sharedApplication] setIdleTimerDisabled: NO];
    }
}

# pragma mark - TeeVidClientDelegate

- (void)client:(TeeVidClient *)client didRequestAccessPIN:(NSString *)roomId {
    // Prompt for access PIN
}

- (void)client:(TeeVidClient *)client didEnterWaitingRoom:(NSString *)roomId {
    // Prompt for owner PIN to unlock waiting room, or wait until owner joins
}

- (void)client:(TeeVidClient *)client didLeaveWaitingRoom:(NSString *)roomId {
    // Dismiss prompt for owner PIN if still shown
}

- (void)client:(TeeVidClient *)client didConnect:(NSString *)roomId {
    // Client has connected to the conference room
    // Perform any appropriate actions - enable controls, etc.
}

- (void)client:(TeeVidClient *)client didDisconnect:(NSString *)roomId {
    // Client has disconnected from the conference room
    // Note that this can be result of disconnect segue, or disconnect on remote end
    // If disconnected remotely, need to perform disconnect seque to return to entry screen
    if (!disconnecing) {
        [self performSegueWithIdentifier:@"DisconnectSegue" sender:self];
    }
}

- (void)client:(TeeVidClient *)client didEnterLectureMode:(NSString *)roomId {
    // Any action specific to lecture mode, e.g. disable microphone
    // Note that client will mute local audio and change layout to lecturer video automatically
}

- (void)client:(TeeVidClient *)client didLeaveLectureMode:(NSString *)roomId {
    // Restore to non-lecture mode
    // Note that client will unmute local audio and change layout to automatic
}

- (void)client:(TeeVidClient *)client didEnterWaitingForLecturer:(NSString *)roomId {
    // Wait for lecturer
    // Note that no video will be shown until lecturer arrives
}

- (void)client:(TeeVidClient *)client didReceiveLecturerDisconnectEvent:(NSString *)roomId {
    // Lecturer has disconnected. Perform whatever action is appropriate or ask user what to do
}

- (void)client:(TeeVidClient *)client didReceiveError:(NSString *)error {
    // Display error and perform disconnect segue
    UIAlertController *alert = [UIAlertController
                                alertControllerWithTitle:@"Error"
                                message:error
                                preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *action = [UIAlertAction
                             actionWithTitle:@"OK"
                             style:UIAlertActionStyleDefault
                             handler:^(UIAlertAction *action) {
                                 [self performSegueWithIdentifier:@"DisconnectSegue" sender:self];
                             }];
    [alert addAction:action];
    
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)client:(TeeVidClient *)client didRequestLayoutRefresh:(NSString *)reason {
    // Conference view layout changed - instruct client to show new layout
    // Note that client will request layout refresh only if video layout is managed by client
    // Add any animatiom you want while refreshind layout
    [UIView transitionWithView:conferenceView duration:0.3 options:(UIViewAnimationOptionLayoutSubviews) animations:^{
        [client refreshLayout];
    } completion:nil];
}

- (void)client:(TeeVidClient *)client didAddParticipant:(NSString *)participantId withAttributes:(NSDictionary *)attributes {
    // New participant has been added
    // Application can use this call to update participant list
    // If video layout is managed by application, application is also responsible to get and manage view for each participant
    if (!clientManagedLayout) {
        id videoAttribute = attributes[TEEVID_ATTRIBUTE_VIDEO];
        if (videoAttribute && [videoAttribute boolValue]) {
            UIView *videoView = [client getViewForParticipant:participantId video:YES];
            // Participant might not have video stream yet
            if (videoView) {
                videoViews[participantId] = videoView;
                [self.view addSubview:videoView];
                [UIView transitionWithView:conferenceView duration:0.3 options:(UIViewAnimationOptionLayoutSubviews) animations:^{
                    [self refreshLayout];
                } completion:nil];
            }
        }
    }
}

- (void)client:(TeeVidClient *)client didUpdateParticipant:(NSString *)participantId withAttributes:(NSDictionary *)attributes {
    // Participant has been updated
    // Application can use this call to update participant list
    // If video layout is managed by application, application is also responsible to manage view for each participant
    if (!clientManagedLayout) {
        id videoAttribute = attributes[TEEVID_ATTRIBUTE_VIDEO];
        if (videoAttribute && [videoAttribute boolValue]) {
            // Is there already video view for this participant?
            UIView *videoView = videoViews[participantId];
            if (!videoView) {
                // No cached video view, check if participant added video stream
                videoView = [client getViewForParticipant:participantId video:YES];
                if (videoView) {
                    videoViews[participantId] = videoView;
                    [self.view addSubview:videoView];
                    [UIView transitionWithView:conferenceView duration:0.3 options:(UIViewAnimationOptionLayoutSubviews) animations:^{
                        [self refreshLayout];
                    } completion:nil];
                }
            }
        }
    }
}

- (void)client:(TeeVidClient *)client didRemoveParticipant:(NSString *)participantId {
    // Participant has been removed
    // Application can use this call to update participant list
    // If video layout is managed by application, application is also responsible to manage view for each participant
    if (!clientManagedLayout) {
        UIView *videoView = videoViews[participantId];
        if (videoView) {
            [videoViews removeObjectForKey:participantId];
            [videoView removeFromSuperview];
            [UIView transitionWithView:conferenceView duration:0.3 options:(UIViewAnimationOptionLayoutSubviews) animations:^{
                [self refreshLayout];
            } completion:nil];
        }
    }
}

- (void)client:(TeeVidClient *)client didChangeVideoSize:(CGSize)videoSize forParticipant:(NSString *)participantId inView:(UIView *)view {
    // Note that client will notify about video size change only if video layout is managed by application
    // Application must use this event to set correct aspect ratio of rendered video by adjusting view size, and,
    // hiding/masking part of the view or using any other appropriate technique
}

- (void)client:(TeeVidClient *)client didRemoveVideoView:(UIView *)view forParticipant:(NSString *)participantId {
    // Note that client will notify about video size change only if video layout is managed by application
    // Application should use this call to remove view from video layout
    UIView *videoView = videoViews[participantId];
    if (videoView) {
        [videoViews removeObjectForKey:participantId];
        [videoView removeFromSuperview];
        [UIView transitionWithView:conferenceView duration:0.3 options:(UIViewAnimationOptionLayoutSubviews) animations:^{
            [self refreshLayout];
        } completion:nil];
    }
}

- (void)client:(TeeVidClient *)client didRecieveUnmuteRequest:(NSDictionary*)request completionHandler:(void (^)(BOOL allowUnmute))completionHandler {
    // completionHandler should called when a user make a choice about approving or discard the request. allowUnmute is a BOOL value that represent a user's answer
    completionHandler(YES);
}

// only used when video layout is managed by application
// otherwise, [client refreshLayout] must be used
- (void)refreshLayout {
    // for demo purposes, just split screen in 4 quadrants and put up to 4 videos there
    // note that code below does not deal with aspect ratio! It must be dealt with in didChangeVideoSize event
    CGRect windowRect = self.view.window.frame;
    CGFloat singleViewWidth = windowRect.size.width / 2;
    CGFloat singleViewHeight = (windowRect.size.height - 80) / 2;
    
    NSArray *keys = [videoViews allKeys];
    unsigned long count = keys.count;
    unsigned long start = count > 4 ? count - 4 : 0;
    for (unsigned long i = 0; i < count; ++i) {
        UIView *videoView = videoViews[keys[i]];
        if (i < start) {
            videoView.hidden = YES;
        } else {
            videoView.bounds = CGRectMake(0, 0, singleViewWidth, singleViewHeight);
            switch(i - start) {
                case 0:
                    videoView.frame = CGRectMake(0, 40, singleViewWidth, singleViewHeight);
                    break;
                case 1:
                    videoView.frame = CGRectMake(singleViewWidth, 40, singleViewWidth, singleViewHeight);
                    break;
                case 2:
                    videoView.frame = CGRectMake(0, singleViewHeight + 40, singleViewWidth, singleViewHeight);
                    break;
                case 3:
                    videoView.frame = CGRectMake(singleViewWidth, singleViewHeight + 40, singleViewWidth, singleViewHeight);
                    break;
            }
            videoView.hidden = NO;
            [self.view bringSubviewToFront:videoView];
        }
    }
}

@end
