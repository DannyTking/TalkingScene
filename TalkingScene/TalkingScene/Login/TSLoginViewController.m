//
//  TSLoginViewController.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSLoginViewController.h"


@interface TSLoginViewController ()

@end

@implementation TSLoginViewController

-(void)dealloc
{
    self.left1          = nil;
    self.left2          = nil;
    self.right1         = nil;
    self.right2         = nil;
    self.btnLogin       = nil;
    self.srlContainer   = nil;
    self.useName        = nil;
    self.password       = nil;
    
    [super dealloc];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
    }
    return self;
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self performSelector:@selector(firstAnimation) withObject:nil afterDelay:0.3];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    UITapGestureRecognizer *tapGesture = [[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapBackground:)]autorelease];
    tapGesture.delegate = self;
    [self.view addGestureRecognizer:tapGesture];
    [self setBaseBackgourndColorWithImageName:@"TS_Login_BKG.png"];
    [self.tsNavBar setTitle:@"登录"];
    [self setupSelf];
}

-(void)setupSelf
{
    _srlContainer = [[UIScrollView alloc]initWithFrame:CGRectMake(0,44,self.view.frame.size.width,(IS_IPHONE5?504:416))];
    _srlContainer.backgroundColor = [UIColor clearColor];
    _srlContainer.showsVerticalScrollIndicator = NO;
    _srlContainer.showsHorizontalScrollIndicator = NO;
    _srlContainer.clipsToBounds = YES;
    _srlContainer.bounces = NO;
    [self.view addSubview:_srlContainer];
    
    float gap = IS_IPHONE5?100:80;
    _left1 = [[TSDialogView alloc]initWithType:TSDialogLeftShow PortraitImage:[UIImage imageNamed:@"Cang.jpg"] Text:@"唐爷，好久没来丽春苑玩儿了，今儿个好雅兴啊！请出示您的会员卡。"];
    _left1.frame = CGRectMake(0,20,_left1.frame.size.width,_left1.frame.size.height);
    [_left1 adjustFrameWithMinSize];
    _left1.delegate = self;
    _left1.alpha = 0;
    [_srlContainer addSubview:_left1];
    
    _right1 = [[TSDialogView alloc] initWithType:TSDialogRightInput PortraitImage:[UIImage imageNamed:@"Tang.JPG"] Text:@"请输入用户名。(点这儿)"];
    _right1.frame = CGRectMake(0,gap*1+20,_right1.frame.size.width,_right1.frame.size.height);
    [_right1 adjustFrameWithMinSize];
    _right1.delegate = self;
    _right1.alpha = 0;
    [_srlContainer addSubview:_right1];
    
    _left2 = [[TSDialogView alloc]initWithType:TSDialogLeftShow PortraitImage:[UIImage imageNamed:@"Cang.jpg"] Text:@"来，唐爷再输个密码！"];
    _left2.frame = CGRectMake(0,gap*2+20,_left2.frame.size.width,_left2.frame.size.height);
    [_left2 adjustFrameWithMinSize];
    _left2.delegate = self;
    _left2.alpha = 0;
    [_srlContainer addSubview:_left2];
    
    _right2 = [[TSDialogView alloc] initWithType:TSDialogRightInput PortraitImage:[UIImage imageNamed:@"Tang.JPG"] Text:@"请输入密码。(点这儿)"];
    _right2.frame = CGRectMake(0,gap*3+20,_right2.frame.size.width,_right2.frame.size.height);
    [_right2 adjustFrameWithMinSize];
    _right2.delegate = self;
    _right2.alpha = 0;
    [_srlContainer addSubview:_right2];
    
    _btnLogin = [[UIButton alloc] initWithFrame:CGRectMake(100,gap*4+20,120,44)];
    [_btnLogin setTitle:@"登录" forState:UIControlStateNormal];
    [_btnLogin setTitle:@"登录" forState:UIControlStateHighlighted];
    [_btnLogin setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [_btnLogin setTitleColor:[UIColor redColor] forState:UIControlStateHighlighted];
    [_btnLogin setBackgroundColor:[UIColor blueColor]];
    [_btnLogin addTarget:self action:@selector(tapBtnLogin:) forControlEvents:UIControlEventTouchUpInside];
    [_btnLogin setAlpha:0];
    [_srlContainer addSubview:_btnLogin];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Self Method
//#################################################################################
//#################################################################################
-(void)tapBtnLogin:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

-(void)tapBackground:(id)sender
{
    [_right1.tfDialog resignFirstResponder];
    [_right2.tfDialog resignFirstResponder];
}

-(void)firstAnimation
{
    [UIView animateWithDuration:1 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations:
     ^{
         [_left1 adjustFrameWithCurrent];
         _left1.alpha = 1;
     }completion:^(BOOL finished)
     {
         [UIView animateWithDuration:1 delay:0.1 options:UIViewAnimationOptionCurveEaseInOut animations:
          ^{
              [_right1 adjustFrameWithCurrent];
              _right1.alpha = 1;
          }completion:^(BOOL finished)
          {
              [_right1.tfDialog becomeFirstResponder];
          }];
     }];
}

-(void)secondAnimation
{
    [UIView animateWithDuration:1 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations:
     ^{
         [_left2 adjustFrameWithCurrent];
         _left2.alpha = 1;
     }completion:^(BOOL finished)
     {
         [UIView animateWithDuration:1 delay:0.1 options:UIViewAnimationOptionCurveEaseInOut animations:
          ^{
              [_right2 adjustFrameWithCurrent];
              _right2.alpha = 1;
              _btnLogin.alpha = 1;
          }completion:^(BOOL finished)
          {
              [_right2.tfDialog becomeFirstResponder];
          }];
     }];
}


#pragma mark - TSDialogViewDelegate Method
//#################################################################################
//#################################################################################
-(void)dialogDidEndOnExit:(TSDialogView *)pDialogView Text:(NSString *)pText
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    if(pDialogView == _right1)
    {
    }
    else if(pDialogView == _right2)
    {
    }
    else
    {
    }
}

-(void)dialogEditingDidBegin:(TSDialogView *)pDialogView Text:(NSString *)pText
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    if(pDialogView == _right1)
    {
    }
    else if(pDialogView == _right2)
    {
        _srlContainer.contentSize = CGSizeMake(_srlContainer.frame.size.width,_srlContainer.frame.size.height+217);
        [_srlContainer setContentOffset:CGPointMake(0,217) animated:YES];
    }
    else
    {
    }
}

-(void)dialogEditingDidEnd:(TSDialogView *)pDialogView Text:(NSString *)pText
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    if(pDialogView == _right1)
    {
        self.useName = pText;
        [self secondAnimation];
    }
    else if(pDialogView == _right2)
    {
        self.password = pText;
        [UIView animateWithDuration:0.5 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations:
         ^{
             [_srlContainer setContentOffset:CGPointMake(0,0)];
         }completion:^(BOOL finished)
         {
             _srlContainer.contentSize = _srlContainer.frame.size;
         }];
    }
    else
    {
    }
}

#pragma mark - UIGestureRecognizerDelegate Method
//#################################################################################
//#################################################################################
-(BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    if(touch.view == _btnLogin) return NO;
    return YES;
}

@end





























