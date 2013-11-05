//
//  TSDialogView.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-24.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSDialogView.h"
#import <QuartzCore/QuartzCore.h>
#import "UIImageView+TalkingScene.h"

const float TSDialogDefHeight = 70;//Dialog Default Height
const float TSDialogDefWidth = 320;//Dialog Default Width

const float TSPortraitWH = 60;//Portrait Width and Height
const float TSPortraitLG = 5;//Portrait Left Gap(距左边的距离)
const float TSPortraitRG = 2;//Portrait Right Gap
const float TSPortraitBG = 8;//Portrait Bottom Gap

const float TSSBMaxWidth = 277;//Speech Bubble Max Width
const float TSSBMinWidth = 72;//Speech Bubble Min Width
const float TSSBLeftGap = 38;//Speech Bubble Left Gap
const float TSSBRightGap = 38;//Speech Bubble Right Gap
const float TSSBHeightDif = 0;//Speech Bubble（imgViSpeechBubble）与 DialogView间的合适高度差
const float TSSBWidthDif = 43;//Speech Bubble（imgViSpeechBubble）与 DialogView间的合适宽度差

const float TSDCMaxWidth = 230;//Dialog Content Max Width
const float TSDCLeftGap = 76;//Dialog Content Left Gap
const float TSDCRightGap = 76;//Dialog Content Right Gap
const float TSDCHeightDif = 16;//对话内容(lbDialog)与Speech Bubble间的合适高度差
const float TSDCWidthDif = 47;//对话内容(lbDialog)与Speech Bubble间的合适宽度差
const float TSDCLeftGapRightType = TSDialogDefWidth-(TSDCMaxWidth+TSDCRightGap);//Dialog Content Left Gap

@implementation TSDialogView

-(void)dealloc
{
    self.viPortrait             = nil;
    self.imgViPortrait          = nil;
    self.imgViPortraitBorder    = nil;
    self.imgViSpeechBubble      = nil;
    self.lbDialog               = nil;
    self.tfDialog               = nil;
    self.portraitImage          = nil;
    self.portraitURL            = nil;
    self.dialogText             = nil;
    self.delegate               = nil;
    [super dealloc];
}

-(id)init
{
    return [self initWithType:TSDialogLeftShow PortraitImage:nil Text:nil];
}

- (id)initWithFrame:(CGRect)frame
{
    return [self initWithType:TSDialogLeftShow PortraitImage:nil Text:nil];
}

-(id)initWithType:(TSDialogType)pType PortraitImage:(UIImage *)pImage Text:(NSString*)pText
{
    CGRect frame = CGRectMake(0,0,TSDialogDefWidth,TSDialogDefHeight);
    self = [super initWithFrame:frame];
    if (self)
    {
        self.dialogType = pType;
        self.portraitImage = pImage;
        self.dialogText = pText;
        [self setupSelfDefault];
    }
    return self;
}

-(id)initWithType:(TSDialogType)pType PortraitUrl:(NSString *)pUrl Text:(NSString*)pText
{
    CGRect frame = CGRectMake(0,0,TSDialogDefWidth,TSDialogDefHeight);
    self = [super initWithFrame:frame];
    if (self)
    {
        self.dialogType = pType;
        self.portraitURL = pUrl;
        self.dialogText = pText;
        [self setupSelfDefault];
    }
    return self;
}

-(void)setupSelfDefault
{
    self.clipsToBounds = YES;
    self.autoresizesSubviews = NO;
    self.backgroundColor = [UIColor clearColor];
    
    if(_dialogType == TSDialogLeftInput || _dialogType == TSDialogLeftShow)//左
    {
        _viPortrait = [[UIView alloc] initWithFrame:CGRectMake(TSPortraitLG,TSDialogDefHeight-(TSPortraitWH+TSPortraitBG),TSPortraitWH,TSPortraitWH)];
        _viPortrait.clipsToBounds = YES;
        _viPortrait.autoresizesSubviews = NO;
        _viPortrait.backgroundColor = [UIColor clearColor];
        [self addSubview:_viPortrait];
        
        _imgViPortrait = [[UIImageView alloc] initWithFrame:CGRectMake(3,2,TSPortraitWH-6,TSPortraitWH-6)];
        _imgViPortrait.layer.cornerRadius = (TSPortraitWH-6)/2;
        _imgViPortrait.layer.masksToBounds = YES;
        _imgViPortrait.image = [UIImage imageNamed:@""];
        _imgViPortrait.backgroundColor = [UIColor clearColor];
        [_viPortrait addSubview:_imgViPortrait];
        
        _imgViPortraitBorder = [[UIImageView alloc] initWithFrame:CGRectMake(0,0,TSPortraitWH,TSPortraitWH)];
        _imgViPortraitBorder.image = [UIImage imageNamed:@"TS_Cus_DialogPortrait_BKG.png"];
        _imgViPortraitBorder.backgroundColor = [UIColor clearColor];
        [_viPortrait addSubview:_imgViPortraitBorder];
        
        _imgViSpeechBubble = [[UIImageView alloc] initWithFrame:CGRectMake(TSSBLeftGap,0,TSSBMaxWidth,TSDialogDefHeight)];
        _imgViSpeechBubble.image = [[UIImage imageNamed:@"TS_Cus_DialogLeft_BKG.png"]resizableImageWithCapInsets:UIEdgeInsetsMake(19,49,41,21)];
        _imgViSpeechBubble.backgroundColor = [UIColor clearColor];
        [self addSubview:_imgViSpeechBubble];
        
        _lbDialog = [[RTLabel alloc] initWithFrame:CGRectMake(TSDCLeftGap,TSDCHeightDif/2,TSDCMaxWidth,TSDialogDefHeight-TSDCHeightDif)];
        _lbDialog.backgroundColor = [UIColor clearColor];
        _lbDialog.paragraphReplacement = @"";
        _lbDialog.font=[UIFont systemFontOfSize:14.0];
        _lbDialog.textColor = [UIColor colorWithHex:0x000000];
        _lbDialog.shadowColor = [UIColor colorWithHex:0x000000];
        _lbDialog.shadowOffset = CGSizeMake(0,0);
        _lbDialog.lineSpacing = 1.0f;
        _lbDialog.lineBreakMode = RTTextLineBreakModeWordWrapping;
        _lbDialog.text = @"";
        _lbDialog.hidden = !(_dialogType == TSDialogLeftShow || _dialogType == TSDialogRightShow);
        [self addSubview:_lbDialog];
        
        _tfDialog = [[UITextField alloc]initWithFrame:CGRectMake(TSDCLeftGap,TSDCHeightDif/2,TSDCMaxWidth,TSDialogDefHeight-TSDCHeightDif)];
        _tfDialog.backgroundColor = [UIColor clearColor];
        _tfDialog.font = [UIFont systemFontOfSize:14.0];
        _tfDialog.textColor = [UIColor colorWithHex:0x999999];
        _tfDialog.borderStyle = UITextBorderStyleNone;
        _tfDialog.adjustsFontSizeToFitWidth = YES;
        _tfDialog.minimumFontSize = 10;
        _tfDialog.placeholder = @"";
        _tfDialog.contentVerticalAlignment = UIControlContentHorizontalAlignmentCenter;
        _tfDialog.hidden = !(_dialogType == TSDialogLeftInput || _dialogType == TSDialogRightInput);
        [_tfDialog addTarget:self action:@selector(dialogDidEndOnExit:) forControlEvents:UIControlEventEditingDidEndOnExit];
        [_tfDialog addTarget:self action:@selector(dialogEditingDidBegin:) forControlEvents:UIControlEventEditingDidBegin];
        [_tfDialog addTarget:self action:@selector(dialogEditingDidEnd:) forControlEvents:UIControlEventEditingDidEnd];
        [self addSubview:_tfDialog];
    }
    else//右
    {
        _viPortrait = [[UIView alloc] initWithFrame:CGRectMake(TSDialogDefWidth-(TSPortraitWH+TSPortraitRG),TSDialogDefHeight-(TSPortraitWH+TSPortraitBG),TSPortraitWH,TSPortraitWH)];
        _viPortrait.clipsToBounds = YES;
        _viPortrait.autoresizesSubviews = NO;
        _viPortrait.backgroundColor = [UIColor clearColor];
        [self addSubview:_viPortrait];
        
        _imgViPortrait = [[UIImageView alloc] initWithFrame:CGRectMake(3,2,TSPortraitWH-6,TSPortraitWH-6)];
        _imgViPortrait.layer.cornerRadius = (TSPortraitWH-6)/2;
        _imgViPortrait.layer.masksToBounds = YES;
        _imgViPortrait.image = [UIImage imageNamed:@""];
        _imgViPortrait.backgroundColor = [UIColor clearColor];
        [_viPortrait addSubview:_imgViPortrait];
        
        _imgViPortraitBorder = [[UIImageView alloc] initWithFrame:CGRectMake(0,0,TSPortraitWH,TSPortraitWH)];
        _imgViPortraitBorder.image = [UIImage imageNamed:@"TS_Cus_DialogPortrait_BKG.png"];
        _imgViPortraitBorder.backgroundColor = [UIColor clearColor];
        [_viPortrait addSubview:_imgViPortraitBorder];
        
        _imgViSpeechBubble = [[UIImageView alloc] initWithFrame:CGRectMake(TSDialogDefWidth-(TSSBMaxWidth+TSSBRightGap),0,TSSBMaxWidth,TSDialogDefHeight)];
        _imgViSpeechBubble.image = [[UIImage imageNamed:@"TS_Cus_DialogRight_BKG.png"]resizableImageWithCapInsets:UIEdgeInsetsMake(21,21,41,49)];
        _imgViSpeechBubble.backgroundColor = [UIColor clearColor];
        [self addSubview:_imgViSpeechBubble];
        
        _lbDialog = [[RTLabel alloc] initWithFrame:CGRectMake(TSDialogDefWidth-(TSDCLeftGap+TSDCMaxWidth),TSDCHeightDif/2,TSDCMaxWidth,TSDialogDefHeight-TSDCHeightDif)];
        _lbDialog.backgroundColor = [UIColor clearColor];
        _lbDialog.paragraphReplacement = @"";
        _lbDialog.font=[UIFont systemFontOfSize:14.0];
        _lbDialog.textColor = [UIColor colorWithHex:0x000000];
        _lbDialog.shadowColor = [UIColor colorWithHex:0x000000];
        _lbDialog.shadowOffset = CGSizeMake(0,0);
        _lbDialog.lineSpacing = 1.0f;
        _lbDialog.lineBreakMode = RTTextLineBreakModeWordWrapping;
        _lbDialog.text = @"";
        _lbDialog.hidden = !(_dialogType == TSDialogLeftShow || _dialogType == TSDialogRightShow);
        [self addSubview:_lbDialog];
        
        _tfDialog = [[UITextField alloc]initWithFrame:CGRectMake(TSDialogDefWidth-(TSDCLeftGap+TSDCMaxWidth),TSDCHeightDif/2,TSDCMaxWidth,TSDialogDefHeight-TSDCHeightDif)];
        _tfDialog.backgroundColor = [UIColor clearColor];
        _tfDialog.font = [UIFont systemFontOfSize:14.0];
        _tfDialog.textColor = [UIColor colorWithHex:0x999999];
        _tfDialog.borderStyle = UITextBorderStyleNone;
        _tfDialog.adjustsFontSizeToFitWidth = YES;
        _tfDialog.minimumFontSize = 10;
        _tfDialog.placeholder = @"";
        _tfDialog.contentVerticalAlignment = UIControlContentHorizontalAlignmentCenter;
        _tfDialog.hidden = !(_dialogType == TSDialogLeftInput || _dialogType == TSDialogRightInput);
        [_tfDialog addTarget:self action:@selector(dialogDidEndOnExit:) forControlEvents:UIControlEventEditingDidEndOnExit];
        [_tfDialog addTarget:self action:@selector(dialogEditingDidBegin:) forControlEvents:UIControlEventEditingDidBegin];
        [_tfDialog addTarget:self action:@selector(dialogEditingDidEnd:) forControlEvents:UIControlEventEditingDidEnd];
        [self addSubview:_tfDialog];
    }
    
    [self setDialogText:_dialogText];
    [self setPortraitImage:_portraitImage];
    [self setPortraitURL:_portraitURL];
}

#pragma mark - Adjust Self Frame
//############################################################################
//############################################################################
-(void)adjustFrameWithCurrent
{
    [self adjustFrameWithType:_dialogType DialogText:_dialogText];
}

-(void)adjustFrameWithMinSize
{
    if(_dialogType == TSDialogLeftShow || _dialogType == TSDialogLeftInput)
    {
        [self adjustFrameWithType:TSDialogLeftShow DialogText:@"a"];
    }
    else if(_dialogType == TSDialogRightShow || _dialogType == TSDialogRightInput)
    {
        [self adjustFrameWithType:TSDialogRightShow DialogText:@"a"];
    }
    else
    {
        
    }
}

-(void)adjustFrameWithType:(TSDialogType)pType DialogText:(NSString*)pText
{
    BOOL isShow = (pType == TSDialogLeftShow || pType == TSDialogRightShow);
    RTLabel *tLabel = [[[RTLabel alloc] initWithFrame:CGRectMake(0,0,TSDCMaxWidth,30)]autorelease];
    tLabel.font=_lbDialog.font;
    tLabel.lineSpacing = _lbDialog.lineSpacing;
    tLabel.lineBreakMode = _lbDialog.lineBreakMode;
    tLabel.text = pText;
    float tDCWidth = isShow?MIN(tLabel.optimumSize.width,TSDCMaxWidth):TSDCMaxWidth;
    float tDCHeight = TSDialogDefHeight-TSDCHeightDif;
    tDCHeight = isShow?MAX(tLabel.optimumSize.height+3,tDCHeight):tDCHeight;
    float tSBWidth = MAX(tDCWidth+TSDCWidthDif,TSSBMinWidth);
    float tSBHeight = tDCHeight+TSDCHeightDif;
    float tDialogWidth = tSBWidth+TSSBWidthDif;
    float tDialogHeight = tSBHeight+TSSBHeightDif;
    
    if(pType == TSDialogLeftInput || pType == TSDialogLeftShow)//左
    {
        CGRect tFrame = self.frame;
        tFrame.size.width = tDialogWidth;
        tFrame.size.height = tDialogHeight;
        self.frame = tFrame;
        
        _viPortrait.frame = CGRectMake(TSPortraitLG,tDialogHeight-(TSPortraitWH+TSPortraitBG),TSPortraitWH,TSPortraitWH);
    
        _imgViSpeechBubble.frame = CGRectMake(TSSBLeftGap,0,tSBWidth,tSBHeight);
        _imgViSpeechBubble.image = [[UIImage imageNamed:@"TS_Cus_DialogLeft_BKG.png"]resizableImageWithCapInsets:UIEdgeInsetsMake(19,49,41,21)];
        
        _lbDialog.frame = CGRectMake(TSDCLeftGap,TSDCHeightDif/2,tDCWidth,tDCHeight);
        _lbDialog.hidden = !(_dialogType == TSDialogLeftShow || _dialogType == TSDialogRightShow);
        
        _tfDialog.frame = CGRectMake(TSDCLeftGap,TSDCHeightDif/2,tDCWidth,tDCHeight);
        _tfDialog.hidden = !(_dialogType == TSDialogLeftInput || _dialogType == TSDialogRightInput);
    }
    else//右
    {
        CGRect tFrame = self.frame;
        tFrame.size.width = tDialogWidth;
        tFrame.size.height = tDialogHeight;
        self.frame = tFrame;
        
        _viPortrait.frame = CGRectMake(tDialogWidth-(TSPortraitWH+TSPortraitRG),tDialogHeight-(TSPortraitWH+TSPortraitBG),TSPortraitWH,TSPortraitWH);
        
        _imgViSpeechBubble.frame = CGRectMake(tDialogWidth-(tSBWidth+TSSBRightGap),0,tSBWidth,tSBHeight);
        _imgViSpeechBubble.image = [[UIImage imageNamed:@"TS_Cus_DialogRight_BKG.png"]resizableImageWithCapInsets:UIEdgeInsetsMake(21,21,41,49)];
        
//        float tx = tDialogWidth-(TSDCRightGap+tDCWidth);
        _lbDialog.frame = CGRectMake(TSDCLeftGapRightType,TSDCHeightDif/2,tDCWidth,tDCHeight);
        _lbDialog.hidden = !(_dialogType == TSDialogLeftShow || _dialogType == TSDialogRightShow);
        
        _tfDialog.frame = CGRectMake(TSDCLeftGapRightType,TSDCHeightDif/2,tDCWidth,tDCHeight);
        _tfDialog.hidden = !(_dialogType == TSDialogLeftInput || _dialogType == TSDialogRightInput);
    }
    self.dialogText = _dialogText;
}


#pragma mark - Optimum Size Method
//############################################################################
//############################################################################
-(CGSize)optimumSize
{
    return [self optimumSizeWithType:_dialogType DialogText:_dialogText];
}

-(CGSize)optimumSizeWithType:(TSDialogType)pType DialogText:(NSString*)pText
{
    if(pType == TSDialogLeftShow || pType == TSDialogRightShow)
    {
        RTLabel *tLabel = [[[RTLabel alloc] initWithFrame:CGRectMake(0,0,TSDCMaxWidth,30)]autorelease];
        tLabel.font=_lbDialog.font;
        tLabel.lineSpacing = _lbDialog.lineSpacing;
        tLabel.lineBreakMode = _lbDialog.lineBreakMode;
        tLabel.text = pText;
        float tDCWidth = MIN(tLabel.optimumSize.width,TSDCMaxWidth);
        float tDCHeight = MAX(tLabel.optimumSize.height,TSDialogDefHeight-TSDCHeightDif);
        float tSBWidth = MAX(tDCWidth+TSDCWidthDif,TSSBMinWidth);
        float tSBHeight = tDCHeight+TSDCHeightDif;
        float tDialogWidth = tSBWidth+TSSBWidthDif;
        float tDialogHeight = tSBHeight+TSSBHeightDif;
        return CGSizeMake(tDialogWidth,tDialogHeight);
    }
    else
    {
        return CGSizeMake(TSDialogDefWidth,TSDialogDefHeight);
    }
}

#pragma mark - Text And Image Method
//############################################################################
//############################################################################
-(void)setDialogText:(NSString *)dialogText
{
    NSString * temp = [dialogText retain];
    [_dialogText release];
    _dialogText = temp;
    _lbDialog.text = _dialogText;
    _tfDialog.placeholder = _dialogText;
}

-(void)setPortraitImage:(UIImage *)portraitImage
{
    UIImage *temp = [portraitImage retain];
    [_portraitImage release];
    _portraitImage = temp;
    _imgViPortrait.image = _portraitImage;
}

-(void)setPortraitURL:(NSString *)portraitURL
{
    NSString * temp = [portraitURL retain];
    [_portraitURL release];
    _portraitURL = temp;
    if(_imgViPortrait != nil)
    {
        UIImage *tImage = (_portraitImage != nil)?_portraitImage:[UIImage imageNamed:@""];
        [_imgViPortrait setImageWithURL:[NSURL URLWithString:portraitURL] placeholderImage:tImage success:^(UIImage *image, BOOL cached)
        {
        }failure:^(NSError *error)
        {
        }];
    }
}

-(void)setTextColor:(UIColor *)pColor
{
    if(pColor == nil) return;
    _lbDialog.textColor = pColor;
    _tfDialog.textColor = pColor;
}

-(void)setTextFont:(UIFont *)pFont
{
    if(pFont == nil) return;
    _lbDialog.font = pFont;
    _tfDialog.font = pFont;
}

-(void)setTextAlignment:(CTTextAlignment)pAlignment
{
    _lbDialog.textAlignment = pAlignment;
    _tfDialog.textAlignment = pAlignment;
}


#pragma mark - Dialog Input Method
//############################################################################
//############################################################################
-(void)dialogDidEndOnExit:(id)sender
{
    if(_delegate != nil && [_delegate respondsToSelector:@selector(dialogDidEndOnExit:Text:)])
    {
        [_delegate dialogDidEndOnExit:self Text:_tfDialog.text];
    }
}

-(void)dialogEditingDidBegin:(id)sender
{
    if(_delegate != nil && [_delegate respondsToSelector:@selector(dialogEditingDidBegin:Text:)])
    {
        [_delegate dialogEditingDidBegin:self Text:_tfDialog.text];
    }
}

-(void)dialogEditingDidEnd:(id)sender
{
    if(_delegate != nil && [_delegate respondsToSelector:@selector(dialogEditingDidEnd:Text:)])
    {
        [_delegate dialogEditingDidEnd:self Text:_tfDialog.text];
    }
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end































