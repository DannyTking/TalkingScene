//
//  UIImage+Scale.m
//  JSJJ_iPhone
//
//  Created by lwjchaos on 13-5-14.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import "UIImage+Scale.h"

@implementation UIImage (Scale)

+ (UIImage *)scaleToSize:(UIImage *)img size:(CGSize)newsize
{
    UIGraphicsBeginImageContext(newsize);
    [img drawInRect:CGRectMake(0, 0, newsize.width, newsize.height)];
    UIImage* scaledImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return scaledImage;
}


@end
