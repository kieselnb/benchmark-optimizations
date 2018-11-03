#!/bin/sh

ITERATIONS=${1:-1}
CONV=$(which convert)
ORIGINAL=../original.png
OUT=image.png
TESTSFILE=../test-sizes.txt

while IFS='' read -r line || [[ -n "$line" ]]; do
    echo ${CONV} ${ORIGINAL} -resize $line ${OUT}
    ${CONV} ${ORIGINAL} -resize $line ${OUT}
    ./blur ${ITERATIONS}
    echo
done < ${TESTSFILE}
