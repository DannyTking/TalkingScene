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
        
        // Custom initialization
        // 根据故事ID选择剧情 ymtang
        std::string subSelect = TB::StoryFSceneFieldNames[TB::SFS_SceneID];
        std::string subWhere = "";//TB::StoryFSceneFieldNames[TB::SFS_StoryID] + std::string("=") + Walaber::StringHelper::intToStr(storyid);
        Walaber::DatabaseIterator subDit(TB::DI_Default, subSelect, "StoryFScene", subWhere);
        while (subDit.next())
        {
            int sceneid = subDit.getIntAtIndex(0);
            std::string select =TB::SceneTableFieldNames[TB::ST_SceneName]+
                            ","+TB::SceneTableFieldNames[TB::ST_SceneBrief]+
                            ","+TB::SceneTableFieldNames[TB::ST_SceneIcon]+
                            ","+TB::SceneTableFieldNames[TB::ST_SceneStars];
            std::string where =TB::SceneTableFieldNames[TB::ST_SceneID]+std::string("=")+Walaber::StringHelper::intToStr(sceneid);
            Walaber::DatabaseIterator dit( TB::DI_Default, select, "SceneTable",where );
            
            if(dit.next())
            {
                // Get the values from the DB
                std::string sName = dit.getStringAtIndex(0);
                std::string sBrief = dit.getStringAtIndex(1);
                std::string sIcon = dit.getStringAtIndex(2);
                int sStars = dit.getIntAtIndex(3);
                std::printf("ST_SceneID id is %d, name is %s, brief is %s, icon is%s,stars is %d\t\n",sceneid,sName.c_str(),sBrief.c_str(),sIcon.c_str(),sStars);
            }
        }
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
    [self tsPushViewController:role];
}

@end
