//
//  TSNavigationBar.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-17.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#define DefFrame CGRectMake(0,0,320,44) //默认frame
#define MaxTitleWidth 120 //标题最大宽度
#define MaxBtnWidth 80 //按钮最大宽度
#define DefGap 15 //按钮与边的默认距离


#import "TSNavigationBar.h"

@implementation TSNavigationBar

-(void)dealloc
{
    self.imgBKG     = nil;
    self.viLeft     = nil;
    self.viRight    = nil;
    self.viTitle    = nil;
    [super dealloc];
}

-(id)init
{
    self = [super initWithFrame:DefFrame];
    if (self)
    {
        self.selfTitle = nil;
        [self setupSelf];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
//    self = [super initWithFrame:frame];
    self = [super initWithFrame:DefFrame];
    if (self)
    {
        self.selfTitle = nil;
        [self setupSelf];
    }
    return self;
}

-(id)initWithTitle:(NSString*)pTitle
{
    self = [super initWithFrame:DefFrame];
    if (self)
    {
        self.selfTitle = pTitle;
        [self setupSelf];
    }
    return self;
}

#pragma mark - Self Method
//########################################################################
//########################################################################
-(void)setupSelf
{
    if(_imgBKG == nil)
    {
        _imgBKG = [[UIImageView alloc] initWithFrame:self.bounds];
        [self addSubview:_imgBKG];
    }
    [_imgBKG setImage:[UIImage imageNamed:@"TS_Com_NavBar_BKG.png"]];
    _viLeft     = nil;
    _viRight    = nil;
    _viTitle    = nil;
    
    if(_selfTitle != nil)
    {
        [self setTitle:_selfTitle];
    }
}

#pragma mark - External Method Title && Background
//########################################################################
//########################################################################
-(void)setBKGImage:(UIImage*)pImage
{
    _imgBKG.image = pImage;
}

-(void)setTitle:(NSString *)pTitle
{
    UILabel *tLabel = nil;
    if(_viTitle == nil || ![_viTitle isKindOfClass:[UILabel class]])
    {
        [_viTitle removeFromSuperview];
        self.viTitle = nil;
        
        CGRect tFrame = CGRectMake(0,0,0,0);
        tFrame.size.width = MaxTitleWidth;
        tFrame.size.height = 30;
        tFrame.origin.x = (self.frame.size.width - tFrame.size.width)/2;
        tFrame.origin.y = (self.frame.size.height - tFrame.size.height)/2;
        
        tLabel = [[UILabel alloc] initWithFrame:tFrame];
        [tLabel setTextColor:[UIColor whiteColor]];
        [tLabel setTextAlignment:NSTextAlignmentCenter];
        [tLabel setBackgroundColor:[UIColor clearColor]];
        [tLabel setFont:[UIFont fontWithName:@"Bradley Hand" size:20]];
        if(SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(6.0))
        {
            [tLabel setMinimumScaleFactor:0.6];
            [tLabel setLineBreakMode:NSLineBreakByTruncatingTail];
        }
        else
        {
            [tLabel setMinimumFontSize:12.0];
            [tLabel setLineBreakMode:UILineBreakModeTailTruncation];
        }
        _viTitle = tLabel;
        [self addSubview:_viTitle];
    }
    tLabel = (UILabel *)_viTitle;
    if(pTitle != nil)
    {
        self.selfTitle = pTitle;
        [tLabel setText:pTitle];
    }
}

-(void)setTitleView:(UIView*)pTitleView
{
    CGRect tFrame = pTitleView.frame;
    tFrame.size.width = MIN(tFrame.size.width,MaxTitleWidth);
    tFrame.size.height = MIN(tFrame.size.height,self.frame.size.height);
    tFrame.origin.x = (self.frame.size.width - tFrame.size.width)/2;
    tFrame.origin.y = (self.frame.size.height - tFrame.size.height)/2;
    pTitleView.frame = tFrame;
    [self.viTitle removeFromSuperview];
    self.viTitle = pTitleView;
    [self addSubview:_viTitle];
}

#pragma mark - External Method Left
//########################################################################
//########################################################################
-(void)setLeftBtn:(id)pTarget Sel:(SEL)pSel Title:(NSString*)pTitle ImageBKG:(UIImage*)pImage Size:(CGSize)pSize;
{
    UIButton *tBtn = nil;
    if(_viLeft == nil || ![_viLeft isKindOfClass:[UIButton class]])
    {
        [_viLeft removeFromSuperview];
        self.viLeft = nil;
        
        CGRect tFrame = CGRectMake(0,0,0,0);
        tFrame.size.width = MIN(MaxBtnWidth,pSize.width);//有待优化
        tFrame.size.height = MIN(self.frame.size.height,pSize.height);
        tFrame.origin.x = DefGap;
        tFrame.origin.y = (self.frame.size.height - tFrame.size.height)/2;
        
        tBtn = [[UIButton alloc] initWithFrame:tFrame];
        [tBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [tBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
        _viLeft = tBtn;
        [self addSubview:_viLeft];
    }
    tBtn = (UIButton*)_viLeft;
    [tBtn setTitle:pTitle forState:UIControlStateNormal];
    [tBtn setTitle:pTitle forState:UIControlStateHighlighted];
    [tBtn setBackgroundImage:pImage forState:UIControlStateNormal];
    [tBtn setBackgroundImage:pImage forState:UIControlStateHighlighted];
    [tBtn addTarget:pTarget action:pSel forControlEvents:UIControlEventTouchUpInside];
}

-(void)setBackBtn:(id)pTarget Sel:(SEL)pSel
{
    [self setLeftBtn:pTarget Sel:pSel Title:@"" ImageBKG:[UIImage imageNamed:@"TS_Com_Back_Icon.png"] Size:CGSizeMake(40,30)];
}

-(void)setLeftView:(UIView*)pLeftView
{
    CGRect tFrame = pLeftView.frame;
    tFrame.size.width = MIN(MaxBtnWidth,tFrame.size.width);
    tFrame.size.height = MIN(tFrame.size.height,self.frame.size.height);
    tFrame.origin.x = DefGap;
    tFrame.origin.y = (self.frame.size.height - tFrame.size.height)/2;
    pLeftView.frame = tFrame;
    [self.viLeft removeFromSuperview];
    self.viLeft = pLeftView;
    [self addSubview:_viLeft];
}

@end



















