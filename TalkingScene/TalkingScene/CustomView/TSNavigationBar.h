//
//  TSNavigationBar.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-17.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TSNavigationBar : UIView

@property(nonatomic,retain) UIImageView *imgBKG;
@property(nonatomic,retain) UIView *viLeft;
@property(nonatomic,retain) UIView *viRight;
@property(nonatomic,retain) UIView *viTitle;
@property(nonatomic,retain) NSString *selfTitle;

-(id)initWithTitle:(NSString*)pTitle;

-(void)setBKGImage:(UIImage*)pImage;
-(void)setTitle:(NSString *)pTitle;
-(void)setTitleView:(UIView*)pTitleView;

-(void)setLeftBtn:(id)pTarget Sel:(SEL)pSel Title:(NSString*)pTitle ImageBKG:(UIImage*)pImage Size:(CGSize)pSize;
-(void)setBackBtn:(id)pTarget Sel:(SEL)pSel;
-(void)setLeftView:(UIView*)pLeftView;

@end
