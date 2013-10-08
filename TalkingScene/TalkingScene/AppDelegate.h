//
//  AppDelegate.h
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RESideMenu.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) RESideMenu *sideMenu;
@property (strong, nonatomic) UINavigationController *loginNav;
@property (strong, nonatomic) UINavigationController *homeNav;
@property (strong, nonatomic) UINavigationController *storySelectNav;

+ (NSInteger)OSVersion;

@end
