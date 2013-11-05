//
//  UIImageView+TalkingScene.m
//  TalkingScene
//
//  Created by liwenjian on 13-11-4.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "UIImageView+TalkingScene.h"

@implementation UIImageView (TalkingScene)

- (void)setImageWithURL:(NSURL *)url
{
    [self setImageWithURL:url placeholderImage:nil];
}

- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder
{
    [self setImageWithURL:url placeholderImage:placeholder options:0];
}

- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(TSImageOptions)options
{
    SDWebImageManager *manager = [SDWebImageManager sharedManager];
    [manager cancelForDelegate:self];
    
    self.image = placeholder;
    if (url)
    {
        NSInteger tOptions = options;
        [manager downloadWithURL:url delegate:self options:tOptions];
    }
}

#if NS_BLOCKS_AVAILABLE
- (void)setImageWithURL:(NSURL *)url success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
{
    [self setImageWithURL:url placeholderImage:nil success:success failure:failure];
}

- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
{
    [self setImageWithURL:url placeholderImage:placeholder options:0 success:success failure:failure];
}

- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(TSImageOptions)options success:(TSImageSuccessBlock)success failure:(TSImageFailureBlock)failure;
{
    SDWebImageManager *manager = [SDWebImageManager sharedManager];
    [manager cancelForDelegate:self];
    
    self.image = placeholder;
    if (url)
    {
        NSInteger tOptions = options;
        [manager downloadWithURL:url delegate:self options:tOptions success:success failure:failure];
    }
}
#endif

- (void)cancelCurrentImageLoad
{
    @synchronized(self)
    {
        [[SDWebImageManager sharedManager] cancelForDelegate:self];
    }
}

- (void)webImageManager:(SDWebImageManager *)imageManager didProgressWithPartialImage:(UIImage *)image forURL:(NSURL *)url
{
    self.image = image;
    [self setNeedsLayout];
}

- (void)webImageManager:(SDWebImageManager *)imageManager didFinishWithImage:(UIImage *)image
{
    self.image = image;
    [self setNeedsLayout];
}


@end
