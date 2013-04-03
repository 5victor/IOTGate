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
#include "Server.h"

class Server;

class ZNP {
public:
	ZNP();
	virtual ~ZNP();
	int initZNP(Server *server);
	void setINDICATEhandle(INDICATE handle);
	FRAME *waitAREQ(int cmd0, int cmd1);
	FRAME *waitAREQRelative(int cmd0, int cmd1, nsecs_t reltime);

private:
	FRAME *sendSREQ(int cmd0, int cmd1);
	// doesn't free buf
	FRAME *sendSREQ(int cmd0, int cmd1, int len, uint8_t *buf);
	uint8_t getRet1Byte(FRAME *frame);
	uint16_t getRet2Byte(FRAME *frame);
	int sendAREQ(int cmd0, int cmd1);

public:
	//MT_SYS
	int SYS_PING();
	int SYS_RESET_REQ();


public:
	//MT_SAPI
	int ZB_START_REQUEST();

	int ZB_WRITE_CONFIGURATION(uint8_t configID, int len, uint8_t *buf);

public:
	//MT_ZDO
	int ZDO_IEEE_ADDR_REQ(uint16_t shortaddr, uint8_t type, uint8_t index);
	int ZDO_NODE_DESC_REQ(uint16_t nwkaddr);
	int ZDO_ACTIVE_EP_REQ(uint16_t nwkaddr);
	int ZDO_SIMPLE_DESC_REQ(uint16_t nwkaddr, uint8_t endpoint);


private:
	MT *mt;
	INDICATE indicate;
	static Server *server;

private:
	static void handleAREQ(FRAME *frame);
	static void handleAREQZDO(FRAME *frame);

private:
	static Mutex *mutex;
	static Mutex *mutexwait;
	static Condition *condwait;
	static int cmd0;
	static int cmd1;
	static FRAME *waitframe;
};

#endif /* ZNP_H_ */
