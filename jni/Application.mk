APP_PROJECT_PATH := $(NDK_PROJECT_PATH)
APP_CPPFLAGS     := -Werror -std=c++11 -frtti -fexceptions -Wall -DANDROID -DPICOJSON_USE_LOCALE=0 -DPICOJSON_USE_RVALUE_REFERENCE=0
APP_STL          := c++_static
APP_PLATFORM     := android-16
APP_OPTIM        := release
APP_ABI          := armeabi-v7a
