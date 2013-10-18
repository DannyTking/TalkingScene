#!/bin/sh

#  Script.sh
#  WalaberEngine
#
#  Created by dongshan on 13-1-31.
#
echo $1
echo $2
sqlite3 $2/perry.db ".read $1/perry_android_del_weibo.sql"