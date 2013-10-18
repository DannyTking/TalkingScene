cd "`dirname "$0"`"

rm -f *.db

for i in `find ./ -iname "*.sql" -depth 1`; do

	BASE=`basename $i`
	echo "compiling database into binary:" $BASE
	
	sqlite3 ${BASE%.*}.db ".read $i"
	
done

