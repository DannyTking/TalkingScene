cd "`dirname "$0"`"

for i in `find ./ -iname "*.db" -depth 1`; do

	BASE=`basename $i`
	echo "dumping database into sql:" ${BASE%.*}.sql
	
	sqlite3 $i ".dump" > ./${BASE%.*}.sql	
done

rm -f *.db