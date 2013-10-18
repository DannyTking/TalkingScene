BEGIN;
--修改 IAP 金币的ID
--删除 IAP原有金币
Delete FROM "IAPInfo" Where Internal like '%coins%';
--添加 IAP 金币
--insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_01','com.ath.wmxyy.coins_01','2206674', '2206800','0000101');
--insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_02','com.ath.wmxyy.coins_02','2206675', '2206801','0000102');
insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_03','com.ath.wmxyy.coins_03','2207030', '2207055','0000103');
--insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_04','com.ath.wmxyy.coins_04','2207031', '2207056','0000104');
--insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_05','com.ath.wmxyy.coins_05','2206674', '3100005','0000105');
insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_06','com.ath.wmxyy.coins_06','2206674', '3100006','0000106');
insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_07','com.ath.wmxyy.coins_07','2206674', '3100007','0000107');
insert into "IAPInfo" ("Internal","IOS","Google","Amazon","ChuKong") values ('coins_08','com.ath.wmxyy.coins_08','2206674', '3100008','0000108');
--清空商店金币
Delete FROM "StoreInfo";
--INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(1,300,'UI_Store_coin01.webp','coins_01','COINS_PRICE_ANDROID_01');
--INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(2,450,'UI_Store_coin02.webp','coins_02','COINS_PRICE_ANDROID_02');
INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(1,770,'UI_Store_coin03.webp','coins_03','COINS_PRICE_ANDROID_03');
--INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(4,950,'UI_Store_coin04.webp','coins_04','COINS_PRICE_ANDROID_04');
--INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(5,1300,'UI_Store_coin05.webp','coins_05','COINS_PRICE_ANDROID_05');
INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(2,1650,'UI_Store_coin06.webp','coins_06','COINS_PRICE_ANDROID_06');
INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(3,2500,'UI_Store_coin07.webp','coins_07','COINS_PRICE_ANDROID_07');
INSERT INTO "StoreInfo" ("ID","Coins","Coins_Icon","IAP_item_id","Coins_Price") VALUES(4,3450,'UI_Store_coin08.webp','coins_08','COINS_PRICE_ANDROID_08');
--第一个作为默认复制
update "StoreInfo" set "Texture_Normal"='UI_Store_btn_price_normal.webp',"Texture_Pressed"='UI_Store_btn_price_pressed.webp' where ID='1';
--第二个作为标签高亮
update "StoreInfo" set "Texture_Normal"='UI_Store_btn_price_classic_normal.webp',"Texture_Pressed"='UI_Store_btn_price_classic_pressed.webp',"Hot_Icon"='UI_Store_hot_sale.webp',"Hot_Label"='PRICE_OFF' where ID='2';
COMMIT;
