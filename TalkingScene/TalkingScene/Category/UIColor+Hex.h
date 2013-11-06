//
//  UIColor+Hex.h
//  TalkingScene
//
//  Created by liwenjian on 13-11-6.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIColor (Hex)
+ (UIColor*)colorWithHex:(NSInteger)hexValue alpha:(CGFloat)alphaValue;
+ (UIColor*)colorWithHex:(NSInteger)hexValue;
+ (UIColor*)whiteColorWithAlpha:(CGFloat)alphaValue;
+ (UIColor*)blackColorWithAlpha:(CGFloat)alphaValue;
+ (UIColor*)colorWithred:(NSInteger)red green:(NSInteger)green blue:(NSInteger)blue;
+ (UIColor*)colorWithred:(NSInteger)red green:(NSInteger)green blue:(NSInteger)blue alpha:(float )alpha;
+ (UIColor *)colorWithHexString:(id)hexString;
@end
