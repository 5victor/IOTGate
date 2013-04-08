LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/MT.cpp	\
	src/ZNP.cpp	\
	src/Server.cpp	\
	src/Node.cpp	\
	src/Endpoint.cpp	\
	src/SocketServer.cpp	\
	src/SocketSession.cpp	\
	src/main.cpp

LOCAL_C_INCLUDES += \
	bionic \
	bionic/libstdc++/include \
	external/stlport/stlport \
	external/openssl/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libstlport libutils libbinder libopenssl

LOCAL_MODULE := iotgateserver
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
