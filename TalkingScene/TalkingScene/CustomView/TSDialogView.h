//
//  TSDialogView.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-24.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RTLabel.h"

typedef enum
{
    TSDialogLeftShow=0, //左边对话展示
    TSDialogRightShow, //右边对话展示
    TSDialogLeftInput, //左边对话输入
    TSDialogRightInput //右边对话输入
    
} TSDialogType;

@protocol TSDialogViewDelegate;
@interface TSDialogView : UIView

@property (nonatomic,retain) UIView *viPortrait;
@property (nonatomic,retain) UIImageView *imgViPortrait;
@property (nonatomic,retain) UIImageView *imgViPortraitBorder;
@property (nonatomic,retain) UIImageView *imgViSpeechBubble;
@property (nonatomic,retain) RTLabel *lbDialog;
@property (nonatomic,retain) UITextField *tfDialog;

@property (nonatomic,retain) UIImage *portraitImage;
@property (nonatomic,retain) NSString *portraitURL;
@property (nonatomic,retain) NSString *dialogText;
@property (nonatomic,assign) TSDialogType dialogType;
@property (nonatomic,retain) id<TSDialogViewDelegate> delegate;

-(id)initWithType:(TSDialogType)pType PortraitImage:(UIImage *)pImage Text:(NSString*)pText;
-(id)initWithType:(TSDialogType)pType PortraitUrl:(NSString *)pUrl Text:(NSString*)pText;

-(void)adjustFrameWithCurrent;
-(void)adjustFrameWithMinSize;
-(void)adjustFrameWithType:(TSDialogType)pType DialogText:(NSString*)pText;
-(CGSize)optimumSize;
-(CGSize)optimumSizeWithType:(TSDialogType)pType DialogText:(NSString*)pText;
-(void)setTextColor:(UIColor *)pColor;
-(void)setTextFont:(UIFont *)pFont;

@end

@protocol TSDialogViewDelegate <NSObject>
-(void)dialogDidEndOnExit:(TSDialogView *)pDialogView Text:(NSString *)pText;
-(void)dialogEditingDidBegin:(TSDialogView *)pDialogView Text:(NSString *)pText;
-(void)dialogEditingDidEnd:(TSDialogView *)pDialogView Text:(NSString *)pText;
@end










