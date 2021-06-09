//
//  AccessPinViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "AccessPinViewController.h"
#import "JoinButton.h"


@interface AccessPinViewController ()

@property (weak, nonatomic) IBOutlet UILabel        *accessPinTitle;
@property (weak, nonatomic) IBOutlet UILabel        *accessPinDescription;
@property (weak, nonatomic) IBOutlet UITextField    *accessPinTextField;
@property (weak, nonatomic) IBOutlet UIButton       *accessPinTogglePasswordButton;
@property (weak, nonatomic) IBOutlet JoinButton     *accessPinJoinButton;

@end



@implementation AccessPinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepareAppearance];
}


- (void)prepareAppearance {
    [self.accessPinTextField addTarget:self action:@selector(pinTextFieldDidChange) forControlEvents:UIControlEventEditingChanged];
    [self.accessPinTextField addTarget:self action:@selector(pinTextFieldEnterPressed) forControlEvents:UIControlEventEditingDidEndOnExit];
    self.accessPinTitle.text        = NSLocalizedString(@"Access PIN", @"Access pin title");
    self.accessPinDescription.text  = NSLocalizedString(@"The room is password protected, enter to join", @"the room is password protected, enter to join");
    self.accessPinTextField.placeholder = NSLocalizedString(@"Enter PIN", @"Enter PIN text field placeholder");
}


- (void)show {
    [self.accessPinJoinButton stopJoinButtonAnimation];
    self.accessPinTextField.text = @"";
    [self disableAccessPinJoinButton];
    [self disableTogglePasswordButton];
}


- (void)hide {

}



#pragma mark - Actions
- (IBAction)togglePasswordButtonTapped:(id)sender {
    if (self.accessPinTextField.isSecureTextEntry)
        [self showPassword];
    else
        [self hidePasword];
}


- (IBAction)accessPinJoinButtonPressed:(id)sender {
    [self.accessPinJoinButton showJoinAnimation];
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(accessPin:connectWithPin:)]) {
        [self.delegate accessPin:self connectWithPin:[self.accessPinTextField.text copy]];
    }
}



#pragma mark -
- (void)showPassword {
    [self.accessPinTextField setSecureTextEntry:NO];
    [self.accessPinTogglePasswordButton setImage:[UIImage imageNamed:@"sm-showPassword"] forState:UIControlStateNormal];
}

- (void)hidePasword {
    [self.accessPinTextField setSecureTextEntry:YES];
    [self.accessPinTogglePasswordButton setImage:[UIImage imageNamed:@"sm-hidePassword"] forState:UIControlStateNormal];
}

- (void)disableTogglePasswordButton {
    [self.accessPinTogglePasswordButton setEnabled:NO];
}

- (void)enableTogglePasswordButton {
    [self.accessPinTogglePasswordButton setEnabled:YES];
}

- (void)disableAccessPinJoinButton {
    [self.accessPinJoinButton setEnabled:NO];
}

- (void)enableAccessPinJoinButton {
    self.accessPinJoinButton.enabled = YES;
}



#pragma mark - Keyboard
- (void)dismissKeyboard {
    if ([self.accessPinTextField isFirstResponder]) [self.accessPinTextField resignFirstResponder];
}

- (void)pinTextFieldDidChange {
    if (self.accessPinTextField.text.length > 0) {
        [self enableAccessPinJoinButton];
        [self enableTogglePasswordButton];
    } else {
        [self disableAccessPinJoinButton];
        [self disableTogglePasswordButton];
    }
}


- (void)pinTextFieldEnterPressed {
    if (self.accessPinTextField.text.length == 0) return;
    [self accessPinJoinButtonPressed:nil];
}


@end
