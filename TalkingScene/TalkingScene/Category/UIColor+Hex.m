//
//  UIColor+Hex.m
//  TalkingScene
//
//  Created by liwenjian on 13-11-6.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "UIColor+Hex.h"

@implementation UIColor (Hex)
+ (UIColor*)colorWithHex:(NSInteger)hexValue alpha:(CGFloat)alphaValue
{
    return [UIColor colorWithRed:((float)((hexValue & 0xFF0000) >> 16))/255.0
                           green:((float)((hexValue & 0xFF00) >> 8))/255.0
                            blue:((float)(hexValue & 0xFF))/255.0 alpha:alphaValue];
}

+ (UIColor*)colorWithHex:(NSInteger)hexValue
{
    return [UIColor colorWithHex:hexValue alpha:1.0];
}

+ (UIColor*)whiteColorWithAlpha:(CGFloat)alphaValue
{
    return [UIColor colorWithHex:0xffffff alpha:alphaValue];
}

+ (UIColor*)blackColorWithAlpha:(CGFloat)alphaValue
{
    return [UIColor colorWithHex:0x000000 alpha:alphaValue];
}


+(UIColor*)colorWithred:(NSInteger)red green:(NSInteger)green blue:(NSInteger)blue
{
	return  [UIColor colorWithRed:[[ NSNumber numberWithInt:red]floatValue]/255.0f green:[[ NSNumber numberWithInt:green]floatValue]/255.0f blue:[[ NSNumber numberWithInt:blue]floatValue]/255.0f alpha:1.f];
}

+(UIColor*)colorWithred:(NSInteger)red green:(NSInteger)green blue:(NSInteger)blue alpha:(float)alpha_
{
	return  [UIColor colorWithRed:[[ NSNumber numberWithInt:red]floatValue]/255.0f green:[[ NSNumber numberWithInt:green]floatValue]/255.0f blue:[[ NSNumber numberWithInt:blue]floatValue]/255.0f alpha:alpha_];
}
+ (UIColor *)colorWithHexString:(id)hexString
{
    @try {
        // Nil or non-string
        if (hexString == nil) @throw [NSException exceptionWithName:@"Nil string" reason:@"Invalid hex string" userInfo:nil];
        if (![hexString isKindOfClass:[NSString class]]) @throw [NSException exceptionWithName:@"Not a string" reason:@"Invalid hex string" userInfo:nil];
        
        // Filter
        hexString = [hexString stringByReplacingOccurrencesOfString:@" " withString:@""];
        hexString = [hexString uppercaseString];
        if ([[NSString stringWithFormat:@"%c",[hexString characterAtIndex:0]] isEqualToString: @"#"]) {
            hexString = [hexString stringByReplacingOccurrencesOfString:@"#" withString:@""];
        }
        
        // #ffffff
        if ([hexString length] == 6) {
            hexString = [NSString stringWithFormat:@"%@%@", hexString, @"FF"];
        }
        
        // #fff
        if ([hexString length] == 3) {
            hexString = [NSString stringWithFormat:@"%c%c%c%c%c%c%@",[hexString characterAtIndex:0], [hexString characterAtIndex:0], [hexString characterAtIndex:1], [hexString characterAtIndex:1], [hexString characterAtIndex:2], [hexString characterAtIndex:2], @"FF"];
        }
        
        // #ff
        if ([hexString length] == 2) {
            hexString = [NSString stringWithFormat:@"%c%c%c%c%c%c%@",[hexString characterAtIndex:0], [hexString characterAtIndex:1], [hexString characterAtIndex:0], [hexString characterAtIndex:1], [hexString characterAtIndex:0], [hexString characterAtIndex:1], @"FF"];
        }
        
        NSArray *hex = [[NSArray alloc] initWithObjects:@"0",@"1",@"2",@"3",@"4",@"5",@"6",@"7",@"8",@"9",@"A",@"B",@"C",@"D",@"E",@"F",nil];
        float r = ([hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:0]]]*16)+[hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:1]]];
        float g = ([hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:2]]]*16)+[hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:3]]];
        float b = ([hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:4]]]*16)+[hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:5]]];
        float a = ([hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:6]]]*16)+[hex indexOfObject:[NSString stringWithFormat:@"%c",[hexString characterAtIndex:7]]];
        
        return [UIColor colorWithRed:r/255.0f green:g/255.0f blue:b/255.0f alpha:a/255.0f];
    }
    @catch (NSException *exception) {
        return [UIColor colorWithRed:0.0f green:0.0f blue:0.0f alpha:1.0f];
    }
}
@end
