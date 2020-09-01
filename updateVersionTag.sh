#!/bin/bash
if [ $# -ne 2 ]; then
	echo "Error: you must define the version tag (x.x.x) and description"
	echo "Usage: updateVersionTag.sh VERSION DESCRIPTION"
	echo "Example: updateVersionTag.sh 1.0.0 'new version'"
	exit 1
fi

echo $1
echo $2

python3 updateLibraryVersion.py $1

rm library.properties
rm library.json
mv out.txt library.properties
mv out2.txt library.json

git add library.properties
git add library.json
git commit -m "$2"
git tag -a v$1 -m "$2"
git push
git push origin v$1