//
//  TSBaseViewController.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RESideMenu.h"
#import "TSNavigationBar.h"

typedef void (^TSNavigationControllerCompletionBlock)(void);

@interface TSBaseViewController : UIViewController

@property(nonatomic,retain) TSNavigationBar *tsNavBar;


-(void)setupNavBarWithTitle:(NSString*)pTitle;

- (void) tsPushViewController:(UIViewController *)viewController;
- (void) tsPushViewController:(UIViewController *)viewController completion:(TSNavigationControllerCompletionBlock)handler;
- (void) tsPopViewController;
- (void) tsPopViewControllerWithCompletion:(TSNavigationControllerCompletionBlock)handler;

@end
