#!/bin/sh

# get directory of script - we know it's in the right place
DIR=$(readlink -f "$(dirname $0)")

MACHINE=ece000.ece.local.cmu.edu

# get username as command-line arg
USER=$1

rsync -avz $DIR $USER@$MACHINE:Private/ --cvs-exclude
