//
//  JoinButton.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "JoinButton.h"
#import "DotsAnimationView.h"


@interface JoinButton()

@property (strong, nonatomic) DotsAnimationView *animationView;

@end



@implementation JoinButton

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    
    if (self) {
        self.layer.masksToBounds    = YES;
        self.layer.cornerRadius     = 20.0f;
                
        [self stopJoinButtonAnimation];
        [self addAnimationView];
    }
    return self;
}


- (NSString *)joinButtonTitle {
    return [NSLocalizedString(@"Join meeting", @"Join meeting title") uppercaseString];
}


- (void)addAnimationView {
    _animationView = [[DotsAnimationView alloc] initWithFrame:self.bounds];
    [self addSubview:_animationView];
    _animationView.translatesAutoresizingMaskIntoConstraints = NO;
    NSLayoutConstraint *topConstraint       = [NSLayoutConstraint constraintWithItem:_animationView attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeTop multiplier:1.0f constant:0];
    
    NSLayoutConstraint *bottomConstraint    = [NSLayoutConstraint constraintWithItem:_animationView attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0];
    
    NSLayoutConstraint *leftConstraint      = [NSLayoutConstraint constraintWithItem:_animationView attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeLeft multiplier:1.0 constant:0];
    
    NSLayoutConstraint *trailingConstraint  = [NSLayoutConstraint constraintWithItem:_animationView attribute:NSLayoutAttributeTrailing relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeTrailing multiplier:1.0 constant:0];
    
    [NSLayoutConstraint activateConstraints:@[topConstraint, bottomConstraint, leftConstraint, trailingConstraint]];
}


- (void)showJoinAnimation {
    [self setTitle:@"" forState:UIControlStateNormal];
    self.backgroundColor = [UIColor clearColor];
    self.animationView.hidden = NO;
}


- (void)stopJoinButtonAnimation {
    [self setTitle:[self joinButtonTitle] forState:UIControlStateNormal];
    self.backgroundColor = [UIColor whiteColor];
    self.animationView.hidden = YES;
}


@end
