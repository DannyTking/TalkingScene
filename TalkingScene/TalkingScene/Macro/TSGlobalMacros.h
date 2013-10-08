//
//  TSGlobalMacros.h
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#ifndef TalkingScene_TSGlobalMacros_h
#define TalkingScene_TSGlobalMacros_h
#endif

//判断是否为长屏幕（就是iPhone5）
#define IS_WIDESCREEN ( fabs( ( double )[ [ UIScreen mainScreen ] bounds ].size.height - ( double )568 ) < DBL_EPSILON )
