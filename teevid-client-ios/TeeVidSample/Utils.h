//
//  Utils.h
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface Utils : NSObject

+ (void)showMessage:(NSString*)message withTitle:(NSString *)title;
+ (void)showMessage:(NSString*)message withTitle:(NSString *)title handler:(void (^)(UIAlertAction *action))handler;

+ (BOOL)isIPad;
+ (BOOL)isIPhone;
+ (BOOL)isIPhone5;

@end
