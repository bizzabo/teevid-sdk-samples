//
//  AccessPinViewController.h
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "SmartJoinEmbedViewController.h"
@class AccessPinViewController;


@protocol AccessPinDelegate <NSObject>

- (void)accessPin:(AccessPinViewController *)viewController connectWithPin:(NSString *)pin;

@end



@interface AccessPinViewController : SmartJoinEmbedViewController

@property (nonatomic, weak) id <AccessPinDelegate> delegate;

- (void)show;
- (void)hide;

- (void)dismissKeyboard;

@end
