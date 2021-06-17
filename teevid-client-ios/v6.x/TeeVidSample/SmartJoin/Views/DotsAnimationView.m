//
//  DotsAnimationView.m
//  TeeVidSample
//
//  Copyright © 2016-2021 cloudAYI. All rights reserved.
//

#import "DotsAnimationView.h"
#import "CircleDot.h"


@interface DotsAnimationView ()

@property (strong, nonatomic) UIView *customView;
@property (weak, nonatomic) IBOutlet CircleDot *leftDot;
@property (weak, nonatomic) IBOutlet CircleDot *centerDot;
@property (weak, nonatomic) IBOutlet CircleDot *rightDot;

@end



@implementation DotsAnimationView

#pragma mark - Init
- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (self) {
        [self loadView];
        [self addSubview:self.customView];
        [self startAnimation];
    }
    return self;
}


- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self loadView];
        if (CGRectIsEmpty(frame)) self.bounds = self.customView.bounds;
        [self addSubview:self.customView];
        [self startAnimation];
    }
    return self;
}


- (void)loadView {
    NSBundle *bundle        = [NSBundle mainBundle];
    NSString *className     = NSStringFromClass([self class]);
    self.customView         = [[bundle loadNibNamed:className owner:self options:nil] firstObject];
    self.customView.frame   = self.bounds;
}



#pragma mark - Animation
- (void)startAnimation {
    [self animateLeftAndRightDots];
    [self animateCenterDots];
}


- (void)animateLeftAndRightDots {
    [UIView animateKeyframesWithDuration:0.36f delay:0.0f options:[self animationOptions] animations:^{
        self.leftDot.alpha      = 0.2f;
        self.rightDot.alpha     = 0.2f;
        self.leftDot.transform  = CGAffineTransformMakeScale(0.75, 0.75);
        self.rightDot.transform = CGAffineTransformMakeScale(0.75, 0.75);
    } completion:nil];
}

- (void)animateCenterDots {
    [UIView animateKeyframesWithDuration:0.36f delay:0.36f options:[self animationOptions] animations:^{
        self.centerDot.alpha        = 0.2f;
        self.centerDot.transform    = CGAffineTransformMakeScale(0.75, 0.75);
    } completion:nil];
}

- (UIViewKeyframeAnimationOptions)animationOptions {
    UIViewKeyframeAnimationOptions options = UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionRepeat | UIViewAnimationOptionAutoreverse | UIViewAnimationOptionCurveLinear;
    return options;
}


@end
