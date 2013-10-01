//
//  ViewController.h
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@interface ViewController : UIViewController

@property(nonatomic,retain) IBOutlet UIButton *btnRecordStart;
@property(nonatomic,retain) IBOutlet UIButton *btnRecordStop;
@property(nonatomic,retain) IBOutlet UIButton *btnPlay;
@property(nonatomic,retain) NSURL *sourcePath;
@property(nonatomic,retain) AVAudioRecorder *recorder;
@property(nonatomic,retain) AVAudioPlayer *audioPlayer;

-(IBAction)tapBtnStart:(id)sender;
-(IBAction)tapBtnStop:(id)sender;
-(IBAction)tapBtnPlay:(id)sender;


@end
