//
//  AppDelegate.h
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RESideMenu.h"
#import "FlipBoardNavigationController.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) RESideMenu *sideMenu;
@property (strong, nonatomic) FlipBoardNavigationController *loginNav;
@property (strong, nonatomic) FlipBoardNavigationController *homeNav;
@property (strong, nonatomic) FlipBoardNavigationController *storySelectNav;

+ (NSInteger)OSVersion;

@end
