#!/bin/bash
cd ./include
includes=$(ls -l *.h | awk '{printf("%s#", $9)}')
cd ../src
srcs=$(ls -l *.c | awk '{printf("%s#", $9)}')
objs=$(sed 's/\.c/\.o/g' <<< $srcs)

cd ..
includes="_DEPS#=#$includes"
objs="_OBJ#=#$objs"

sed -e 13a'\'$includes -e 15a'\'$objs Makefile_template > Makefile
sed -e 's/#/ /g' -i Makefile

if [[ $1 = "-c" || $1 = "--clean" || $2 = "-c" || $2 = "--clean" ]] 
then
    make clean
fi
make clox

if [[ $1 = "-r" || $1 = "--run" || $2 = "-r" || $2 = "--run" ]] 
then
    echo \n
    ./target/clox
fi