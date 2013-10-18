PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE "ContentTable" ("Points" INTEGER NOT NULL , "ID" TEXT PRIMARY KEY  NOT NULL  UNIQUE , "Hidden" INTEGER NOT NULL  DEFAULT 0, "PreEarnedDescription" TEXT NOT NULL , "EarnedDescription" TEXT NOT NULL , "Image" TEXT NOT NULL , "PercentComplete" FLOAT NOT NULL  DEFAULT 0, "SortingGroup" INTEGER NOT NULL  DEFAULT 0, 'FacebookDescription' TEXT NOT NULL DEFAULT '', "LevelFile" TEXT NOT NULL  DEFAULT '', "HasViewed" BOOL NOT NULL  DEFAULT 0);
INSERT INTO "ContentTable" VALUES(10,'ACH_LP_ONE_CHINA',0,'ACH_LP_ONE_HOWTO','ACH_LP_ONE_DESC','ACH_01.png',0.0,0,'ACH_LP_ONE_FB','',0);
INSERT INTO "ContentTable" VALUES(25,'ACH_TRIGNOME_LP_ONE_CHINA',0,'ACH_TRIGNOME_LP_ONE_HOWTO','ACH_TRIGNOME_LP_ONE_DESC','ACH_02.png',0.0,0,'ACH_TRIGNOME_LP_ONE_FB','',0);
INSERT INTO "ContentTable" VALUES(10,'ACH_LP_TWO_CHINA',0,'ACH_LP_TWO_HOWTO','ACH_LP_TWO_DESC','ACH_03.png',0.0,0,'ACH_LP_TWO_FB','',0);

CREATE TABLE "PlayerData" ("ID" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , "EventName" TEXT NOT NULL , "EventValue" INTEGER NOT NULL , 'EventStringValue' TEXT);
INSERT INTO "PlayerData" VALUES(17,'TimesFailed',0,'');
INSERT INTO "PlayerData" VALUES(18,'FirstInstallTime',-1,'');
INSERT INTO "PlayerData" VALUES(19,'TotalPlayTimeInLevel',0,'');

CREATE TABLE "HeroTable" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Storyline" INTEGER NOT NULL  DEFAULT (-1) ,"PackName" TEXT NOT NULL  DEFAULT ('') ,"Hidden" BOOL NOT NULL  DEFAULT (0) ,"IAP_item_id" TEXT NOT NULL  DEFAULT ('') ,"Bought" BOOL NOT NULL  DEFAULT (1) ,"Unlocked" BOOL NOT NULL  DEFAULT (1) ,"StarsRequired" INTEGER NOT NULL  DEFAULT (-1) ,"TitleText" TEXT NOT NULL  DEFAULT ('') ,"SubtitleText" TEXT NOT NULL  DEFAULT ('') ,"MainTexture" TEXT NOT NULL  DEFAULT ('') ,"FrameTexture" TEXT NOT NULL  DEFAULT ('') , "LockedTexture" TEXT NOT NULL  DEFAULT '', "IsLevelPack" BOOL NOT NULL  DEFAULT 1, "LockedText" TEXT NOT NULL  DEFAULT '', FB_AlbumName text, LS_Unlocked integer default(0), "SimulateUnlock" BOOL NOT NULL  DEFAULT 0, 'IAP_Text' TEXT NOT NULL DEFAULT '','BGTexture' TEXT NOT NULL DEFAULT '','MainSkeleton' TEXT NOT NULL DEFAULT '');
INSERT INTO "HeroTable" VALUES(10,1000,'LP_ONE',0,'',1,1,-1,'LP_ONE_TITLE','LP_ONE_SUBTITLE','2x2.webp','2x2.webp','',1,'','Mission 1 - An Agent I Can Trust',0,0,'','materials_rock_backdrop_p01.webp','back1');
INSERT INTO "HeroTable" VALUES(11,1000,'LP_TWO',0,'',1,0,35,'LP_TWO_TITLE','LP_TWO_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 2 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back2');
INSERT INTO "HeroTable" VALUES(12,1000,'LP_THREE',0,'',1,0,85,'LP_THREE_TITLE','LP_THREE_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 3 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back3');
INSERT INTO "HeroTable" VALUES(13,1000,'LP_FOUR',0,'',1,0,150,'LP_FOUR_TITLE','LP_FOUR_SUBTITLE','2x2.webp','2x2.webp','2x2.webp',1,'LOCKED','Mission 4 - A Thousand Times Over',0,0,'','materials_rock_backdrop_p01.webp','back4');
INSERT INTO "HeroTable" VALUES(30,1000,'MORE_COMING_SOON',0,'',1,1,-1,'','MORE_LEVELS','2x2.webp','2x2.webp','',0,'','',0,0,'','materials_rock_backdrop_p01.webp','back5');
INSERT INTO "HeroTable" VALUES(-1,-1,'LP_LOW',0,'',1,0,85,'LP_LOW_TITLE','LP_LOW_SUBTITLE','LP_LOW_weslie.webp','','',1,'LOCKED','',0,0,'','materials_rock_backdrop_p01.webp','');

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

CREATE TABLE "LevelInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL ,"Name" TEXT NOT NULL ,"Filename" TEXT NOT NULL ,"Stars" INTEGER NOT NULL  DEFAULT (0) ,"PackName" TEXT,"TimesPlayed" INTEGER NOT NULL  DEFAULT (0) ,"TimesFinished" INTEGER NOT NULL  DEFAULT (0) ,"Unlocked" BOOL NOT NULL  DEFAULT (0) ,"ParTime" FLOAT NOT NULL  DEFAULT (15.0) ,"BestScore" INTEGER NOT NULL  DEFAULT (0) ,"CollectibleFound" TEXT NOT NULL ,"PlayTime" INTEGER NOT NULL  DEFAULT (0) ,"TimesRetried" INTEGER NOT NULL  DEFAULT (0) ,"IgnoreInStarCount" BOOL NOT NULL  DEFAULT (0) ,"Type" INTEGER DEFAULT (0) ,"StartDate" DATETIME,"EndDate" DATETIME,"Available" BOOL NOT NULL  DEFAULT (1) ,"IsBonus" BOOL NOT NULL  DEFAULT (0) , "ForceVO" INTEGER DEFAULT 0);
INSERT INTO "LevelInfo" VALUES(0,'LN_GIVE_ME_SOME_WATER','/Perry/Levels/01_p01_give_me_some_water',0,'LP_ONE',0,0,1,25.0,0,'',0,0,0,0,NULL,NULL,1,0,1);
INSERT INTO "LevelInfo" VALUES(1,'LN_THE_RIGHT_DIRECTION','/Perry/Levels/02_p01_the_right_direction',0,'LP_ONE',0,0,0,30.0,0,'',0,0,0,0,NULL,NULL,1,0,0);
INSERT INTO "LevelInfo" VALUES(2,'LN_GOING_BOTH_WAYS','/Perry/Levels/03_p01_going_both_ways',0,'LP_ONE',0,0,0,40.0,0,'',0,0,0,0,NULL,NULL,1,0,1);

CREATE TABLE "StoreInfo" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE ,"Coins" INTEGER DEFAULT 0,"Add_Coins" FLOAT DEFAULT 0,"Texture_Normal" TEXT DEFAULT '',"Texture_Pressed" TEXT DEFAULT '',"Coins_Icon" TEXT DEFAULT '',"Hot_Icon" TEXT DEFAULT '',"Discount_Icon" TEXT DEFAULT '', "IAP_item_id" TEXT DEFAULT '', "Coins_Price" TEXT DEFAULT '',"Hot_Label" TEXT DEFAULT '',"Discount_Label" TEXT DEFAULT '');
INSERT INTO "StoreInfo" VALUES(1,1000,0,'UI_Store_btn_price_normal.webp','UI_Store_btn_price_pressed.webp','UI_Store_coin05.webp','','','coins_01','COINS_PRICE_IOS_01','','');
INSERT INTO "StoreInfo" VALUES(2,2100,0,'','','UI_Store_coin06.webp','','','coins_02','COINS_PRICE_IOS_02','','');
INSERT INTO "StoreInfo" VALUES(3,3300,0,'UI_Store_btn_price_classic_normal.webp','UI_Store_btn_price_classic_pressed.webp','UI_Store_coin07.webp','UI_Store_hot_sale.webp','','coins_03','COINS_PRICE_IOS_03','PRICE_OFF','');
INSERT INTO "StoreInfo" VALUES(4,4700,0,'','','UI_Store_coin08.webp','','','coins_04','COINS_PRICE_IOS_04','','');

CREATE TABLE LOWInfo ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "Storyline" INTEGER NOT NULL UNIQUE DEFAULT -1, "PackName" TEXT NOT NULL DEFAULT 'LP_', "DisplayOrder" INTEGER NOT NULL DEFAULT -1, "LevelName" TEXT NOT NULL DEFAULT 'LN_', "DownloadDate" DATETIME NOT NULL DEFAULT '1000-01-01 00:00:00', "PortalTexture" TEXT DEFAULT '', "PlayButtonTexture" TEXT DEFAULT '', "BannerTexture" TEXT DEFAULT '', levelStoryline INTEGER);
INSERT INTO "LOWInfo" VALUES(1,1,'LP_LOW',0,'LN_WESLIE_LOW','2013-06-10 00:00:00','WMW_UI_low_swampy.png','WMW_UI_low_swampy_level_icon_%d.png','WMW_UI_low_swampy_banner.png',1);
INSERT INTO "LOWInfo" VALUES(2,2,'LP_LOW',1,'LN_WOLF_LOW','2013-06-10 00:00:00','WMW_UI_low_cranky.png','WMW_UI_low_cranky_level_icon_%d.png','WMW_UI_low_cranky_banner.png',2);

DELETE FROM sqlite_sequence;
INSERT INTO "sqlite_sequence" VALUES('PlayerData',14);
INSERT INTO "sqlite_sequence" VALUES('Settings',32);

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
