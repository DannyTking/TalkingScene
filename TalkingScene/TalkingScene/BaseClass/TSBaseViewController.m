//
//  TSBaseViewController.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSBaseViewController.h"

@interface TSBaseViewController ()

@end

@implementation TSBaseViewController

-(void)dealloc
{
    self.tsNavBar = nil;
    [super dealloc];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    UIPanGestureRecognizer *gestureRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(swipeHandler:)];
    [self.view addGestureRecognizer:gestureRecognizer];
    [self setupNavBarWithTitle:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - TSNavigationBar Relate Method
//########################################################################
//########################################################################
-(void)setupNavBarWithTitle:(NSString*)pTitle
{
    [self.tsNavBar removeFromSuperview];
    self.tsNavBar = nil;
    
    if(pTitle == nil || [pTitle isEqualToString:@""])
    {
        pTitle = self.title;
    }
    _tsNavBar = [[TSNavigationBar alloc] initWithTitle:pTitle];
//    if(self.flipboardNavigationController != nil && [self.flipboardNavigationController.viewControllers objectAtIndex:0] != self)
//    {
//        [_tsNavBar setBackBtn:self Sel:@selector(tsPopViewController)];
//    }
    [_tsNavBar setBackBtn:self Sel:@selector(tsPopViewController)];
    [self.view addSubview:_tsNavBar];
}

#pragma mark - FlipBoardNavigationController Relate Method
//########################################################################
//########################################################################
- (void) tsPushViewController:(UIViewController *)viewController
{
    [self.flipboardNavigationController pushViewController:viewController];
}

- (void) tsPushViewController:(UIViewController *)viewController completion:(TSNavigationControllerCompletionBlock)handler
{
    [self.flipboardNavigationController pushViewController:viewController completion:handler];
}

- (void) tsPopViewController
{
    [self.flipboardNavigationController popViewController];
}

- (void) tsPopViewControllerWithCompletion:(TSNavigationControllerCompletionBlock)handler
{
    [self.flipboardNavigationController popViewControllerWithCompletion:handler];
}

#pragma mark - RESideMenu Relate Method
//########################################################################
//########################################################################
- (void)swipeHandler:(UIPanGestureRecognizer *)sender
{
    [[self sideMenu] showFromPanGesture:sender];
}

#pragma mark - Base Method
//########################################################################
//########################################################################
- (void)setBaseBackgourndColorWithImageName:(NSString *)imageName
{
    UIColor *bgColor = [UIColor colorWithPatternImage:[UIImage imageNamed:imageName]];
    if ([self.view isKindOfClass:[UITableView class]])
    {
        UIView *viewi = [[[UIView alloc] initWithFrame:self.view.bounds] autorelease];
        viewi.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        viewi.backgroundColor = bgColor;
        UITableView *tView = (UITableView *)self.view;
        if ([tView respondsToSelector:@selector(setBackgroundView:)])
        {
            [tView setBackgroundView:viewi];
        }
    }
    self.view.backgroundColor = bgColor;
}



@end






