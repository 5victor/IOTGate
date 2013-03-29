/*
 * areq.h
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#ifndef INDICATE_H_
#define INDICATE_H_

enum areq_type {
	ZB_START_CONFIRM,


};

struct indicate {
	areq_type type;
	union {
		unsigned char data;
		unsigned char *buf;
	};
};

typedef void (*INDICATE)(struct indicate *msg);

#endif /* INDICATE_H_ */
