BEGIN;


delete from "Achievements" where ID = "ACH_SHARE_WEIBO";

update "Settings" set "Value"=0  where ID=35;

COMMIT;
