//
//  ViewController.h
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "TSBaseViewController.h"

@interface ViewController : TSBaseViewController<AVAudioRecorderDelegate,AVAudioPlayerDelegate>

@property(nonatomic,retain) IBOutlet UIButton *btnRecord;
@property(nonatomic,retain) IBOutlet UIButton *btnPlay;
@property(nonatomic,retain) AVAudioRecorder *recorder;
@property(nonatomic,retain) AVAudioPlayer *audioPlayer;

-(IBAction)btnRecordStart:(id)sender;
-(IBAction)btnRecordEnd:(id)sender;
-(IBAction)tapBtnPlay:(id)sender;


#pragma mark Database Encrypt
//chase 2013-05-28
//rekey the .db in ./Content
-(void) rekeyBundleDB:(NSString*)userDatabase;
-(void) vertifyBundleSqlite:(int) databaseKey;
-(void) vertifySqliteKey:(int) databaseKey;
-(const char*) getSqliteKey;
-(const char*) getBundleSqliteKey;

@end
