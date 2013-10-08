//
//  AppDelegate.m
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "TSLoginViewController.h"
#import "TSStorySelectViewController.h"

@implementation AppDelegate

+ (NSInteger)OSVersion
{
    static NSUInteger _deviceSystemMajorVersion = -1;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _deviceSystemMajorVersion = [[[[[UIDevice currentDevice] systemVersion] componentsSeparatedByString:@"."] objectAtIndex:0] intValue];
    });
    return _deviceSystemMajorVersion;
}

- (void)dealloc
{
    [_window release];
    [_sideMenu release];
    [_loginNav release];
    [_homeNav release];
    [_storySelectNav release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    TSLoginViewController *loginViewController = [[TSLoginViewController alloc] init];
    _loginNav = [[UINavigationController alloc] initWithRootViewController:loginViewController];
    
    ViewController *homeViewController = [[ViewController alloc] init];
    _homeNav = [[UINavigationController alloc] initWithRootViewController:homeViewController];
    
    TSStorySelectViewController *storyViewController = [[TSStorySelectViewController alloc] init];
    _storySelectNav = [[UINavigationController alloc] initWithRootViewController:storyViewController];
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    
    RESideMenuItem *loginItem = [[RESideMenuItem alloc] initWithTitle:@"登录" action:^(RESideMenu *menu, RESideMenuItem *item){
        [menu displayContentController:_loginNav];
    }];
    
    RESideMenuItem *homeItem = [[RESideMenuItem alloc] initWithTitle:@"首页" action:^(RESideMenu *menu, RESideMenuItem *item){
        [menu displayContentController:_homeNav];
    }];
    
    RESideMenuItem *storyItem = [[RESideMenuItem alloc] initWithTitle:@"选故事" action:^(RESideMenu *menu, RESideMenuItem *item){
        [menu displayContentController:_storySelectNav];
    }];
    
    _sideMenu = [[RESideMenu alloc] initWithItems:@[loginItem, homeItem, storyItem]];
    _sideMenu.verticalPortraitOffset = IS_WIDESCREEN ? 110 : 76;
    _sideMenu.verticalLandscapeOffset = 16;
    _sideMenu.hideStatusBarArea = [AppDelegate OSVersion] < 7;
    _sideMenu.openStatusBarStyle = UIStatusBarStyleBlackTranslucent;
    storyItem.action(_sideMenu, storyItem);
    
    self.window.rootViewController = _sideMenu;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
