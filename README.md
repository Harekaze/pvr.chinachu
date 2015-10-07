# Chinachu client for Kodi/XBMC
Chinachu PVR addon for Kodi
[![Build status: master](https://travis-ci.org/mzyy94/pvr.chinachu.svg?branch=2.0.0-dev)](https://travis-ci.org/mzyy94/pvr.chinachu/)

![icon](/template/pvr.chinachu/icon.png)

## Supported environment

## Backend
Chinachu devel-beta **NOT TESTED**

## Frontend
- Kodi **16.x** Jarvis
  + for OS X 64bit
  + for Android ARM
  + for Linux amd64/i386/armv7
  
## Latest release

[pvr.chinachu/releases](https://github.com/mzyy94/pvr.chinachu/releases)

## Building from source

### Linux / OS X
```sh
$ ./bootstrap
$ ./configure
$ make
$ ls pvr.chinachu.*.zip
```
> TIPS: If a warning about AC_CHECK_HEADER_STDBOOL has occurred, install gnulib and execute bootstrap with
> AUTORECONF_FLAGS option with gnulib's m4 directory (e.g. `AUTORECONF_FLAGS=-I/usr/share/gnulib/m4 ./bootstrap`)

### Android ARM
*Android NDK is required.*

```sh
$ ./bootstrap
$ ./configure --host=arm-linux-androideabi
$ make
$ ls pvr.chinachu.*.zip
```

## Installing

1. Build or download the appropriate version for your platform. 
2. Launch Kodi.
3. Navigate to System -> Add-ons -> Install from zip file
4. Select the zip file which you get in first step.

> NOTE: In some cases, Kodi for android installs addons into non-executable device. As a result, you need to do something more.
> See [wiki/android-installation](https://github.com/mzyy94/pvr.chinachu/wiki/android-installation).

## Configuration

See [wiki/configuration](https://github.com/mzyy94/pvr.chinachu/wiki/configuration)

## Contribution

See [wiki/contribution](https://github.com/mzyy94/pvr.chinachu/wiki/contribution)

## License

GPLv3
