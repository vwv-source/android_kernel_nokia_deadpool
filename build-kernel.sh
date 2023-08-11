#!/bin/bash

OUTDIR=../out
CROSS_COMPILE=$(pwd)/aarch64-linux-android-4.9/bin/aarch64-linux-android-
ARCH=arm64
DEFCONFIG=msmCustom_defconfig



clean(){
rm -rf $OUTDIR
}

build(){

mkdir -p $OUTDIR
if test -f $OUTDIR/.config
then
	make O=$OUTDIR ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE 2>&1 | tee kernel.log
else
	make O=$OUTDIR ARCH=$ARCH $DEFCONFIG
	nano $OUTDIR/.config
	make O=$OUTDIR ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE 2>&1 | tee kernel.log

fi

}

help(){

echo -e "\nA simple build script to compile Venus kernel\n"
echo -e "Syntax: build-kernel.sh [help|clean|build]\n"
echo -e "Options:\n"
echo -e "build  -   Build the target kernel\n"
echo -e "clean  -   Clean the working directory\n"
echo -e "help   -    Display help\n"

}

while [ True ];
do
	if [ "$1" == "help" ];
	then
		help
		break
	elif [ "$1" == "build" ];
	then
		build
		break
	elif [ "$1" == "clean" ];
    then
		clean
		break
	else
		help
		break
	fi

done


