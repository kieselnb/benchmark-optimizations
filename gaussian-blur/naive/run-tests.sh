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

while read width height filter; do
    convert="${CONV} ${ORIGINAL} -resize "$width"x"$height"! ${OUTIMAGE}"
    echo $convert
    $convert
    blur="./blur ${ITERATIONS} ${filter}"
    echo $blur
    $blur >> ${RESULTS}
    echo
done < ${TESTSFILE}
