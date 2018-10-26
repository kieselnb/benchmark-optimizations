#!/bin/sh

# get directory of script - we know it's in the right place
DIR=$(readlink -f "$(dirname $0)")

MACHINE=ece001.ece.local.cmu.edu

# get username as command-line arg
: ${ECEUSER:=$1}
if [ -z "$ECEUSER" ]
then
    echo Please specify a user on the command line or with env variable ECEUSER
    exit
fi

rsync -avz $DIR $ECEUSER@$MACHINE:~/Private/ --cvs-exclude
