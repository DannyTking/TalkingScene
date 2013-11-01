//
//  UIColor+Hex.h
//  Pluto
//
//  Created by 峰 胡 on 12-6-9.
//  Copyright (c) 2012年 Magus. All rights reserved.
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
