//
//  ConferenceViewController.h
//  TeeVidSample
//
//  Copyright © 2016-2018 cloudAYI. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <TeeVidClient/TeeVidClient.h>

@interface ConferenceViewController : UIViewController <TeeVidClientDelegate>

@property (nonatomic, strong) NSString *serverAddress;
@property (nonatomic, strong) NSString *roomId;

@property (weak, nonatomic) IBOutlet UIBarButtonItem *disconnectButton;

@end
