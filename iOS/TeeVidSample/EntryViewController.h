//
//  EntryViewController.h
//  TeeVidSample
//
//  Copyright © 2016-2017 cloudAYI. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface EntryViewController : UIViewController

@property (weak, nonatomic) IBOutlet UITextField *serverAddress;
@property (weak, nonatomic) IBOutlet UITextField *roomId;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *connectButton;

@end

