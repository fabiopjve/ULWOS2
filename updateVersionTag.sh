#!/bin/bash
if [ $# -ne 2 ]; then
	echo "Error: you must define the version tag (x.x.x) and description"
	echo "Usage: updateVersionTag.sh VERSION DESCRIPTION"
	echo "Example: updateVersionTag.sh 1.0.0 'new version'"
	exit 1
fi

echo $1
echo $2

echo "name=ULWOS2" > temp.txt
echo "version=$1" >> temp.txt
echo "author=Fábio Pereira <fabio@embeddedsystems.io>" >> temp.txt
echo "maintainer=Fábio Pereira <fabio@embeddedsystems.io>" >> temp.txt
echo "sentence=ULWOS2 - An Ultra LightWeight Operating System" >> temp.txt
echo "paragraph=ULWOS2 is a cooperative thread scheduler 100% written in C which can run on any platform supported by GCC. You can use it to run multiple threads at the same time in a cooperative fashion" >> temp.txt
echo "category=Other" >> temp.txt
echo "url=https://github.com/fabiopjve/ULWOS2" >> temp.txt
echo "architectures=*" >> temp.txt
echo "includes=ULWOS2.h" >> temp.txt

rm library.properties
mv temp.txt library.properties

git add library.properties
git commit -m "$2"
git tag -a v$1 -m "$2"
git push
git push origin v$1