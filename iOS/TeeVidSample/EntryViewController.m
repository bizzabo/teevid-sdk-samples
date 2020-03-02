//
//  EntryViewController.h
//  TeeVidSample
//
//  Copyright © 2016-2017 cloudAYI. All rights reserved.
//

#import "EntryViewController.h"
#import "ConferenceViewController.h"


@interface EntryViewController () <ConferenceViewControllerDelegate>

@end



@implementation EntryViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib
    
    // Add tap recognizer to hide the keyboard
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
    tap.cancelsTouchesInView = NO;
    [self.view addGestureRecognizer:tap];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"ConnectSegue"]) {
        // Pass input parameters to ConferenceViewController
        ConferenceViewController *conferenceViewController = segue.destinationViewController;
        conferenceViewController.serverAddress = self.serverAddress.text;
        conferenceViewController.roomId = self.roomId.text;
        conferenceViewController.roomDelegate = self;
    }
}


- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)segue sender:(id)sender {
    if ([segue isEqualToString:@"ConnectSegue"]) {
        // Check for required input parameters
        if (self.serverAddress.text.length == 0) {
            [self showAlert:@"Server address is missing"];
            return NO;
        }
        if (self.roomId.text.length == 0) {
            [self showAlert:@"Room ID is missing"];
            return NO;
        }
    }
    
    return YES;
}

-(void)dismissKeyboard {
    [self.serverAddress resignFirstResponder];
    [self.roomId resignFirstResponder];
}

- (void)showAlert:(NSString *)message {
    UIAlertController *alert = [UIAlertController
                                alertControllerWithTitle:nil
                                message:message
                                preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *action = [UIAlertAction
                             actionWithTitle:@"OK"
                             style:UIAlertActionStyleDefault
                             handler:nil];
    [alert addAction:action];
    
    [self presentViewController:alert animated:YES completion:nil];
}


- (void)didExitRoom:(ConferenceViewController *)room {
    [room dismissViewControllerAnimated:YES completion:nil];
}


@end
