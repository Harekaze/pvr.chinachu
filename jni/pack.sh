#!/bin/sh

ADDONNAME=libpvr.chinachu.so

if [ -z ${VERSION+x} ]; then
	echo "VERSION is unset"
	exit 1
fi
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
cat template/pvr.chinachu/addon.xml | sed "s/VERSION/${VERSION}/g" > pvr.chinachu/addon.xml
cp libs/${APP_ABI}/${ADDONNAME} pvr.chinachu/${ADDONNAME}
zip -9 -r ./pvr.chinachu.${VERSION}.zip pvr.chinachu
rm -rf pvr.chinachu
