//
//  TSSceneSelectViewController.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSSceneSelectViewController.h"
#import "TSRoleSelectViewController.h"

@interface TSSceneSelectViewController ()

@end

@implementation TSSceneSelectViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)tapBtn:(id)sender
{
    TSRoleSelectViewController *role = [[[TSRoleSelectViewController alloc] init]autorelease];
    [self.navigationController pushViewController:role animated:YES];
}

@end
