BEGIN;
--将除 PACk1 10关开始设为不可用 和 需要IAP购买
update "LevelInfo" set "Available"=0,"Type"=1 where ID >= 10 AND ID <=59;
--添加 IAP ChuKong列
--alter table "IAPInfo" ADD "ChuKong" TEXT;
--添加 IAP 解锁xyy01
insert into "IAPInfo" ("Internal","iOS","Google","Amazon","ChuKong") values ('xyy01','com.ath.wmxyy.xyy_full_version','com.punchboxusa.xiyangyangxiaowanpiwanzhengban1.m.iap.tier1','2206800','0000001');
--IAP的IP决定解锁的Pack IAP_TEXT 是未解开包的
update "LevelPackInfo" set "IAP_item_id"='xyy01' ,"IAP_Text"='PURCHASE_PACK', "Bought"=0 where ID >= 10 AND ID <=13;
--HubInfo IAP
update "HubInfo" set "IAP_item_id"='xyy01' , "Bought"=0 where Storyline='1000';
--添加 Xyy01 解锁记录
insert into "PlayerData" ("EventName","EventValue") values ('XyyFullVersionIAPState','0');

--第二个作为标签高亮
update "StoreInfo" set "Texture_Normal"='UI_Store_btn_price_classic_normal.webp',"Texture_Pressed"='UI_Store_btn_price_classic_pressed.webp',"Hot_Icon"='UI_Store_hot_sale.webp',"Hot_Label"='PRICE_OFF' where ID='2';
--第三个去除高亮
update "StoreInfo" set "Texture_Normal"='',"Texture_Pressed"='',"Hot_Icon"='',"Hot_Label"='' where ID='3';

COMMIT;
