//
//  TSHTTPClient.h
//  TalkingScene
//
//  Created by liwenjian on 13-11-6.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "AFHTTPClient.h"

typedef void (^TSSuccessBlock) (NSString *errorMsg,NSUInteger errorCode,NSDictionary *responseObject);
typedef void (^TSFailureBlock) (NSString *errorMsg,NSUInteger errorCode);

@interface TSHTTPClient : AFHTTPClient
@property (nonatomic,retain) NSString *pathURL;

+(instancetype)sharedDefClient;
+(instancetype)clientWithBaseURL:(NSURL*)url Path:(NSString*)pPath;
-(id)initWithBaseURL:(NSURL*)url Paht:(NSString*)pPaht;


+(BOOL)checkConnect;
-(BOOL)checkConnect;
-(void)postParameters:(NSDictionary *)parameters success:(TSSuccessBlock)success failure:(TSFailureBlock)failure;
-(void)postPath:(NSString*)path Parameters:(NSDictionary *)parameters success:(TSSuccessBlock)success failure:(TSFailureBlock)failure;

@end
