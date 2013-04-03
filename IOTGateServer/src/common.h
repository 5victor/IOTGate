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

#endif /* COMMON_H_ */
