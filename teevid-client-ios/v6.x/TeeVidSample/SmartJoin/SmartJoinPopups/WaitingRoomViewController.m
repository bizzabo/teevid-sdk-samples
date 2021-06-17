//
//  WaitingRoomViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "WaitingRoomViewController.h"


@interface WaitingRoomViewController ()

@property (weak, nonatomic) IBOutlet UILabel *waitingRoomTitle;
@property (weak, nonatomic) IBOutlet UILabel *waitingRoomDescription;

@end



@implementation WaitingRoomViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepare];
}



#pragma mark - Waiting Room appearance
- (void)prepare {
    self.waitingRoomTitle.text          = NSLocalizedString(@"Wait for the owner of the room", @"wait for the owner of the room");
    self.waitingRoomDescription.text    = NSLocalizedString(@"please choose your video and audio settings before meeting starts", @"please choose your video and audio settings before meeting starts");
}


@end
