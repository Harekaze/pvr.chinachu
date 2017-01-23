LOCAL_PATH := $(call my-dir)/..

## chinachu
include $(CLEAR_VARS)

LOCAL_MODULE      := chinachu

LOCAL_SRC_FILES   := $(wildcard src/chinachu/*.cpp)
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)

## pvr_client
include $(CLEAR_VARS)

LOCAL_MODULE      := pvr.chinachu

LOCAL_SRC_FILES   := $(wildcard src/pvr_client/*.cpp)
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/include $(LOCAL_PATH)/src
LOCAL_STATIC_LIBRARIES := chinachu

include $(BUILD_SHARED_LIBRARY)
