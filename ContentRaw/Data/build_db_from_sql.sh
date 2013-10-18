#!/bin/sh

#  Script.sh
#  WalaberEngine
#
#  Created by dongshan on 13-1-31.
#
echo $1
echo $2
rm -f $2/*.db
for i in `find $1 -iname "*.sql" -depth 1`; do
BASE=`basename $i`
sqlite3 $2/${BASE%.*}.db ".read $i"
done