//
//  TSSceneSelectViewController.m
//  TalkingScene
//
//  Created by liwenjian on 13-10-8.
//  Copyright (c) 2013年 magus. All rights reserved.
//

#import "TSSceneSelectViewController.h"
#import "TSRoleSelectViewController.h"
#include "DBTableConstants.h"
#include "DatabaseManager.h"

@interface TSSceneSelectViewController ()

@end

@implementation TSSceneSelectViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        NSString* dbBase = @"Content/Data/talkingScene.db";
#ifdef _LITE
        dbBase = @"talkingScene-Lite.db";
#endif
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString* userDatabase = [documentsDirectory stringByAppendingPathComponent:dbBase];
        NSLog(@"userDatabase: %@", userDatabase);
        // first, check if there is a database at all in the user store path.
        NSFileManager* fmgr = [NSFileManager defaultManager];
        
        if ([fmgr fileExistsAtPath:userDatabase])
        {
            // Open the user's DB
            if (Walaber::DatabaseManager::openDatabase( std::string( [userDatabase UTF8String] ), TB::DI_Default ))
            {
                Walaber::DatabaseManager::closeDatabase(TB::DI_Default);
            }
        }

        // Custom initialization
        std::string select = TB::ContentTableFieldNames[TB::CT_ContentID];
        std::string where = TB::ContentTableFieldNames[TB::CT_SequenceID] + std::string("='") + std::string("*'");
        
        Walaber::DatabaseIterator dit( TB::DI_Default, select, "ContentTable", where );
        
        while(dit.next())
        {
            // Get the values from the DB
            int id = dit.getIntAtIndex(0);
            std::printf("id is %d",id);
            // Now unlock that first level!
            //        std::string setU = PC::LevelInfoFieldNames[PC::LI_Unlocked] + std::string("=1");
            //        std::string whereU = PC::LevelInfoFieldNames[PC::LI_ID] + std::string("=") + StringHelper::intToStr(id);
            //
            //        DatabaseManager::updateEntry(PC::DI_Default, "LevelInfo", setU, whereU);
        }

    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    std::string select = TB::ContentTableFieldNames[TB::CT_ContentID];
    std::string where = TB::ContentTableFieldNames[TB::CT_SequenceID] + std::string("='") + std::string("*'");
    
    Walaber::DatabaseIterator dit( TB::DI_Default, select, "ContentTable", where );
    
    while(dit.next())
    {
        // Get the values from the DB
        int id = dit.getIntAtIndex(0);
        std::printf("id is %d",id);
        // Now unlock that first level!
//        std::string setU = PC::LevelInfoFieldNames[PC::LI_Unlocked] + std::string("=1");
//        std::string whereU = PC::LevelInfoFieldNames[PC::LI_ID] + std::string("=") + StringHelper::intToStr(id);
//        
//        DatabaseManager::updateEntry(PC::DI_Default, "LevelInfo", setU, whereU);
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)tapBtn:(id)sender
{
    TSRoleSelectViewController *role = [[[TSRoleSelectViewController alloc] init]autorelease];
    [self tsPushViewController:role];
}

@end
