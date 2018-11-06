#!/bin/sh

ITERATIONS=${1:-100}
CONV=$(which convert)
ORIGINAL=../original.png
OUTIMAGE=image.png
RESULTS=cycles.txt
TESTSFILE=../test-sizes.txt

# clear out old results if they exist
if [ -f ${RESULTS} ]
then
    rm ${RESULTS}
fi

touch ${RESULTS}

while IFS='' read -r line || [[ -n "$line" ]]; do
    echo ${CONV} ${ORIGINAL} -resize $line\! ${OUTIMAGE}
    ${CONV} ${ORIGINAL} -resize $line\! ${OUTIMAGE}
    ./blur ${ITERATIONS} >> ${RESULTS}
    echo
done < ${TESTSFILE}
