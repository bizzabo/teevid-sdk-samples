//
//  SmartJoinEmbedViewController.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "SmartJoinEmbedViewController.h"


@interface SmartJoinEmbedViewController ()
@end


@implementation SmartJoinEmbedViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.layer.masksToBounds   = YES;
    self.view.layer.borderColor     = [UIColor whiteColor].CGColor;
    self.view.layer.borderWidth     = 1.f;
    self.view.layer.cornerRadius    = 10.0f;
}


@end
