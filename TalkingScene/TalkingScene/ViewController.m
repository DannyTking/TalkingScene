//
//  ViewController.m
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

-(void)dealloc
{
    self.btnRecord      = nil;
    self.btnPlay        = nil;
    self.recorder       = nil;
    self.audioPlayer    = nil;
    [super dealloc];
    
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	
    NSURL *sourcePath = [NSURL fileURLWithPath:[NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat: @"%@.%@",@"test",@"caf"]]];
    NSDictionary *dicModify = [[[NSDictionary alloc] initWithObjectsAndKeys:
                                [NSNumber numberWithFloat: 44100.0],AVSampleRateKey,
                                [NSNumber numberWithInt: kAudioFormatLinearPCM],AVFormatIDKey,
                                [NSNumber numberWithInt:16],AVLinearPCMBitDepthKey,
                                [NSNumber numberWithInt: 2], AVNumberOfChannelsKey,
                                [NSNumber numberWithBool:NO],AVLinearPCMIsBigEndianKey,
                                [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey,nil]autorelease];
    
    
    _recorder = [[AVAudioRecorder alloc] initWithURL:sourcePath settings:dicModify error:nil];
    _recorder.delegate = self;
    _audioPlayer=[[AVAudioPlayer alloc]initWithContentsOfURL:sourcePath error:nil];
    _audioPlayer.delegate = self;
    _audioPlayer.volume=1.0;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

-(IBAction)btnRecordStart:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    [_btnPlay setEnabled:NO];
    
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    [audioSession setActive:YES error:nil];
    [_recorder prepareToRecord];
    [_recorder record];
}

-(IBAction)btnRecordEnd:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    
    [self performSelector:@selector(stopRecordMy) withObject:nil afterDelay:0.5];
}

-(void)stopRecordMy
{
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
    [audioSession setActive:NO error:nil];
    [_recorder pause];
    [_recorder stop];
    [_btnPlay setEnabled:YES];
}

-(IBAction)tapBtnPlay:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:NO];
    
    NSLog(@"%@",[NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat: @"%@.%@",@"test",@"caf"]]);
                 NSLog(@"%@",_audioPlayer.url.absoluteString);
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if([fileManager fileExistsAtPath:[NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat: @"%@.%@",@"test",@"caf"]]])
    {
        [_audioPlayer prepareToPlay];
        [_audioPlayer play];
    }
    else
    {
        [_btnRecord setEnabled:YES];
    }
}

#pragma mark - AVAudioRecorderDelegate
//#############################################################################
//#############################################################################

- (void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError *)error
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

#pragma mark - AVAudioPlayerDelegate
//#############################################################################
//#############################################################################
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:YES];
}

- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player withOptions:(NSUInteger)flags
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:YES];
}


@end































