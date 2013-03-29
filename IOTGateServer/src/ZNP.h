/*
 * ZNP.h
 *
 *  Created on: 2013-3-5
 *      Author: victor
 */

#ifndef ZNP_H_
#define ZNP_H_

#include <utils/Thread.h>
using namespace android;

#include "MT.h"
#include "common.h"
#include "debug.h"
#include "indicate.h"

class ZNP {
public:
	ZNP();
	virtual ~ZNP();
	int initZNP();
	void setINDICATEhandle(INDICATE handle);

public:
	//MT_SYS
	int SYS_PING();

public:
	//MT_SAPI
	int ZB_START_REQUEST();

	int ZB_WRITE_CONFIGURATION(unsigned char configID, int len, unsigned char *buf);

private:
	MT *mt;
	INDICATE indicate;

private:
	static void handleAREQ(FRAME *frame);
};

#endif /* ZNP_H_ */
