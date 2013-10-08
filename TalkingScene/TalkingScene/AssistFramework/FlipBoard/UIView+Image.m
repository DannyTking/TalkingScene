//
//  UIView+Image.m
//  transform
//
//  Created by liwenjian on 13-9-30.
//
//

#import "UIView+Image.h"
#import <QuartzCore/QuartzCore.h>

@implementation UIView (Image)

- (UIImage*)image
{
	UIImage *image = nil;
	UIGraphicsBeginImageContextWithOptions(self.bounds.size, NO, [UIScreen mainScreen].scale);
	[self.layer renderInContext:UIGraphicsGetCurrentContext()];
	image = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
    
	return image;
}

@end
