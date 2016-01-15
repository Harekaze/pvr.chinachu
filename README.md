# Chinachu client for Kodi/XBMC
Chinachu PVR addon for Kodi
[![Build status: master](https://travis-ci.org/mzyy94/pvr.chinachu.svg?branch=master)](https://travis-ci.org/mzyy94/pvr.chinachu/)

![icon](/template/pvr.chinachu/icon.png)

## Supported environment

### Backend
Chinachu tree [5c39543657](https://github.com/kanreisa/Chinachu/tree/5c395436575f266810a40cc475c436bff884f792)

### Frontend
- Kodi **15.x** Isengard
  + for OS X 64bit
  + for Android ARM
  + for Linux amd64/i386/armv7
  + for Windows

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
