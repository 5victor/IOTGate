/*
 * common.h
 *
 *  Created on: Mar 27, 2013
 *      Author: victor
 */

#ifndef COMMON_H_
#define COMMON_H_

struct FRAME {
	unsigned char len; /* length of data */
	unsigned char cmd0;
	unsigned char cmd1;
	unsigned char *data;
};

typedef void (*AREQHANDLE)(FRAME *frame);

void inline freeFrame(FRAME *frame)
{
	delete frame->data;
	delete frame;
}

/////////////////////////////////////////////////////////////

#define LOG_TAG "IOTGateServer"

#define LOG_NDEBUG 0

#define LOG_NDEBUG_FUNCTION

#ifndef LOG_NDEBUG_FUNCTION
#define D(...) ((void)0)
#else
#define D(...) (ALOGV(__VA_ARGS__))
#endif

#define LOG(...) (ALOGV(__VA_ARGS__))

#include <cutils/log.h>

/////////////////////////////////////////////////////////////

#define MAX_CLUSTER 32

#endif /* COMMON_H_ */
