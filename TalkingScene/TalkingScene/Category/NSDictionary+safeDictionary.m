//
//  NSDictionary+safeDictionary.m
//  JSJJ_iPhone
//
//  Created by sky on 13-5-13.
//  Copyright (c) 2013年 张自玉. All rights reserved.
//

#import "NSDictionary+safeDictionary.h"

NSString * const kEmptyString = @"";

@implementation NSDictionary (NSDictionary_Utils)


// zhengyue: return an empty string if the value is null or not a string.
- (NSString *)stringForKey:(id)key
{
    NSString *result = [self objectForKey:key];
    if([result isKindOfClass:[NSString class]])
    {
        return result;
    }
    if ([result isKindOfClass:[NSNumber class]]) {
        return [NSString stringWithFormat:@"%@",result];
    }
    else {
        return kEmptyString;
    }
}

// zhengyue: return nil if the object is null or not a NSDictionary.
- (NSDictionary *)dictionaryForKey:(id)key
{
    NSDictionary *result = [self objectForKey:key];
    if([result isKindOfClass:[NSDictionary class]])
    {
        return result;
    }
    else {
        return nil;
    }
}

// zhengyue: return nil if the object is null or not a NSArray.
- (NSArray *)arrayForKey:(id)key
{
    NSArray *result = [self objectForKey:key];
    if([result isKindOfClass:[NSArray class]])
    {
        return result;
    }
    else {
        return nil;
    }
}

//根据http请求，得到字典
+(NSDictionary *) dictionaryWithData:(NSData *) data{
    NSError *resDictionaryError = nil;
	NSDictionary *resDictionary = [[CJSONDeserializer deserializer] deserialize:data error:&resDictionaryError];
	if (resDictionaryError) {
		return nil;
	}else {
        if ([resDictionary isKindOfClass:[NSDictionary class]]) {
            return resDictionary;
        }
		return nil;
	}
}
@end
