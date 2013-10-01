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
    self.btnRecordStart = nil;
    self.btnRecordStop  = nil;
    self.btnPlay        = nil;
    self.sourcePath     = nil;
    self.recorder       = nil;
    self.audioPlayer    = nil;
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)tapBtnStart:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

-(IBAction)tapBtnStop:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

-(IBAction)tapBtnPlay:(id)sender
{
    NSLog(@"%@",NSStringFromSelector(_cmd));
}

@end































