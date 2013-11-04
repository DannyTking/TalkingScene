//
//  UIImageView+TalkingScene.h
//  TalkingScene
//
//  Created by liwenjian on 13-11-4.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SDWebImageManager.h"
#import "SDWebImageManagerDelegate.h"

typedef enum
{
    TSImageRetryFailed = 1 << 0,
    TSImageLowPriority = 1 << 1,
    TSImageCacheMemoryOnly = 1 << 2,
    TSImageProgressiveDownload = 1 << 3
} TSImageOptions;

#if NS_BLOCKS_AVAILABLE
typedef void(^TSImageSuccessBlock)(UIImage *image, BOOL cached);
typedef void(^TSImageFailureBlock)(NSError *error);
#endif





@interface UIImageView (TalkingScene)<SDWebImageManagerDelegate>

- (void)setImageWithURL:(NSURL *)url;
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder;
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(TSImageOptions)options;

#if NS_BLOCKS_AVAILABLE
- (void)setImageWithURL:(NSURL *)url success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(TSImageOptions)options success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
#endif
- (void)cancelCurrentImageLoad;

@end















