LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/MT.cpp	\
	src/ZNP.cpp	\
	src/main.cpp

LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libbinder

LOCAL_MODULE := iotgateserver
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
