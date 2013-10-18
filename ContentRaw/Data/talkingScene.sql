PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE "Achievements" ("Points" INTEGER NOT NULL , "ID" TEXT PRIMARY KEY  NOT NULL  UNIQUE , "Hidden" INTEGER NOT NULL  DEFAULT 0, "PreEarnedDescription" TEXT NOT NULL , "EarnedDescription" TEXT NOT NULL , "Image" TEXT NOT NULL , "PercentComplete" FLOAT NOT NULL  DEFAULT 0, "SortingGroup" INTEGER NOT NULL  DEFAULT 0, 'FacebookDescription' TEXT NOT NULL DEFAULT '', "LevelFile" TEXT NOT NULL  DEFAULT '', "HasViewed" BOOL NOT NULL  DEFAULT 0);
INSERT INTO "Achievements" VALUES(10,'ACH_LP_ONE_CHINA',0,'ACH_LP_ONE_HOWTO','ACH_LP_ONE_DESC','ACH_01.png',0.0,0,'ACH_LP_ONE_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_TRIGNOME_LP_ONE_CHINA',0,'ACH_TRIGNOME_LP_ONE_HOWTO','ACH_TRIGNOME_LP_ONE_DESC','ACH_02.png',0.0,0,'ACH_TRIGNOME_LP_ONE_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_LP_TWO_CHINA',0,'ACH_LP_TWO_HOWTO','ACH_LP_TWO_DESC','ACH_03.png',0.0,0,'ACH_LP_TWO_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_TRIGNOME_LP_TWO_CHINA',0,'ACH_TRIGNOME_LP_TWO_HOWTO','ACH_TRIGNOME_LP_TWO_DESC','ACH_04.png',0.0,0,'ACH_TRIGNOME_LP_TWO_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_LP_THREE_CHINA',0,'ACH_LP_THREE_HOWTO','ACH_LP_THREE_DESC','ACH_05.png',0.0,0,'ACH_LP_THREE_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_TRIGNOME_LP_THREE_CHINA',0,'ACH_TRIGNOME_LP_THREE_HOWTO','ACH_TRIGNOME_LP_THREE_DESC','ACH_06.png',0.0,0,'ACH_TRIGNOME_LP_THREE_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_LP_FOUR_CHINA',0,'ACH_LP_FOUR_HOWTO','ACH_LP_FOUR_DESC','ACH_07.png',0.0,0,'ACH_LP_FOUR_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_TRIGNOME_LP_FOUR_CHINA',0,'ACH_TRIGNOME_LP_FOUR_HOWTO','ACH_TRIGNOME_LP_FOUR_DESC','ACH_08.png',0.0,0,'ACH_TRIGNOME_LP_FOUR_FB','',0);
INSERT INTO "Achievements" VALUES(5,'ACH_FIRST_FILE_CHINA',0,'ACH_FIRST_FILE_HOWTO','ACH_FIRST_FILE_DESC','ACH_09.png',0.0,0,'ACH_FIRST_FILE_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_TEN_FILES_CHINA',0,'ACH_TEN_FILES_HOWTO','ACH_TEN_FILES_DESC','ACH_10.png',0.0,0,'ACH_TEN_FILES_FB','',0);
INSERT INTO "Achievements" VALUES(5,'ACH_ONE_BONUS_CHINA',0,'ACH_ONE_BONUS_HOWTO','ACH_ONE_BONUS_DESC','ACH_11.png',0.0,0,'ACH_ONE_BONUS_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_ACTION_CHINA',0,'ACH_ACTION_HOWTO','ACH_ACTION_DESC','ACH_12.png',0.0,0,'ACH_ACTION_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_FINDUS_CHINA',0,'ACH_FINDUS_HOWTO','ACH_FINDUS_DESC','ACH_13.png',0.0,0,'ACH_FINDUS_FB','',0);
INSERT INTO "Achievements" VALUES(5,'ACH_EFFICIENT_CUT_CHINA',0,'ACH_EFFICIENT_CUT_HOWTO','ACH_EFFICIENT_CUT_DESC','ACH_14.png',0.0,0,'ACH_EFFICIENT_CUT_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_CONFIDENCE_CHINA',0,'ACH_CONFIDENCE_HOWTO','ACH_CONFIDENCE_DESC','ACH_15.png',0.0,0,'ACH_CONFIDENCE_FB','',0);
INSERT INTO "Achievements" VALUES(5,'ACH_PERSISTENCE_CHINA',0,'ACH_PERSISTENCE_HOWTO','ACH_PERSISTENCE_DESC','ACH_16.png',0.0,0,'ACH_PERSISTENCE_FB','',0);
INSERT INTO "Achievements" VALUES(25,'ACH_SHARE_WEIBO_CHINA',0,'ACH_SHARE_WEIBO_HOWTO','ACH_SHARE_WEIBO_DESC','ACH_17.png',0.0,0,'ACH_SHARE_WEIBO_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_ONLYWATER_CHINA',0,'ACH_ONLYWATER_HOWTO','ACH_ONLYWATER_DESC','ACH_18.png',0.0,0,'ACH_ONLYWATER_FB','',0);
INSERT INTO "Achievements" VALUES(5,'ACH_KILLBELL_CHINA',0,'ACH_KILLBELL_HOWTO','ACH_KILLBELL_DESC','ACH_19.png',0.0,0,'ACH_KILLBELL_FB','',0);
INSERT INTO "Achievements" VALUES(10,'ACH_FAIL80_CHINA',0,'ACH_FAIL80_HOWTO','ACH_FAIL80_DESC','ACH_20.png',0.0,0,'ACH_FAIL80_FB','',0);
CREATE TABLE "PlayerData" ("ID" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , "EventName" TEXT NOT NULL , "EventValue" INTEGER NOT NULL , 'EventStringValue' TEXT);
INSERT INTO "PlayerData" VALUES(0,'PoisonWaterLosses',0,NULL);
INSERT INTO "PlayerData" VALUES(1,'RubberDuckiesKilled',0,NULL);
INSERT INTO "PlayerData" VALUES(2,'FirstCollectibleSighted',0,NULL);
INSERT INTO "PlayerData" VALUES(3,'LastHourCount',0,NULL);
INSERT INTO "PlayerData" VALUES(4,'SwampyTouched',0,NULL);
INSERT INTO "PlayerData" VALUES(5,'RateMePromptEligible',1,NULL);
INSERT INTO "PlayerData" VALUES(6,'RateMePromptNumToPrompt',10,NULL);
INSERT INTO "PlayerData" VALUES(7,'DateOfNextAMPSCheck',0,NULL);
INSERT INTO "PlayerData" VALUES(8,'AMPSDownloadWasInterrupted',0,NULL);
INSERT INTO "PlayerData" VALUES(9,'PromotionSuffix',-1,'');
INSERT INTO "PlayerData" VALUES(10,'ThemeSuffix',-1,'');
INSERT INTO "PlayerData" VALUES(11,'CrankyPackShown',1,NULL);
INSERT INTO "PlayerData" VALUES(12,'PresentUnavailableChallenges',1,'');
INSERT INTO "PlayerData" VALUES(13,'CloudSyncPrompted',0,NULL);
INSERT INTO "PlayerData" VALUES(14,'PreferredLanguage',-1,'');
INSERT INTO "PlayerData" VALUES(15,'DoofIAPState',0,'');
INSERT INTO "PlayerData" VALUES(16,'ShouldDeleteLocalAmpsCat',1,'');
INSERT INTO "PlayerData" VALUES(17,'TimesFailed',0,'');
INSERT INTO "PlayerData" VALUES(18,'FirstInstallTime',-1,'');
INSERT INTO "PlayerData" VALUES(19,'TotalPlayTimeInLevel',0,'');
CREATE TABLE 'IAPInfo' ('ID' INTEGER PRIMARY KEY  NOT NULL  UNIQUE , 'Internal'	TEXT, 'iOS'		TEXT, 'Google'		TEXT, 'Amazon'		TEXT, 'ChuKong'		TEXT);
INSERT INTO "IAPInfo" VALUES(1,'locksmith_one','disney.wmp.locksmith_one','2206674','2206800','2206674');
INSERT INTO "IAPInfo" VALUES(2,'locksmith_perry','disney.wmp.locksmith_perry','2206675','2206801','2206675');
INSERT INTO "IAPInfo" VALUES(3,'doof01','disney.wmp.doofs_story','2207030','2207055','2207030');
INSERT INTO "IAPInfo" VALUES(4,'locksmith_doof','disney.wmp.locksmith_doof','2207031','2207056','2207031');
INSERT INTO "IAPInfo" VALUES(5,'coins_01','com.ath.wmyxyychina.coins_01','com.punchboxusa.xiyangyangxiaowanpi.um.iap.tier1','2206800','2206674');
INSERT INTO "IAPInfo" VALUES(6,'coins_02','com.ath.wmyxyychina.coins_02','com.punchboxusa.xiyangyangxiaowanpi.um.iap.tier2','2206801','2206675');
INSERT INTO "IAPInfo" VALUES(7,'coins_03','com.ath.wmyxyychina.coins_03','com.punchboxusa.xiyangyangxiaowanpi.um.iap.tier3','2207030','2207055');
INSERT INTO "IAPInfo" VALUES(8,'coins_04','com.ath.wmyxyychina.coins_04','com.punchboxusa.xiyangyangxiaowanpi.um.iap.tier4','2207056','2207031');
ANALYZE sqlite_master;
INSERT INTO "sqlite_stat1" VALUES('LevelInfo',NULL,'201');
INSERT INTO "sqlite_stat1" VALUES('IAPInfo','sqlite_autoindex_IAPInfo_1','1 1');
INSERT INTO "sqlite_stat1" VALUES('Achievements','sqlite_autoindex_Achievements_1','31 1');
INSERT INTO "sqlite_stat1" VALUES('PlayerData',NULL,'11');
CREATE TABLE "LevelPackInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Storyline" INTEGER NOT NULL  DEFAULT (-1) ,"PackName" TEXT NOT NULL  DEFAULT ('') ,"Hidden" BOOL NOT NULL  DEFAULT (0) ,"IAP_item_id" TEXT NOT NULL  DEFAULT ('') ,"Bought" BOOL NOT NULL  DEFAULT (1) ,"Unlocked" BOOL NOT NULL  DEFAULT (1) ,"StarsRequired" INTEGER NOT NULL  DEFAULT (-1) ,"TitleText" TEXT NOT NULL  DEFAULT ('') ,"SubtitleText" TEXT NOT NULL  DEFAULT ('') ,"MainTexture" TEXT NOT NULL  DEFAULT ('') ,"FrameTexture" TEXT NOT NULL  DEFAULT ('') , "LockedTexture" TEXT NOT NULL  DEFAULT '', "IsLevelPack" BOOL NOT NULL  DEFAULT 1, "LockedText" TEXT NOT NULL  DEFAULT '', FB_AlbumName text, LS_Unlocked integer default(0), "SimulateUnlock" BOOL NOT NULL  DEFAULT 0, 'IAP_Text' TEXT NOT NULL DEFAULT '','BGTexture' TEXT NOT NULL DEFAULT '','MainSkeleton' TEXT NOT NULL DEFAULT '');
INSERT INTO "LevelPackInfo" VALUES(10,1000,'LP_ONE',0,'',1,1,-1,'LP_ONE_TITLE','LP_ONE_SUBTITLE','2x2.webp','2x2.webp','',1,'','Mission 1 - An Agent I Can Trust',0,0,'','materials_rock_backdrop_p01.webp','back1');
INSERT INTO "LevelPackInfo" VALUES(11,1000,'LP_TWO',0,'',1,0,35,'LP_TWO_TITLE','LP_TWO_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 2 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back2');
INSERT INTO "LevelPackInfo" VALUES(12,1000,'LP_THREE',0,'',1,0,85,'LP_THREE_TITLE','LP_THREE_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 3 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back3');
INSERT INTO "LevelPackInfo" VALUES(13,1000,'LP_FOUR',0,'',1,0,150,'LP_FOUR_TITLE','LP_FOUR_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 4 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back4');
INSERT INTO "LevelPackInfo" VALUES(30,1000,'MORE_COMING_SOON',0,'',1,1,-1,'','MORE_LEVELS','2x2.webp','2x2.webp','',0,'','',0,0,'','materials_rock_backdrop_p01.webp','back5');
INSERT INTO "LevelPackInfo" VALUES(-1,-1,'LP_LOW',0,'',1,0,85,'LP_LOW_TITLE','LP_LOW_SUBTITLE','LP_LOW_weslie.webp','','',1,'LOCKED','',0,0,'','materials_rock_backdrop_p01.webp','');
CREATE TABLE "Settings" ("ID" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "Name" TEXT NOT NULL , "Value" INTEGER NOT NULL  DEFAULT 0);
INSERT INTO "Settings" VALUES(0,'DatabaseVersion',1);
INSERT INTO "Settings" VALUES(10,'AudioOn',1);
INSERT INTO "Settings" VALUES(11,'MusicOn',1);
INSERT INTO "Settings" VALUES(12,'VoiceOn',0);
INSERT INTO "Settings" VALUES(13,'SubtitlesOn',1);
INSERT INTO "Settings" VALUES(20,'Graphics',-1);
INSERT INTO "Settings" VALUES(21,'TargetFramerate',-1);
INSERT INTO "Settings" VALUES(31,'AllowFacebookPublishing',-1);
INSERT INTO "Settings" VALUES(32,'CloudSync',0);
INSERT INTO "Settings" VALUES(33,'FirstLaunch', 1);
INSERT INTO "Settings" VALUES(34,'UsedHintsTutorial', 0);
INSERT INTO "Settings" VALUES(35,'HasSinaMicroBlog', 1);
INSERT INTO "Settings" VALUES(36,'HasChukongExchangeCode', 1);

CREATE TABLE "HubInfo" (	"ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , 
							"Storyline" INTEGER DEFAULT -1, 
							"IAP_item_id" TEXT DEFAULT '', 
							"Bought" BOOL DEFAULT 1, 
							"HubType" INTEGER DEFAULT 1,
							
							"TitleTexture_1" TEXT DEFAULT '',
							"MainTexture_1" TEXT DEFAULT '', 
							"FrameTexture_1" TEXT DEFAULT '', 

							"TitleTexture_2" TEXT DEFAULT '',
							"MainTexture_2" TEXT DEFAULT '',
							"FrameTexture_2" TEXT DEFAULT '',

							"TitleTexture_3" TEXT DEFAULT '',
							"MainTexture_3" TEXT DEFAULT '',
							"FrameTexture_3" TEXT DEFAULT '',
							
							"TextColor" TEXT DEFAULT '',
							
							"TextLine1" TEXT DEFAULT '', 
							"TextLine2" TEXT DEFAULT '', 
							
							"StarCharacter" TEXT DEFAULT '', 
							"ItemCharacter" TEXT DEFAULT '', 
							
							"StarSQL1" TEXT DEFAULT '',
							"StarSQL2" TEXT DEFAULT '',
							
							"ItemSQL1" TEXT DEFAULT '',
							"ItemSQL2" TEXT DEFAULT '',
							
							"TextButton" TEXT DEFAULT '',
							
							"AlertText" TEXT DEFAULT ''	, "Unlocked" BOOL NOT NULL  DEFAULT 1, "Tips" TEXT DEFAULT '');
INSERT INTO "HubInfo" VALUES(10,1000,'',1,1,'','UI_Character_frame_weslie_zh-Hans.png','','','','','','','','37 108 138','','HUB_TEXT_PERRY_BODY','☆','⚀','','','SELECT count(ID) FROM CollectibleInfo WHERE Unlocked=1','SELECT count(ID) FROM CollectibleInfo','','',0,'');
INSERT INTO "HubInfo" VALUES(15,-1,'',0,1,'','UI_Character_comingsoon.png','','','','','','','','','','','','','','','','','','',0,'COMMING_SOON');
/*INSERT INTO "HubInfo" VALUES(80,-1,'',1,'','story_swampy_icon.png','story_story_frame.png','37 108 138','HUB_TEXT_SWAMPY_TITLE','HUB_TEXT_SWAMPY_BODY','','','','','','','TRY_NOW','',0);*/
/*INSERT INTO "HubInfo" VALUES(90,-1,'',1,1,'','story_intro_icon.png','story_story_frame_XYY.png','','','','','','','37 108 138','','','','','','','','','','',0);*/
CREATE TABLE "CollectibleInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Unlocked" BOOL NOT NULL  DEFAULT (0) ,"HasViewed" BOOL NOT NULL  DEFAULT (0) ,"Type" INTEGER NOT NULL  DEFAULT (0) ,"LevelFile" TEXT NOT NULL  DEFAULT ('') ,"IconTexture" TEXT NOT NULL ,"FullTexture" TEXT NOT NULL ,"BaseName" TEXT NOT NULL );
INSERT INTO "CollectibleInfo" VALUES(1,0,0,0,'','Collections_Pack1_Sunglasses.png','Collections_Pack1_Sunglasses_Detail.png','GLASSES');
INSERT INTO "CollectibleInfo" VALUES(2,0,0,0,'','Collections_Pack1_microphone.png','Collections_Pack1_microphone_Detail.png','MICROPHONE');
INSERT INTO "CollectibleInfo" VALUES(3,0,0,0,'/Perry/Levels/b_01_justdoit','Collections_Pack1_JacksonSheep.png','Collections_Pack1_JacksonSheep_Detail.png','JACKSON');
INSERT INTO "CollectibleInfo" VALUES(4,0,0,0,'','Collections_Pack1_RedHeeledShoes.png','Collections_Pack1_RedHeeledShoes_Detail.png','SHOES');
INSERT INTO "CollectibleInfo" VALUES(5,0,0,0,'','Collections_Pack1_sunhat.png','Collections_Pack1_sunhat_Detail.png','SUNHAT');
INSERT INTO "CollectibleInfo" VALUES(6,0,0,0,'/Perry/Levels/b_02_dodolook','Collections_Pack1_LadyLiberty.png','Collections_Pack1_LadyLiberty_Detail.png','LIBERTY');
INSERT INTO "CollectibleInfo" VALUES(7,0,0,0,'','Collections_Pack2_RedApple.png','Collections_Pack2_RedApple_Detail.png','APPLE');
INSERT INTO "CollectibleInfo" VALUES(8,0,0,0,'','Collections_Pack2_WarChess.png','Collections_Pack2_WarChess_Detail.png','CHESS');
INSERT INTO "CollectibleInfo" VALUES(9,0,0,0,'/Perry/Levels/b_03_none','Collections_Pack2_Thinker.png','Collections_Pack2_Thinker_Detail.png','THINKER');
INSERT INTO "CollectibleInfo" VALUES(10,0,0,0,'','Collections_Pack2_BoxingGloves.png','Collections_Pack2_BoxingGloves_Detail.png','GLOVES');
INSERT INTO "CollectibleInfo" VALUES(11,0,0,0,'','Collections_Pack2_Nunchaku.png','Collections_Pack2_Nunchaku_Detail.png','NUNCHAKU');
INSERT INTO "CollectibleInfo" VALUES(12,0,0,0,'/Perry/Levels/b_04_waterflying','Collections_Pack2_TigerStatue.png','Collections_Pack2_TigerStatue_Detail.png','TIGER');
INSERT INTO "CollectibleInfo" VALUES(13,0,0,0,'','Collections_Pack3_Palette.png','Collections_Pack3_Palette_Detail.png','PALETTE');
INSERT INTO "CollectibleInfo" VALUES(14,0,0,0,'','Collections_Pack3_Portrait.png','Collections_Pack3_Portrait_Detail.png','PORTRAIT');
INSERT INTO "CollectibleInfo" VALUES(15,0,0,0,'/Perry/Levels/b_05_so','Collections_Pack3_Painter.png','Collections_Pack3_Painter_Detail.png','PAINTER');
INSERT INTO "CollectibleInfo" VALUES(16,0,0,0,'','Collections_Pack3_Telescope.png','Collections_Pack3_Telescope_Detail.png','TELESCOPE');
INSERT INTO "CollectibleInfo" VALUES(17,0,0,0,'','Collections_Pack3_MasterKey.png','Collections_Pack3_MasterKey_Detail.png','MASTERKEY');
INSERT INTO "CollectibleInfo" VALUES(18,0,0,0,'/Perry/Levels/b_06_pull_up','Collections_Pack3_MartialWolf.png','Collections_Pack3_MartialWolf_Detail.png','MARTIALWOLF');
INSERT INTO "CollectibleInfo" VALUES(19,0,0,0,'','Collections_Pack4_Mask.png','Collections_Pack4_Mask_Detail.png','MASK');
INSERT INTO "CollectibleInfo" VALUES(20,0,0,0,'','Collections_Pack4_Horn.png','Collections_Pack4_Horn_Detail.png','HORN');
INSERT INTO "CollectibleInfo" VALUES(21,0,0,0,'/Perry/Levels/b_07_what','Collections_Pack4_Joker.png','Collections_Pack4_Joker_Detail.png','JOKER');
INSERT INTO "CollectibleInfo" VALUES(22,0,0,0,'','Collections_Pack4_Fan.png','Collections_Pack4_Fan_Detail.png','FAN');
INSERT INTO "CollectibleInfo" VALUES(23,0,0,0,'','Collections_Pack4_Toy.png','Collections_Pack4_Toy_Detail.png','TOY');
INSERT INTO "CollectibleInfo" VALUES(24,0,0,0,'/Perry/Levels/b_08_key','Collections_Pack4_Princess.png','Collections_Pack4_Princess_Detail.png','PRINCESS');
CREATE TABLE "LevelInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Name" TEXT NOT NULL ,"Filename" TEXT NOT NULL ,"Stars" INTEGER NOT NULL  DEFAULT (0) ,"PackName" TEXT,"TimesPlayed" INTEGER NOT NULL  DEFAULT (0) ,"TimesFinished" INTEGER NOT NULL  DEFAULT (0) ,"Unlocked" BOOL NOT NULL  DEFAULT (0) ,"ParTime" FLOAT NOT NULL  DEFAULT (15.0) ,"BestScore" INTEGER NOT NULL  DEFAULT (0) ,"CollectibleFound" TEXT NOT NULL ,"PlayTime" INTEGER NOT NULL  DEFAULT (0) ,"TimesRetried" INTEGER NOT NULL  DEFAULT (0) ,"IgnoreInStarCount" BOOL NOT NULL  DEFAULT (0) ,"Type" INTEGER DEFAULT (0) ,"StartDate" DATETIME,"EndDate" DATETIME,"Available" BOOL NOT NULL  DEFAULT (1) ,"IsBonus" BOOL NOT NULL  DEFAULT (0) , "ForceVO" INTEGER DEFAULT 0);
INSERT INTO "LevelInfo" VALUES(0,'LN_GIVE_ME_SOME_WATER','/Perry/Levels/01_p01_give_me_some_water',0,'LP_ONE',0,0,1,25.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(1,'LN_THE_RIGHT_DIRECTION','/Perry/Levels/02_p01_the_right_direction',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(2,'LN_GOING_BOTH_WAYS','/Perry/Levels/03_p01_going_both_ways',0,'LP_ONE',0,0,0,40.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(3,'LN_SPLIT_IT_UP','/Perry/Levels/04_p01_split_it_up',0,'LP_ONE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(4,'LN_BEWARE_OF_FIRE','/Perry/Levels/05_p01_beware_of_fire',0,'LP_ONE',0,0,0,40.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(5,'LN_COVER_ME','/Perry/Levels/06_p01_cover_me',0,'LP_ONE',0,0,0,35.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(6,'LN_TROUGH_THE_JUNGLE','/Perry/Levels/07_p01_trough_the_jungle',0,'LP_ONE',0,0,0,35.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(7,'LN_JUMP_UP_GET_DOWN','/Perry/Levels/08_p01_jump_up_get_down',0,'LP_ONE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(8,'LN_WATER_ARCH','/Perry/Levels/09_p01_water_arch',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(9,'LN_CROSSFIRE','/Perry/Levels/10_p01_crossfire',0,'LP_ONE',0,0,0,45.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(10,'LN_LIGHT_THE_FUSE','/Perry/Levels/11_p01_light_the_fuse',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(11,'LN_LET_IT_GROW','/Perry/Levels/12_p01_let_it_grow',0,'LP_ONE',0,0,0,40.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(12,'LN_BUILD_AND_DESTROY','/Perry/Levels/13_p01_build_and_destroy',0,'LP_ONE',0,0,0,35.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(13,'LN_SWITCH_AND_GO','/Perry/Levels/14_p01_switch_and_go',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(14,'LN_BLOCK_THAT_FIRE','/Perry/Levels/15_p01_block_the_fire',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(15,'LN_MATCH_THE_COLORS','/Perry/Levels/16_p01_match_the_colors',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(16,'LN_LEAKING_WATER','/Perry/Levels/17_p01_leaking_water',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(17,'LN_THE_RIGHT_ORDER','/Perry/Levels/18_p01_the_right_order',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(18,'LN_THE_RIGHT_MOMENT','/Perry/Levels/19_p01_the_right_moment',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(19,'LN_USE_THE_LOOP','/Perry/Levels/20_p01_use_the_loop',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(20,'LN_KEEP_AFLOAT','/Perry/Levels/01_p02_keep_afloat',0,'LP_TWO',0,0,0,35.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(21,'LN_DOLA','/Perry/Levels/02_p02_dola',0,'LP_TWO',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(22,'LN_TAKE_ME_HIGHER','/Perry/Levels/03_p02_take_me_higher',0,'LP_TWO',0,0,0,45.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(23,'LN_ONE_BY_ONE','/Perry/Levels/04_p02_one_by_one',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(24,'LN_U_TUBE','/Perry/Levels/05_p02_u_tube',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(25,'LN_GREEN_BRIDGE','/Perry/Levels/06_p02_green_bridge',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(26,'LN_GLASSLANDS','/Perry/Levels/07_p02_grasslands',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(27,'LN_BURNING_WOK','/Perry/Levels/08_p02_burning_wok',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(28,'LN_HONEYCOMB','/Perry/Levels/09_p02_honeycomb',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(29,'LN_JUMPER','/Perry/Levels/10_p02_jumper',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(30,'LN_O_B','/Perry/Levels/11_p02_o_b',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(31,'LN_DONT_CROSS_THE_WATER_FALL','/Perry/Levels/12_p02_a',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(32,'LN_OIL_AND_SWITCH','/Perry/Levels/13_p02_o_and_s1',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(33,'LN_BURNINGOIL_AND_SWITCH','/Perry/Levels/14_p02_b_s',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(34,'LN_BONGBONG','/Perry/Levels/15_p02_bongbong',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(35,'LN_BURNING_FLOWER','/Perry/Levels/16_p02_burning_flower',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(36,'LN_TRY_AGAIN','/Perry/Levels/17_p02_try_again',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(37,'LN_RECYCLE','/Perry/Levels/18_p02_recycle',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(38,'LN_AGAIN','/Perry/Levels/19_p02_again',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(39,'LN_FIRE_RAIN','/Perry/Levels/20_p02_fire_rain',0,'LP_TWO',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(40,'LN_W_T','/Perry/Levels/01_p03_w_t',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(41,'LN_TAKE_ME_HIGH','/Perry/Levels/02_p03_take_me_high',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(42,'LN_W_T_S','/Perry/Levels/03_p03_w_t_s',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(43,'LN_TOXIC_VINE','/Perry/Levels/04_p03_toxic_vine',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(44,'LN_TOXIC_SPRINKLER','/Perry/Levels/05_p03_toxic_sprinkler',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(45,'LN_T_V_O','/Perry/Levels/06_p03_t_v_o',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(46,'LN_T_O_W','/Perry/Levels/07_p03_t_v_w',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(47,'LN_TWO_BOMBS','/Perry/Levels/08_p03_two_bombs',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(48,'LN_BANGBANGWAY','/Perry/Levels/09_p03_bangbangway',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(49,'LN_CRASH','/Perry/Levels/10_p03_crash',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(50,'LN_B_V_S','/Perry/Levels/11_p03_b_v_s',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(51,'LN_BRUSH','/Perry/Levels/12_p03_brush',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(52,'LN_COVA','/Perry/Levels/13_p03_cova',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(53,'LN_BURNING_VINE','/Perry/Levels/14_p03_burning_vine',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(54,'LN_FIRE_IN_THE_HOLE','/Perry/Levels/15_p03_fire_in_the_hole',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(55,'LN_CHANGING_WAYS','/Perry/Levels/16_p03_changing_ways',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(56,'LN_CROSS_MOVE','/Perry/Levels/17_p03_cross_move',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(57,'LN_F','/Perry/Levels/18_p03_f',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(58,'LN_T','/Perry/Levels/19_p03_t',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(59,'LN_OUT_AND_IN','/Perry/Levels/20_p03_out_and_in',0,'LP_THREE',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(60,'LN_BIG_ROOTS','/Perry/Levels/01_p04_big_roots',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(61,'LN_BURNING','/Perry/Levels/02_p04_burning',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(62,'LN_TIME','/Perry/Levels/03_p04_time',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(63,'LN_MUCH_TIME','/Perry/Levels/04_p04_much_time',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(64,'LN_PUT_WATER_ON_WOOD','/Perry/Levels/05_p04_put_water_on_wood',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(65,'LN_LIGHT_THE_OIL','/Perry/Levels/06_p04_light_the_oil',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(66,'LN_REWIRE_THE_CONNECTION','/Perry/Levels/07_p04_rewire_the_connection',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(67,'LN_OPEN_AND_CLOSE','/Perry/Levels/08_p04_open_and_close',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(68,'LN_LINK_IT','/Perry/Levels/09_p04_link_it',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(69,'LN_FOREST','/Perry/Levels/10_p04_forest',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(70,'LN_BOOM','/Perry/Levels/11_p04_boom',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(71,'LN_OH_MY_GOD','/Perry/Levels/12_p04_oh_my_god',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(72,'LN_REPAIR_THE_WIRING','/Perry/Levels/13_p04_repair_the_wiring',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(73,'LN_LINKED_TREES','/Perry/Levels/14_p04_linked_trees',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(74,'LN_FIREBALL','/Perry/Levels/15_p04_fireball',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(75,'LN_TOW_WOODS','/Perry/Levels/16_p04_tow_woods',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(76,'LN_BRIDGE','/Perry/Levels/17_p04_bridge',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(77,'LN_OIL','/Perry/Levels/18_p04_oil',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(78,'LN_SUPERNATURAL','/Perry/Levels/19_p04_supernatural',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(79,'LN_SUN','/Perry/Levels/20_p04_sun',0,'LP_FOUR',0,0,0,25.0,0,'',0,0,0,0,NULL,NULL,1,0,0);

INSERT INTO "LevelInfo" VALUES(2000,'LN_JUSTDOIT','/Perry/Levels/b_01_justdoit',0,'LP_ONE',0,0,0,35.0,0,'',0,0,0,0,NULL,NULL,1,1,0);	
INSERT INTO "LevelInfo" VALUES(2001,'LN_DODOLOOK','/Perry/Levels/b_02_dodolook',0,'LP_ONE',0,0,0,60.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(2002,'LN_NONE','/Perry/Levels/b_03_none',0,'LP_TWO',0,0,0,90.0,0,'',0,0,0,0,NULL,NULL,1,1,0);	
INSERT INTO "LevelInfo" VALUES(2003,'LN_WATERFLYING','/Perry/Levels/b_04_waterflying',0,'LP_TWO',0,0,0,90.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(2004,'LN_SO','/Perry/Levels/b_05_so',0,'LP_THREE',0,0,0,90.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(2005,'LN_PILL_UP','/Perry/Levels/b_06_pull_up',0,'LP_THREE',0,0,0,60.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(2006,'LN_WHAT','/Perry/Levels/b_07_what',0,'LP_FOUR',0,0,0,60.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(2007,'LN_KEY','/Perry/Levels/b_08_key',0,'LP_FOUR',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,1,0);
INSERT INTO "LevelInfo" VALUES(7000,'LN_WESLIE_LOW','/Perry/Levels/low_weslie_weekly',0,'LP_LOW',0,0,0,15.0,0,'',0,0,0,0,NULL,NULL,1,0,0);	
INSERT INTO "LevelInfo" VALUES(7001,'LN_WOLF_LOW','/Perry/Levels/low_wolf_weekly',0,'LP_LOW',0,0,0,15.0,0,'',0,0,0,0,NULL,NULL,1,0,0);	
CREATE TABLE "CoinsInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Coins" TEXT NOT NULL  DEFAULT ('0VnxEJX0taE=') );
INSERT INTO "CoinsInfo" VALUES(0,'0VnxEJX0taE=');
CREATE TABLE "StoreInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE ,"Coins" INTEGER DEFAULT 0,"Add_Coins" FLOAT DEFAULT 0,"Texture_Normal" TEXT DEFAULT '',"Texture_Pressed" TEXT DEFAULT '',"Coins_Icon" TEXT DEFAULT '',"Hot_Icon" TEXT DEFAULT '',"Discount_Icon" TEXT DEFAULT '', "IAP_item_id" TEXT DEFAULT '', "Coins_Price" TEXT DEFAULT '',"Hot_Label" TEXT DEFAULT '',"Discount_Label" TEXT DEFAULT '');
/*INSERT INTO "StoreInfo" VALUES(1,500,0,'UI_Store_btn_price_normal.webp','UI_Store_btn_price_pressed.webp','UI_Store_coin1.webp','UI_Store_hot_sale.webp','UI_Store_discount.webp','coins_01','COINS_PRICE_IOS_01','PRICE_HOT','PRICE_SALE1');*/
INSERT INTO "StoreInfo" VALUES(1,1000,0,'UI_Store_btn_price_normal.webp','UI_Store_btn_price_pressed.webp','UI_Store_coin05.webp','','','coins_01','COINS_PRICE_IOS_01','','');
INSERT INTO "StoreInfo" VALUES(2,2100,0,'','','UI_Store_coin06.webp','','','coins_02','COINS_PRICE_IOS_02','','');
INSERT INTO "StoreInfo" VALUES(3,3300,0,'UI_Store_btn_price_classic_normal.webp','UI_Store_btn_price_classic_pressed.webp','UI_Store_coin07.webp','UI_Store_hot_sale.webp','','coins_03','COINS_PRICE_IOS_03','PRICE_OFF','');
INSERT INTO "StoreInfo" VALUES(4,4700,0,'','','UI_Store_coin08.webp','','','coins_04','COINS_PRICE_IOS_04','','');
CREATE TABLE LOWInfo ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "Storyline" INTEGER NOT NULL UNIQUE DEFAULT -1, "PackName" TEXT NOT NULL DEFAULT 'LP_', "DisplayOrder" INTEGER NOT NULL DEFAULT -1, "LevelName" TEXT NOT NULL DEFAULT 'LN_', "DownloadDate" DATETIME NOT NULL DEFAULT '1000-01-01 00:00:00', "PortalTexture" TEXT DEFAULT '', "PlayButtonTexture" TEXT DEFAULT '', "BannerTexture" TEXT DEFAULT '', levelStoryline INTEGER);
INSERT INTO "LOWInfo" VALUES(1,1,'LP_LOW',0,'LN_WESLIE_LOW','2013-06-10 00:00:00','WMW_UI_low_swampy.png','WMW_UI_low_swampy_level_icon_%d.png','WMW_UI_low_swampy_banner.png',1);
INSERT INTO "LOWInfo" VALUES(2,2,'LP_LOW',1,'LN_WOLF_LOW','2013-06-10 00:00:00','WMW_UI_low_cranky.png','WMW_UI_low_cranky_level_icon_%d.png','WMW_UI_low_cranky_banner.png',2);
CREATE TABLE "PowerUpInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , 
							"Type" INTEGER NOT NULL  DEFAULT (0),  
							"IconTexture" TEXT NOT NULL DEFAULT '',
							"NameText" TEXT NOT NULL DEFAULT '', 
							"DiscribeText"TEXT  NOT NULL  DEFAULT '', 
							"CoinTexture" TEXT NOT NULL DEFAULT '',
							"CostNum" INTEGER NOT NULL  DEFAULT (0),
							"Unlock" INTEGER NOT NULL DEFAULT(0));
INSERT INTO "PowerUpInfo" VALUES(1,0,'UI_hint_light.png','HINTS_NAME','HINTS_DES','UI_Powerup_coin.png',100,1);
INSERT INTO "PowerUpInfo" VALUES(2,1,'UI_Powerup_Magnetic_bell.png','METALLIC_BELL','METALLIC_BELL_DSC','UI_Powerup_coin.png',100,0);
INSERT INTO "PowerUpInfo" VALUES(3,1,'UI_Powerup_pre-Filled_bell.png','PREFILL_BELL','PREFILL_BELL_DSC','UI_Powerup_coin.png',100,0);
INSERT INTO "PowerUpInfo" VALUES(4,1,'UI_Powerup_Immunity_bell.png','IMMUNITY_BELL','IMMUNITY_BELL_DSC','UI_Powerup_coin.png',100,0);
DELETE FROM sqlite_sequence;
INSERT INTO "sqlite_sequence" VALUES('PlayerData',16);
INSERT INTO "sqlite_sequence" VALUES('Settings',32);
INSERT INTO "sqlite_sequence" VALUES('TimebasedEventInfo',1);
CREATE TABLE IntroStoryInfo ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "Unlocked" BOOL NOT NULL DEFAULT(0));
INSERT INTO "IntroStoryInfo" VALUES(0, 1);
INSERT INTO "IntroStoryInfo" VALUES(1, 1);
INSERT INTO "IntroStoryInfo" VALUES(2, 0);
INSERT INTO "IntroStoryInfo" VALUES(3, 0);
INSERT INTO "IntroStoryInfo" VALUES(4, 0);
INSERT INTO "IntroStoryInfo" VALUES(5, 0);
INSERT INTO "IntroStoryInfo" VALUES(6, 0);
INSERT INTO "IntroStoryInfo" VALUES(7, 0);
INSERT INTO "IntroStoryInfo" VALUES(8, 0);
COMMIT;
