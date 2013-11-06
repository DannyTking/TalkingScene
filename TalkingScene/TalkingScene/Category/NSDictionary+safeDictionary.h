//
//  NSDictionary+safeDictionary.h
//  JSJJ_iPhone
//
//  Created by sky on 13-5-13.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CJSONDeserializer.h"
extern NSString * const kEmptyString;

@interface NSDictionary (safeDictionary)

- (NSString *)stringForKey:(id)key;

// zhengyue: return nil if the object is NSNull or not a NSDictionary
- (NSDictionary *)dictionaryForKey:(id)key;

// zhengyue: return nil if the object is null or not a NSArray.
- (NSArray *)arrayForKey:(id)key;

//根据http请求，得到字典
+(NSDictionary *) dictionaryWithData:(NSData *) data;
@end
