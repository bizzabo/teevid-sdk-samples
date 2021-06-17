//
//  Utils.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "Utils.h"

@implementation Utils

+ (void)showMessage:(NSString*)message withTitle:(NSString *)title {
    [Utils showMessage:message withTitle:title handler:^(UIAlertAction *action) {}];
}

+ (void)showMessage:(NSString*)message withTitle:(NSString *)title handler:(void (^)(UIAlertAction *action))handler {
    UIAlertController *alert    = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
    NSString *actionTitle       = NSLocalizedString(@"OK", @"OK button title");
    UIAlertAction *okAction     = [UIAlertAction actionWithTitle:actionTitle style:UIAlertActionStyleDefault handler:handler];
    [alert addAction:okAction];
    
    UIViewController *topController = [UIApplication sharedApplication].keyWindow.rootViewController;
    
    while (topController.presentedViewController) {
        topController = topController.presentedViewController;
    }
    [topController presentViewController:alert animated:YES completion:nil];
}


#pragma mark - Device Type
+ (BOOL)isIPad {
    BOOL ipad = UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? YES : NO;
    return ipad;
}

+ (BOOL)isIPhone {
    BOOL iphone = UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone ? YES : NO;
    return iphone;
}

+ (BOOL)isIPhone5 {
    CGSize screenSize   = [UIScreen mainScreen].bounds.size;
    CGFloat width       = screenSize.width;
    CGFloat height      = screenSize.height;
    
    // 320x480 - this is iPhone 4 or 4S
    // 320x568 - this is iPhone 5 or 5S
    // we need to check against both of those devices
    if ((width == 320 && height == 568) || (width == 568 && height == 320) || (width == 320 && height == 480) || (width == 480 && height == 320)) {
        return YES;
    }
    else return NO;
}

@end
