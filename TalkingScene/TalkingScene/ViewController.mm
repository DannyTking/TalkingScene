//
//  ViewController.m
//  TalkingScene
//
//  Created by liwenjian on 16-10-1.
//  Copyright (c) 2016年 magus. All rights reserved.
//

#import "ViewController.h"
#import "DatabaseManager.h"
#import "DBTableConstants.h"

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
    DELog(@"%@",NSStringFromSelector(_cmd));
    [_btnPlay setEnabled:NO];
    
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    [audioSession setActive:YES error:nil];
    [_recorder prepareToRecord];
    [_recorder record];
}

-(IBAction)btnRecordEnd:(id)sender
{
    DELog(@"%@",NSStringFromSelector(_cmd));
    
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
    DELog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:NO];
    
    DELog(@"%@",[NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat: @"%@.%@",@"test",@"caf"]]);
                 DELog(@"%@",_audioPlayer.url.absoluteString);
    
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
    DELog(@"%@",NSStringFromSelector(_cmd));
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError *)error
{
    DELog(@"%@",NSStringFromSelector(_cmd));
}

#pragma mark - AVAudioPlayerDelegate
//#############################################################################
//#############################################################################
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    DELog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:YES];
}

- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player withOptions:(NSUInteger)flags
{
    DELog(@"%@",NSStringFromSelector(_cmd));
    [_btnRecord setEnabled:YES];
}


#pragma mark Database Encrypt

//debug. encrypt the content db.Don't call this,because the .db is already encrypted
-(void) keyBundleDB:(NSString*)userDatabase
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *encryptName = [documentsDirectory stringByAppendingPathComponent:@"encrypt.db"];
    
    //read db
    sqlite3 *db;
    
    int code = sqlite3_open([userDatabase UTF8String], &db);
    if (code == SQLITE_OK) {
        
        NSString *key = @"chase";
        
        NSString *sqlString = [NSString stringWithFormat:@"ATTACH DATABASE '%@' AS encrypted KEY '%@';",encryptName,key];
        int ret = sqlite3_exec(db, [sqlString UTF8String] , NULL, NULL, NULL);
        
        ret = sqlite3_exec(db, "SELECT sqlcipher_export('encrypted');" , NULL, NULL, NULL);
        ret = sqlite3_exec(db, "DETACH DATABASE encrypted;" , NULL, NULL, NULL);
        
        //test decrypt
        /*NSString *decryptName = [documentsDirectory stringByAppendingPathComponent:@"decryptName.db"];
         sqlString = [NSString stringWithFormat:@"ATTACH DATABASE '%@' AS plaintext KEY '';",decryptName];
         ret = sqlite3_exec(db, [sqlString UTF8String] , NULL, NULL, NULL);
         
         ret = sqlite3_exec(db, "SELECT sqlcipher_export('plaintext');" , NULL, NULL, NULL);
         ret = sqlite3_exec(db, "DETACH DATABASE plaintext;" , NULL, NULL, NULL);
         */
        sqlite3_close(db);
        
    }
}

//rekey the .db in ./Content
-(void) rekeyBundleDB:(NSString*)userDatabase
{
    
#ifndef NO_DATA_ENCRYTP
    
    //rekey the content db
    //read db
    sqlite3 *db;
    
    int code = sqlite3_open([userDatabase UTF8String], &db);
    if (code == SQLITE_OK)
    {
        int ret;
//        //vertify original key
//        const char* key = [self getBundleSqliteKey];
//        ret = sqlite3_key(db, key, strlen(key));
//        
//        
//        const char* newKey = [self getSqliteKey];
//        ret = sqlite3_trace(db, newKey, strlen(newKey));
        
        sqlite3_close(db);
    }
#endif
}

-(void) vertifyBundleSqlite:(int) databaseKey
{
#ifndef NO_DATA_ENCRYTP
    const char* key = [self getBundleSqliteKey];
    sqlite3 *db = Walaber::DatabaseManager::getDatabase(databaseKey);
    
//    if (db) {
//        int ret = sqlite3_key(db, key, strlen(key));
//        printf("vertify buidle db %d",ret);
//    }
#endif
}

-(void) vertifySqliteKey:(int) databaseKey
{
#ifndef NO_DATA_ENCRYTP
    const char* key = [self getSqliteKey];
    sqlite3 *db = Walaber::DatabaseManager::getDatabase(databaseKey);
    
    if (db) {
//        int ret = sqlite3_key(db, key, strlen(key));
        printf("vertify user db %d",databaseKey);
    }
#endif
}

-(const char*) getSqliteKey
{
    NSString * macAddress = @"mac";
    NSString * code = [NSString stringWithFormat:@"%@V01YWVk=",macAddress];
    //TODO:create key
    return [code UTF8String];
}

-(const char*) getBundleSqliteKey
{
    return [@"d2hlcmUgaXMgbXkgeHl5" UTF8String];
}



@end































