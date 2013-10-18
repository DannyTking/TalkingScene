//
//  TSGlobalMacros.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#ifndef TalkingScene_TSGlobalMacros_h
#define TalkingScene_TSGlobalMacros_h

//判断是否为长屏幕（就是iPhone5）
#define IS_IPHONE5 ( fabs( ( double )[ [ UIScreen mainScreen ] bounds ].size.height - ( double )568 ) < DBL_EPSILON )

//日志开关  用于调试
#define DELog_Switch
#ifdef DELog_Switch
#   define DELog(fmt, ...) NSLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#   define DELog(...)
#endif

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v) ([[[UIDevice currentDevice] systemVersion] floatValue] >= v)

#endif


