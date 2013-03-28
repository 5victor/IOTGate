#ifndef DEBUG_H_
#define DEBUG_H_

#define LOG_TAG "IOTGateServer"

#define LOG_NDEBUG 0
#define LOG_NDEBUG_FUNCTION

#ifndef LOG_NDEBUG_FUNCTION
#define D(...) ((void)0)
#else
#define D(...) (ALOGV(__VA_ARGS__))
#endif

#include <cutils/log.h>

#endif
