//
//  TSLoginViewController.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSBaseViewController.h"
#import "TSDialogView.h"

@interface TSLoginViewController : TSBaseViewController<UIGestureRecognizerDelegate,TSDialogViewDelegate>

@property(nonatomic,retain) UIScrollView *srlContainer;
@property(nonatomic,retain) TSDialogView *left1;
@property(nonatomic,retain) TSDialogView *left2;
@property(nonatomic,retain) TSDialogView *right1;
@property(nonatomic,retain) TSDialogView *right2;
@property(nonatomic,retain) UIButton *btnLogin;
@property(nonatomic,retain) NSString *useName;
@property(nonatomic,retain) NSString *password;


@end
