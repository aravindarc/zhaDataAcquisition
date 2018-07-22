#!/bin/sh

COUNT=247

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

a=0
while [ -f "RawImages/$a.jpg" ]
do
    rm -rf "ResultantStorage/$a"
    mkdir "ResultantStorage/$a"
    b=$(./DataAcquisition "RawImages/$a.jpg" "ResultantStorage/$a/")
    if [ $b -eq $COUNT ]
    then
        echo "${GREEN}Extracted character images from ${NC}$a.jpg"
    else
        rm -rf "ResultantStorage/$a"
        echo "${RED}Error extracting from ${NC}$a.jpg"
    fi
    a=`expr $a + 1`
done
