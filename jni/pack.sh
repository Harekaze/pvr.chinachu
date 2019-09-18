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
mkdir -p dist
cp -r template/pvr.chinachu dist/
cp ChangeLog.txt dist/pvr.chinachu/
cp LICENSE dist/pvr.chinachu/
cp libs/${APP_ABI}/${ADDONNAME} dist/pvr.chinachu/${ADDONNAME}
cd dist; zip -9 -r ../pvr.chinachu.zip pvr.chinachu
