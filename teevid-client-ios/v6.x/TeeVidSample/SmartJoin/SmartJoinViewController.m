//
//  SmartJoinViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "SmartJoinViewController.h"
#import <QuartzCore/QuartzCore.h>
#import "Utils.h"
#import "JoinButton.h"
#import "AccessPinViewController.h"

int const POPUP_Y_ADJUSTMENT_DEFAULT_VALUE  = 50;
int const POPUP_Y_ADJUSTMENT_IPHONE5_VALUE  = 100;

typedef NS_ENUM(NSInteger) {
    SmartJoinScreenModeMain,
    SmartJoinScreenModeAccessPin,
    SmartJoinScreenModeWaitingRoom
} SmartJoinScreenMode;



@interface SmartJoinViewController () <UIGestureRecognizerDelegate, AccessPinDelegate>

@property (assign, nonatomic) SmartJoinScreenMode   screenMode;

@property (weak, nonatomic) IBOutlet UIView     *bgView;

@property (weak, nonatomic) IBOutlet UILabel    *roomNameLabel;

@property (weak, nonatomic) IBOutlet UITextField    *userNameTextField;

@property (weak, nonatomic) IBOutlet UILabel        *areYouReadyTitle;
@property (weak, nonatomic) IBOutlet UILabel        *areYouReadyDescription;

@property (weak, nonatomic) IBOutlet JoinButton     *joinButton;

@property (weak, nonatomic) IBOutlet UIButton       *switchCamButton;
@property (weak, nonatomic) IBOutlet UIButton       *camButton;
@property (weak, nonatomic) IBOutlet UIButton       *micButton;

@property (weak, nonatomic) IBOutlet UILabel        *micLevelLabel;
@property (weak, nonatomic) IBOutlet UIProgressView *micLevelIndicator;


@property (weak, nonatomic) IBOutlet UIView *accessPinContainer;
@property (strong, nonatomic) AccessPinViewController *accessPinViewController;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *cAccessPinViewBottom;

@property (weak, nonatomic) IBOutlet UIView *muteRecomendContainer;
@property (weak, nonatomic) IBOutlet UIView *waitingRoomContainer;

/*!
 we need such ratio to fix the "access pin" view misplace during keyboard show and hide (especially that text field is out of the screen in landscape mode)
 the root cause of this is that on some devices keyboard would appear via 2 animations (two willShow and didShow methods calls)
 and on some devices keyboard appears via just one animation (one willShow and didShow methods call)
 but keyboard always dissapears via one call (just one willHide a didHide regardless of deivce)
 such issue causes a collistion in "access pin" view missplace after keyboard didHide
*/
@property (assign, nonatomic) int   keyboardAnimationsRatio;
@property (assign, nonatomic) int   popupVertAdjustmentValue;
@property (assign, nonatomic) BOOL  isIphone;

@end



@implementation SmartJoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // prepare gestures
    [self prepareKeyboardGesture];
    
    // prepare notifications
    [self prepareNotifications];
    
    [self prepareUserNameTextField];
    
    [self prepareSmartJoinAppearance];
    
    // just join button, no popups
    [self activateMainMode];
    
    [self prepareAccessPinVerticalAdjustmentProperties];
    
    self.userNameTextField.userInteractionEnabled = NO;
}


- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)setRoomName:(NSString *)roomName {
    [_roomNameLabel setText:roomName];
}

#pragma mark - SmartJoin screen UI
- (IBAction)closeIconPressed:(id)sender {
    [self closeButtonPressed];
}


- (void)closeButtonPressed {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:closeButtonTapped:)])
        [self.delegate smartJoinScreen:self closeButtonTapped:nil];
}


- (IBAction)joinButtonPressed:(id)sender {
    if (self.userNameTextField.text.length == 0) {
        [Utils showMessage:@"User name can't be empty" withTitle:@"Error"];
        return;
    }
    
    [self showJoinButtonAnimation];
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:joinButtonTapped:)])
        [self.delegate smartJoinScreen:self joinButtonTapped:nil];
}


- (IBAction)switchCamButtonPressed:(id)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:switchCamButtonTapped:)])
        [self.delegate smartJoinScreen:self switchCamButtonTapped:nil];
}

- (IBAction)camButtonPressed:(id)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:camButtonTapped:)])
        [self.delegate smartJoinScreen:self camButtonTapped:nil];
}


- (IBAction)micButtonPressed:(id)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:micButtonTapped:)])
        [self.delegate smartJoinScreen:self micButtonTapped:nil];
}


- (void)refreshUserImage {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:userImageRefreshRequested:)])
        [self.delegate smartJoinScreen:self userImageRefreshRequested:nil];
}


- (void)deviceDidRotated {
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:deviceRotationRefreshRequested:)])
        [self.delegate smartJoinScreen:self deviceRotationRefreshRequested:nil];
}



#pragma mark - SmartJoin public API
- (void)muteCamera {
    [self.camButton setImage:[UIImage imageNamed:@"sm-CamMuted"] forState:UIControlStateNormal];
    [self.switchCamButton setEnabled:NO];
}

- (void)unmuteCamera {
    [self.camButton setImage:[UIImage imageNamed:@"sm-Cam"] forState:UIControlStateNormal];
    [self.switchCamButton setEnabled:YES];
}


- (void)muteMicrophone {
    [self.micButton setImage:[UIImage imageNamed:@"sm-MicMuted"] forState:UIControlStateNormal];
}

- (void)unmuteMicrophone {
    [self.micButton setImage:[UIImage imageNamed:@"sm-Mic"] forState:UIControlStateNormal];
}


- (void)updateMicLevelIndicator:(float)micLevelValue {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.micLevelIndicator setProgress:micLevelValue];
    });
}


- (void)showMessageAndDisconnect:(NSString *)message {
    [Utils showMessage:message withTitle:nil handler:^(UIAlertAction *action) {
        if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:closeAndDisconnect:)])
            [self.delegate smartJoinScreen:self closeAndDisconnect:nil];
    }];
}


- (void)showAccessPinPrompt {
    if (self.screenMode == SmartJoinScreenModeAccessPin) return;
    [self activateAccessPinMode];
}


- (void)showWaitingRoomPrompt {
    if (self.screenMode == SmartJoinScreenModeWaitingRoom) return;
    [self activateWaitingRoomMode];
}


- (void)dismissWaitingRoomPrompt {
    [self deactivateWaitingRoomMode];
}



#pragma mark - Keyboard handlers
- (void)keyboardWillChange:(NSNotification *)notification {
    double duration     = [[notification.userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    CGRect curFrame     = [[notification.userInfo objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue];
    CGRect targetFrame  = [[notification.userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    float deltaY        = curFrame.origin.y - targetFrame.origin.y;
    float deltaYAdjusted;
    
    if (self.isIphone) {
        // on iPhone we use ratio value > meaning we adjust given delta value to shift "access pin" view vertically more correctly
        [self adjustKeyboardAnimationsRatioAgainstDelta:deltaY];
        deltaYAdjusted = (deltaY > 0) ? (deltaY - self.popupVertAdjustmentValue) : (deltaY + (self.popupVertAdjustmentValue * self.keyboardAnimationsRatio));
    }
    else {
        // on iPad we use no ratio > just using the given delta value to shift "access pin" view vertically
        deltaYAdjusted = deltaY;
    }
    
    self.cAccessPinViewBottom.constant += deltaYAdjusted;
    [self.accessPinContainer setNeedsUpdateConstraints];
    
    [UIView animateWithDuration:duration delay:0 options:(UIViewAnimationOptionCurveEaseOut) animations:^{
        CGRect newFrame     = self.accessPinContainer.frame;
        newFrame.origin.y   -= deltaYAdjusted;
        self.accessPinContainer.frame = newFrame;
        [self.accessPinContainer layoutIfNeeded];
    } completion:nil];
}


- (void)adjustKeyboardAnimationsRatioAgainstDelta:(float)delta {
    /*!
    we need such ratio to fix the "access pin" view misplace during keyboard show and hide (especially that text field is out of the screen in landscape mode)
    the root cause of this is that on some devices keyboard would appear via 2 animations (two willShow and didShow methods calls)
    and on some devices keyboard appears via just one animation (one willShow and didShow methods call)
    but keyboard always dissapears via one call (just one willHide a didHide regardless of deivce)
    such issue causes a collistion in "access pin" view missplace after keyboard didHide
    */
    
    if (delta > 0) { // will show keyboard
        if (self.keyboardAnimationsRatio < 0)
            self.keyboardAnimationsRatio = 1;
        else
            self.keyboardAnimationsRatio++;
    }
}

- (void)keyboardDidHide:(NSNotification *)notification {
    if (self.keyboardAnimationsRatio > 0) self.keyboardAnimationsRatio = -1;
}


- (void)dismissKeyboard {
    if ([self.userNameTextField isFirstResponder]) [self.userNameTextField resignFirstResponder];
    [self.accessPinViewController dismissKeyboard];
}


#pragma mark - SmartJoin appearance

- (void)prepareUserNameTextField {
    [self.userNameTextField addTarget:self action:@selector(dismissKeyboard) forControlEvents:UIControlEventEditingDidEndOnExit];
    self.userNameTextField.layer.borderColor = [UIColor grayColor].CGColor;
    self.userNameTextField.layer.borderWidth = 1.0f;
    self.userNameTextField.text = @"TeeVidSample iOS";
}

- (void)prepareSmartJoinAppearance {
    self.micLevelLabel.text             = NSLocalizedString(@"Microphone level", @"microphone level");
    self.areYouReadyTitle.text          = NSLocalizedString(@"Are you ready to start?", @"are you ready to start");
    self.areYouReadyDescription.text    = NSLocalizedString(@"Choose your video and audio settings, change name", @"choose your video and audio settings, change name");
}


- (void)showJoinButton {
    [self.joinButton setHidden:NO];
}

- (void)hideJoinButton {
    [self.joinButton setHidden:YES];
}


- (void)showAreYouReadyText {
    [self.areYouReadyTitle          setHidden:NO];
    [self.areYouReadyDescription    setHidden:NO];
}

- (void)hideAreYouReadyText {
    [self.areYouReadyTitle          setHidden:YES];
    [self.areYouReadyDescription    setHidden:YES];
}



#pragma mark - Access PIN appearance
- (void)showAccessPinView {
    [self.accessPinViewController show];
    [self.accessPinContainer setHidden:NO];
}

- (void)hideAccessPinView {
    [self.accessPinViewController hide];
    [self.accessPinContainer setHidden:YES];
}


- (void)accessPin:(AccessPinViewController *)viewController connectWithPin:(NSString *)pin {
    [self showJoinButtonAnimation];
    if (self.delegate && [self.delegate respondsToSelector:@selector(smartJoinScreen:connectWithPin:)]) {
        [self.delegate smartJoinScreen:self connectWithPin:pin];
        [self deactivateAccessPinMode];
    }
}


- (void)showWaitingRoomView {
    [self.waitingRoomContainer setHidden:NO];
}

- (void)hideWaitingRoomView {
    [self.waitingRoomContainer setHidden:YES];
}


- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"accessPinSegue"]) {
        self.accessPinViewController = segue.destinationViewController;
        self.accessPinViewController.delegate = self;
    }
}



#pragma mark - HardMute appearance
- (void)showMuteTextRecommendation {
    [self showHardMuteView];
}

- (void)showHardMuteView {
    [self.muteRecomendContainer setHidden:NO];
}

- (void)hideHardMuteView {
    [self.muteRecomendContainer setHidden:YES];
}



#pragma mark - SmartJoin Mode
- (void)activateMainMode {
    self.screenMode = SmartJoinScreenModeMain;
    [self stopJoinButtonAnimation];
    [self showJoinButton];
    [self hideAccessPinView];
    [self hideWaitingRoomView];
}


- (void)activateAccessPinMode {
    self.screenMode = SmartJoinScreenModeAccessPin;
    
    [UIView animateWithDuration:0.5 delay:0 options:(UIViewAnimationOptionCurveEaseOut) animations:^{
        [self showAccessPinView];
        [self hideAreYouReadyText];
        [self hideJoinButton];
        [self hideWaitingRoomView];
        [self hideHardMuteView];
    } completion:nil];
}

- (void)deactivateAccessPinMode {
    self.screenMode = SmartJoinScreenModeMain;
    [self dismissKeyboard];
    
    [UIView animateWithDuration:0.3 delay:0 options:(UIViewAnimationOptionCurveEaseOut) animations:^{
        [self showAreYouReadyText];
        [self showJoinButton];
        [self hideAccessPinView];
    } completion:nil];
}


- (void)activateWaitingRoomMode {
    self.screenMode = SmartJoinScreenModeWaitingRoom;
    
    [UIView animateWithDuration:0.5 delay:0 options:(UIViewAnimationOptionCurveEaseOut) animations:^{
        [self showWaitingRoomView];
        [self hideAreYouReadyText];
        [self hideJoinButton];
        [self hideAccessPinView];
        [self hideHardMuteView];
    } completion:nil];
}

- (void)deactivateWaitingRoomMode {
    self.screenMode = SmartJoinScreenModeMain;
    
    [UIView animateWithDuration:0.3 delay:0 options:(UIViewAnimationOptionCurveEaseOut) animations:^{
        [self showAreYouReadyText];
        [self showJoinButton];
        [self hideWaitingRoomView];
    } completion:nil];
}



#pragma mark - Gestures & Notifications
- (void)prepareKeyboardGesture {
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
    [self.view addGestureRecognizer:tap];
}


- (void)prepareNotifications {
    NSNotificationCenter *notificationManager = [NSNotificationCenter defaultCenter];
    
    [notificationManager addObserver:self selector:@selector(keyboardWillChange:) name:UIKeyboardWillChangeFrameNotification object:nil];
    [notificationManager addObserver:self selector:@selector(keyboardDidHide:) name:UIKeyboardDidHideNotification object:nil];
}


- (void)prepareAccessPinVerticalAdjustmentProperties {
    /*!
     we need such ratio to fix the "access pin" view misplace during keyboard show and hide (especially that text field is out of the screen in landscape mode)
     the root cause of this is that on some devices keyboard would appear via 2 animations (two willShow and didShow methods calls)
     and on some devices keyboard appears via just one animation (one willShow and didShow methods call)
     but keyboard always dissapears via one call (just one willHide a didHide regardless of deivce)
     such issue causes a collistion in "access pin" view missplace after keyboard didHide
     */
    self.keyboardAnimationsRatio = -1;
    
    self.isIphone = [Utils isIPhone];
    
    if (self.isIphone) {
        self.popupVertAdjustmentValue = [Utils isIPhone5] ? POPUP_Y_ADJUSTMENT_IPHONE5_VALUE : POPUP_Y_ADJUSTMENT_DEFAULT_VALUE;
    }
}



#pragma mark - Join button animation
- (void)showJoinButtonAnimation {
    [self.joinButton showJoinAnimation];
}


- (void)stopJoinButtonAnimation {
    [self.joinButton stopJoinButtonAnimation];
}



#pragma mark - View transition animation
- (void)dismissViewControllerAnimated:(BOOL)flag completion:(void (^)(void))completion {
    UIView *splashView          = [[UIView alloc] initWithFrame:self.view.frame];
    splashView.alpha            = 1.0f;
    splashView.backgroundColor  = [UIColor blackColor];
    splashView.translatesAutoresizingMaskIntoConstraints = NO;
    
    [self.view addSubview:splashView];
    [self.view bringSubviewToFront:splashView];
    
    NSArray *animationConstraints = @[
        [splashView.topAnchor constraintEqualToAnchor:self.view.topAnchor],
        [splashView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
        [splashView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
        [splashView.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor]
    ];
    [NSLayoutConstraint activateConstraints:animationConstraints];
        
    [super dismissViewControllerAnimated:flag completion:completion];
}


@end
