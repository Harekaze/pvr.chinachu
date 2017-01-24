#!/bin/sh

ADDONNAME=libpvr.chinachu.so

if [ $# -eq 0 ]; then
	echo "usage: $0 APP_ABI=armeabi-v7a"
	exit 1
fi
export "$1"
if [ -z ${APP_ABI+x} ]; then
	echo "APP_ABI is unset"
	exit 1
fi
if [ ! -e libs/${APP_ABI}/${ADDONNAME} ]; then
	echo "build first."
	exit 1
fi

set -eux
which zip
cp -r template/pvr.chinachu .
cp ChangeLog.txt pvr.chinachu/
cp LICENSE pvr.chinachu/
cp libs/${APP_ABI}/${ADDONNAME} pvr.chinachu/${ADDONNAME}
zip -9 -r ./pvr.chinachu.zip pvr.chinachu
rm -rf pvr.chinachu
