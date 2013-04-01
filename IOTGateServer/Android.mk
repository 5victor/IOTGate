LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/MT.cpp	\
	src/ZNP.cpp	\
	src/Server.cpp	\
	src/main.cpp

LOCAL_C_INCLUDES += \
	bionic \
	bionic/libstdc++/include \
	external/stlport/stlport

LOCAL_SHARED_LIBRARIES := liblog libcutils libstlport libutils libbinder

LOCAL_MODULE := iotgateserver
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
