//
//  UINavigationBar+NavBarCfg.m
//  JSFundSec
//
//  Created by yueshenyuan on 13-7-23.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import "UINavigationBar+NavBarCfg.h"

@implementation UINavigationBar (NavBarCfg)

- (void)drawRect:(CGRect)rect {
    UIImage *image = [UIImage imageNamed:@"bar_1.png"];
    [image drawInRect:CGRectMake(self.bounds.origin.x, self.bounds.origin.y, self.bounds.size.width, self.bounds.size.height+2) ];
    self.tintColor = [UIColor clearColor];
}

- (void)didMoveToSuperview {
    [super didMoveToSuperview];
    
    if (self.superview != nil) {
        
        UIImage *image = [UIImage imageNamed:@"bj_hongse.png"];
        SEL sel = @selector(setBackgroundImage:forBarMetrics:);
        if ([self respondsToSelector:sel]) {
            
            NSMethodSignature *signature = [self methodSignatureForSelector:sel];
            NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
            [invocation setTarget:self];
            [invocation setSelector:sel];
            int metrics = 0;    // UIBarMetricsDefault;
            [invocation setArgument:&image atIndex:2];
            [invocation setArgument:&metrics atIndex:3];
            [invocation invoke];
        }
    }
}
//让title居中 否则会根据lefttile 和righttitle的 存在与否进行变化
//-(void)layoutSubviews {
////    [super layoutSubviews];
//    for (id view in self.subviews) {
//        if ([view isKindOfClass:NSClassFromString(@"UILabel")]) {
//            // Do whatever you want with view here
//            UILabel *lab = (UILabel *)view;
//            lab.backgroundColor = [UIColor clearColor];
//            [lab setTextColor:[UIColor blackColor]];
//			[view setFrame:CGRectMake(0 ,0, 320, 44)];
//        }
//    }
//}

@end
