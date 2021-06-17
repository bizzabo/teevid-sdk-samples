//
//  MuteRecomendViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "MuteRecomendViewController.h"


@interface MuteRecomendViewController ()

@property (weak, nonatomic) IBOutlet UILabel *messageLabel;

@end



@implementation MuteRecomendViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepare];
}



#pragma mark - Waiting Room appearance
- (void)prepare {
    self.messageLabel.text = NSLocalizedString(@"Maximum recommended count of publishers has been reached.\nWe recommend you join with your camera and mic muted.", @"Maximum recommended count of publishers has been reached.\nWe recommend you join with your camera and mic muted.");
}

@end
