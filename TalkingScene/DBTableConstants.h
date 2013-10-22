//
//  DBTableConstants.h
//  TalkingScene
//
//  Created by Tang YangMing on 10/21/13.
//  Copyright (c) 2013 magus. All rights reserved.
//

#ifndef DBTableConstants_h
#define DBTableConstants_h

#include "StringHelper.h"
namespace TB
{
    //------------------------------------------------------------------------------------------
    enum DatabaseIndex
    {
        DI_Default = 1000
	};
	
	
    //------------------------------------------------------------------------------------------
    enum ContentTableField
    {
        CT_ContentID,
        CT_SequenceID,
        CT_HeroID,
        CT_ContentText,
        CT_ContentVoice,
        CT_IsOrigin
    };
    
    const std::string ContentTableFieldNames[] =
    {
        std::string("ContentID"),
        std::string("SequenceID"),
        std::string("HeroID"),
        std::string("ContentText"),
        std::string("ContentVoice"),
        std::string("IsOrigin")
    };
    
    //------------------------------------------------------------------------------------------
    enum HeroTableField
    {
        HT_HeroID,
        HT_HeroName,
        HT_HeroIcon,
        HT_HeroBrief,
        HT_HeroStars
    };
    
    const std::string HeroTableFieldNames[] =
    {
        std::string("HeroID"),
        std::string("HeroName"),
        std::string("HeroIcon"),
        std::string("HeroBrief"),
        std::string("HeroStars")
    };
    
    //------------------------------------------------------------------------------------------
    enum SceneTableField
    {
        ST_SceneID,
        ST_SceneName,
        ST_SceneIcon,
        ST_SceneBrief,
        ST_SceneStars
    };
    
    const std::string SceneTableFieldNames[] =
    {
        std::string("SceneID"),
        std::string("SceneName"),
        std::string("SceneIcon"),
        std::string("SceneBrief"),
        std::string("SceneStars")
    };
    
    //------------------------------------------------------------------------------------------
    enum StoryTableField
    {
        ST_StoryID,
        ST_StoryName,
        ST_StroyIcon,
        ST_StoryBrief,
        ST_StoryAuthor,
        ST_StoryStars
    };
    
    const std::string StoryTableFieldNames[] =
    {
        std::string("StoryID"),
        std::string("StoryName"),
        std::string("StroyIcon"),
        std::string("StoryBrief"),
        std::string("StoryAuthor"),
        std::string("StoryStars")
    };
    
    
    //------------------------------------------------------------------------------------------
    enum StoryFSceneField
    {
        SFS_StoryID,
        SFS_SceneID
    };
    
    const std::string StoryFSceneFieldNames[] =
    {
        std::string("StoryID"),
        std::string("SceneID")
    };
    
    //------------------------------------------------------------------------------------------
    enum SceneFContentField
    {
        SFC_StoryID,
        SFC_SceneID
    };
    
    const std::string SceneFContentFieldNames[] =
    {
        std::string("SceneID"),
        std::string("ContentID")
    };

    //------------------------------------------------------------------------------------------
    enum UserPlayTableField
    {
        UserID,
        PlayDate,
        StroyID,
        SceneID,
        HeroID
    };
    
    const std::string UserPlayTableFieldNames[] =
    {
        std::string("UserID"),
        std::string("PlayDate"),
        std::string("StroyID"),
        std::string("SceneID"),
        std::string("HeroID")
    };


}


#endif
