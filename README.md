# Harekaze for Kodi/XBMC
Chinachu PVR addon for Kodi

[![Build status: master](https://img.shields.io/travis/Harekaze/pvr.chinachu/master.svg?maxAge=259200&style=flat-square)](https://travis-ci.org/Harekaze/pvr.chinachu/)
[![GitHub release](https://img.shields.io/github/release/Harekaze/pvr.chinachu.svg?maxAge=259200&style=flat-square)](https://github.com/Harekaze/pvr.chinachu/releases)
[![GitHub release downloads](https://img.shields.io/github/downloads/Harekaze/pvr.chinachu/total.svg?style=flat-square)](https://github.com/Harekaze/pvr.chinachu/releases)
[![GitHub issues](https://img.shields.io/github/issues/Harekaze/pvr.chinachu.svg?style=flat-square)](https://github.com/Harekaze/pvr.chinachu/issues)
[![GitHub stars](https://img.shields.io/github/stars/Harekaze/pvr.chinachu.svg?style=flat-square)](https://github.com/Harekaze/pvr.chinachu/stargazers)
[![GitHub license](https://img.shields.io/badge/license-GPLv3-orange.svg?style=flat-square)](https://raw.githubusercontent.com/Harekaze/pvr.chinachu/master/LICENSE)

![icon](/template/pvr.chinachu/icon.png)

## Supported environment

## Backend
Chinachu tree [5981ebb0c9](https://github.com/kanreisa/Chinachu/tree/5981ebb0c9d01330da08f83dd828459614caf6d8)

## Frontend
- Kodi **16.x** Jarvis
  + for OS X 64bit
  + for Android ARM
  + for Linux amd64/i386/armv7
  + for Windows

## Latest release

[pvr.chinachu/releases](https://github.com/Harekaze/pvr.chinachu/releases)

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
$ make CPPFLAGS=-DPICOJSON_USE_LOCALE=0
$ ls pvr.chinachu.*.zip
```

### Windows

Requirements:
- Visual Studio 2015
- PowerShell v5

```powershell
> ./build.ps1
> ls ./pvr.chinachu.*.zip
```
> TIPS: If a powershell warning about Execution Policies has occurred, run `Set-ExecutionPolocy Unrestricted`
> with Administrator privileges. After building this project, run `Set-ExecutionPolocy RemoteSigned`, please.

> NOTE: PowerShell command 'Compress-Archive' creates broken zip file.
> Please unzip created archive yourself, and re-zip it with other compression tool.

## Installing

1. Build or download the appropriate version for your platform.
2. Launch Kodi.
3. Navigate to System -> Add-ons -> Install from zip file
4. Select the zip file which you get in first step.

> NOTE: In some cases, Kodi for android installs addons into non-executable device. As a result, you need to do something more.
> See [wiki/android-installation](https://github.com/Harekaze/pvr.chinachu/wiki/android-installation).

## Configuration

See [wiki/configuration](https://github.com/Harekaze/pvr.chinachu/wiki/configuration)

## Contribution

See [wiki/contribution](https://github.com/Harekaze/pvr.chinachu/wiki/contribution)

## License

GPLv3
