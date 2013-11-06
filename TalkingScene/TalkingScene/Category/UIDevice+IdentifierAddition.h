//
//  UIDevice+IdentifierAddition.h
//  JSJJ_iPhone
//
//  Created by sky on 13-5-12.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface UIDevice (IdentifierAddition)


- (NSString *) uniqueDeviceIdentifier;

- (NSString *) uniqueGlobalDeviceIdentifier;

@end
