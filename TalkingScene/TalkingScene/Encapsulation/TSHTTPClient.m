//
//  TSHTTPClient.m
//  TalkingScene
//
//  Created by liwenjian on 13-11-6.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSHTTPClient.h"
#import "Reachability.h"
#import "AFJSONRequestOperation.h"
#import "NSDictionary+safeDictionary.h"

NSString *const TSDefaultHost=@"";
NSString *const TSDefaultPath=@"";
const NSInteger  responseSuccessCode = 0;
const NSString  *responseMessageKey  = @"errmsg";
const NSString  *responseCodeKey = @"result";
const NSString  *responseStatusKey = @"status";
const NSInteger timeoutInterval = 60;

@implementation TSHTTPClient

-(void)dealloc
{
    [self cancelAllHTTPOperationsWithMethod:nil path:self.pathURL];
    self.pathURL = nil;
    [super dealloc];
}

#pragma mark - Constructor Method
//#######################################################################
//#######################################################################
+(instancetype)sharedDefClient;
{
    static TSHTTPClient *netClient = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        netClient = [[TSHTTPClient alloc] initWithBaseURL:[NSURL URLWithString:TSDefaultHost]];
        netClient.pathURL = [NSString stringWithFormat:@"%@",TSDefaultPath];
    });
    return netClient;
}

+(instancetype)clientWithBaseURL:(NSURL*)url Path:(NSString*)pPath
{
    return [[[self alloc] initWithBaseURL:url Paht:pPath]autorelease];
}

- (id)initWithBaseURL:( NSURL *)url
{
    self = [super initWithBaseURL:url];
    if (!self)
    {
        return nil;
    }
    [self registerHTTPOperationClass:[AFJSONRequestOperation class]];
    return self;
}

-(id)initWithBaseURL:(NSURL*)url Paht:(NSString*)pPaht
{
    self = [super initWithBaseURL:url];
    if(self)
    {
        self.pathURL = pPaht;
    }
    return self;
}

#pragma mark - Check Connect Method
//#######################################################################
//#######################################################################
+(BOOL)checkConnect
{
    return [[self sharedDefClient] checkConnect];
}

-(BOOL)checkConnect
{
#ifdef _SYSTEMCONFIGURATION_H
    return (self.networkReachabilityStatus != AFNetworkReachabilityStatusNotReachable);
#else
    Reachability *rc1 = [Reachability reachabilityForInternetConnection];
    Reachability *rc2 = [Reachability reachabilityForLocalWiFi];
    BOOL bReachableViaWWAN = ([rc1 isReachableViaWWAN] || [rc2 isReachableViaWWAN]);
    BOOL bReachableViaWiFi = ([rc1 isReachableViaWiFi] || [rc2 isReachableViaWiFi]);
    BOOL bReachable = (bReachableViaWWAN || bReachableViaWiFi);
    if (bReachable)
    {
        return YES;
    }
    return NO;
#endif
}

#pragma mark - Request Method
//#######################################################################
//#######################################################################
-(void)postParameters:(NSDictionary *)parameters success:(TSSuccessBlock)success failure:(TSFailureBlock)failure
{
    NSMutableDictionary *params = [self combineParams:parameters];
    [self generateParameterURL:params];//把参数给打出来，方便在浏览器上看返回
    
    NSMutableURLRequest *request = [self requestWithMethod:@"POST" path:self.pathURL parameters:params];
    request.timeoutInterval = timeoutInterval;
    AFHTTPRequestOperation *operation = [self HTTPRequestOperationWithRequest:request success:^(AFHTTPRequestOperation *operation, id responseObject)
                                         {
                                             NSDictionary * tDic = [self parseDateToDic:responseObject];
                                             if (tDic != nil)
                                             {
                                                 NSInteger tErrorCode = [[tDic objectForKey:responseCodeKey] intValue];
                                                 NSString *tErrorMsg = [tDic objectForKey:responseMessageKey];
                                                 success(tErrorMsg,tErrorCode,tDic);
                                             }else
                                             {
                                                 success(@"获取数据失败，请稍后再试！",1,nil);
                                             }
                                         } failure:^(AFHTTPRequestOperation *operation, NSError *error)
                                         {
                                             NSString *tErrorMsg = [NSString stringWithFormat:@"网络不稳定，请稍后再试！"];
                                             failure(tErrorMsg,400);
                                         }];
    [self enqueueHTTPRequestOperation:operation];
}

-(void)postPath:(NSString*)path Parameters:(NSDictionary *)parameters success:(TSSuccessBlock)success failure:(TSFailureBlock)failure
{
    self.pathURL = path;
    [self postParameters:parameters success:success failure:failure];
}


#pragma mark - Self Method
//#######################################################################
//#######################################################################
//将参数与基本参数拼接
-(NSMutableDictionary *)combineParams:(NSDictionary *)pParams
{
    NSMutableDictionary * combParams = [NSMutableDictionary dictionaryWithDictionary:[self baseParams]];
    for(NSString *key in [pParams allKeys])
    {
        [combParams setValue:[pParams objectForKey:key] forKey:key];
    }
    return combParams;
}

//生成基本参数
-(NSDictionary*)baseParams
{
    NSDictionary * baseParams = @{ @"agent" : @"agent" ,
                                   @"udid" : @"udid",
                                   @"token": @"token",
                                   @"appversion":@"appversion",
                                   @"osversion":@"osversion",
                                   @"appid":@"appid",
                                   @"publishChannel":@"publishChannel"
                                   };
    return baseParams;
    
    //    NSString * udid=nil;
    //    udid= [[UIDevice currentDevice] uniqueDeviceIdentifier];
    //    JSFundUserInfo * userInfo =[JSFundUserInfo sharedUserInfo];
    //    NSString * token =userInfo.token;
    //    NSDictionary * baseParams = @{ @"agent" : DEVICE_NAME ,
    //                                   @"udid" : udid,
    //                                   @"token": token,
    //                                   @"appversion":APP_VERSION,
    //                                   @"osversion":SYSTEM_VERSION,
    //                                   @"appid":APP_ID,
    //                                   @"publishChannel":CHANNEL_NAME
    //                                   };
    //    return baseParams;
}

//生成带参URL，方便在浏览器上看返回
-(NSString *)generateParameterURL:(NSDictionary *)pParam
{
    NSArray * kesArray = [pParam allKeys];
    NSString * paraStr =@"?";
    for (NSString * astr in kesArray)
    {
        NSString * value =[pParam objectForKey:astr];
        paraStr=[paraStr stringByAppendingFormat:@"%@=%@&",astr,value];
    }
    DELog(@"finaUrl :%@%@%@",self.baseURL, self.pathURL,paraStr);
    return paraStr;
}

//解析数据
-(NSString*)parseDateToStr:(id)responseData
{
    NSStringEncoding gbkEncoding =CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
    NSString*responseStr = [[[NSString alloc] initWithData:responseData encoding:gbkEncoding]autorelease];
    return responseStr;
}

-(NSDictionary*)parseDateToDic:(id)responseData
{
    NSStringEncoding gbkEncoding =CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
    NSString*responseStr = [[[NSString alloc] initWithData:responseData encoding:gbkEncoding]autorelease];
    NSDictionary *dict = [NSDictionary dictionaryWithData:[responseStr dataUsingEncoding:NSUTF8StringEncoding]];
    return dict;
}


@end
