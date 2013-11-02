//
//  TSStorySelectViewController.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSStorySelectViewController.h"
#import "TSSceneSelectViewController.h"
#include "DBTableConstants.h"
#include "DatabaseManager.h"

@interface TSStorySelectViewController ()

@end

@implementation TSStorySelectViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        NSString* dbBase = @"/Content/Data/talkingScene.db";
#ifdef _LITE
        dbBase = @"talkingScene-Lite.db";
#endif
        
        // first, check if there is a database at all in the user store path.
        NSFileManager* fmgr = [NSFileManager defaultManager];
        NSString* dbNPath = [[NSBundle mainBundle] bundlePath];
        dbNPath = [dbNPath stringByAppendingString:dbBase];
        std::string dbPath = [dbNPath UTF8String];
        if ([fmgr fileExistsAtPath:dbNPath])
        {
            // Open the user's DB
            Walaber::DatabaseManager::openDatabase( std::string( [dbNPath UTF8String] ), TB::DI_Default );
            std::string select = TB::StoryTableFieldNames[TB::ST_StoryID]+
                            ","+TB::StoryTableFieldNames[TB::ST_StoryName]+
                            ","+TB::StoryTableFieldNames[TB::ST_StoryBrief]+
                            ","+TB::StoryTableFieldNames[TB::ST_StroyIcon]+
                            ","+TB::StoryTableFieldNames[TB::ST_StoryAuthor]+
                            ","+TB::StoryTableFieldNames[TB::ST_StoryStars];;
            Walaber::DatabaseIterator dit( TB::DI_Default, select, "StoryTable" );
            
            while(dit.next())
            {
                // Get the values from the DB
                int storyID = dit.getIntAtIndex(0);
                std::string  storyName = dit.getStringAtIndex(1);
                std::string storyBrief = dit.getStringAtIndex(2);
                std::string storyIcon = dit.getStringAtIndex(3);
                std::string storyAuthor = dit.getStringAtIndex(4);
                int storyStars =  dit.getIntAtIndex(5);
                
                std::printf("ST_StoryID id is %d, name is %s, brief is %s, icon is%s,stars is %d\t\n",storyID,storyName.c_str(),storyBrief.c_str(),storyIcon.c_str(),storyStars);
            }

        }

    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.tsNavBar setTitle:@"选故事"];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)tapBtn:(id)sender
{
    TSSceneSelectViewController *scene = [[[TSSceneSelectViewController alloc] init]autorelease];
    [self tsPushViewController:scene];
}

@end

















