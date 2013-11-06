//
//  UIImage+Scale.h
//  JSJJ_iPhone
//
//  Created by lwjchaos on 13-5-14.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (Scale)
+ (UIImage *)scaleToSize:(UIImage *)img size:(CGSize)newsize;
@end
