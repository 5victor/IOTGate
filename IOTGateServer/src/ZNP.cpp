/*
 * ZNP.cpp
 *
 *  Created on: 2013-3-5
 *      Author: victor
 */

#include "ZNP.h"
#include <fcntl.h>
#include <stdlib.h>

Mutex *ZNP::mutex;
Mutex *ZNP::mutexwait;
Condition *ZNP::condwait;
int ZNP::cmd0;
int ZNP::cmd1;
FRAME *ZNP::waitframe;
Server *ZNP::server;

ZNP::ZNP() {
	// TODO Auto-generated constructor stub
	mutex = new Mutex(Mutex::SHARED);
	mutexwait = new Mutex(Mutex::SHARED);
	condwait = new Condition(Condition::SHARED);
	cmd0 = 0;
	cmd1 = 0;
}

ZNP::~ZNP() {
	// TODO Auto-generated destructor stub
}

int ZNP::initZNP(Server *server)
{
	mt = new MT();
	mt->setAREQHandle(&ZNP::handleAREQ);
	this->server = server;
	return mt->start();
}

FRAME *ZNP::waitAREQ(int cmd0, int cmd1)
{
	mutex->lock();
	this->cmd0 = cmd0;
	this->cmd1 = cmd1;

	D("%s", __FUNCTION__);
	Mutex::Autolock _l(*mutexwait);
	condwait->wait(*mutexwait);
	D("%s:waitAREQ Success", __FUNCTION__);

	this->cmd0 = 0;
	this->cmd1 = 0;

	mutex->unlock();
	return waitframe;
}

FRAME *ZNP::waitAREQRelative(int cmd0, int cmd1, nsecs_t reltime)
{
	int ret;
	mutex->lock();
	this->cmd0 = cmd0;
	this->cmd1 = cmd1;

	D("%s", __FUNCTION__);
	Mutex::Autolock _l(*mutexwait);
	ret = condwait->waitRelative(*mutexwait, reltime);

	D("%s:waitAREQ Success", __FUNCTION__);

	this->cmd0 = 0;
	this->cmd1 = 0;

	mutex->unlock();
	return ret ? NULL : waitframe;
}


void ZNP::setINDICATEhandle(INDICATE handle)
{
	indicate = handle;
}

void ZNP::handleAREQ(FRAME *frame)
{
	D("%s", __FUNCTION__);

	if ((frame->cmd0 == cmd0) && (frame->cmd1 == cmd1)) {
		ZNP::waitframe = frame;
		condwait->signal();
		return ;
	}
	/*
	 * code above use the waitAREQ
	 *
	 * 有的AREQ被上面的代码wait掉了，想像这样一种情况，
	 * 发现网络的设备的一个机制是使用ZDO_IEEE_ADDR_REQ
	 * 但是如果我想真实的获取其IEEE ADDR呢，那么这个ZDO_IEEE_ADDR_RSP
	 * 就要被wait掉，不能走下面代码，走下面代码就是发现网络中设备的实现了
	 *
	 */

	int cmd0 = frame->cmd0 & 0xF;

	D("%s:cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
	if (cmd0 == 0x01) {/* SYS interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x02) {/* MAC interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x03) {/* NWK interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x04) {/* AF interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x05) {/* ZDO interface */
		handleAREQZDO(frame);
	} else if (cmd0 == 0x06) {/* SAPI interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x07) {/* UTIL interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x08) {/* DEBUG interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else if (cmd0 == 0x09) {/* APP interface */
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
	} else {
		D("recv error frame");
	}

	//if (frame->len)
		//delete frame->data;
	//delete frame;
}

void ZNP::handleAREQZDO(FRAME *frame)
{
	D("%s:cmd0=0x%x, cmd1=0x%x, len=%d", __FUNCTION__, frame->cmd0, frame->cmd1, frame->len);
	switch(frame->cmd1) {
	case 0x81:
		server->foundNode(*((uint16_t *)&frame->data[9]));
		freeFrame(frame);
		break;
	case 0xc1:
		D("srcaddr:0x%x", *(uint16_t *)frame->data);
		server->foundNode(*((uint16_t *)&frame->data[2]));
		freeFrame(frame);
		break;
	}
}

FRAME *ZNP::sendSREQ(int cmd0, int cmd1)
{
	FRAME *result;
	FRAME *frame = new FRAME();
	frame->len = 0;
	frame->cmd0 = cmd0;
	frame->cmd1 = cmd1;

	result = mt->sendSREQ(frame);

	delete frame;
	return result;
}

FRAME *ZNP::sendSREQ(int cmd0, int cmd1, int len, uint8_t *buf)
{
	FRAME *result;
	FRAME *frame = new FRAME();
	frame->len = len;
	frame->cmd0 = cmd0;
	frame->cmd1 = cmd1;
	frame->data = buf;
	result = mt->sendSREQ(frame);

	//delete frame->data;
	delete frame;
	return result;
}


uint8_t ZNP::getRet1Byte(FRAME *frame)
{
	return *((uint8_t *)(&frame->data[0]));
}

uint16_t ZNP::getRet2Byte(FRAME *frame)
{
	return *((uint16_t *)(&frame->data[0]));
}

int ZNP::sendAREQ(int cmd0, int cmd1)
{
	int ret;
	FRAME *frame = new FRAME();
	frame->len = 0;
	frame->cmd0 = cmd0;
	frame->cmd1 = cmd1;

	ret = mt->sendAREQ(frame);
	delete frame;
	return ret;
}

///////////////////////////////////////////////////////////////////////

int ZNP::SYS_RESET_REQ()
{
	return sendAREQ(0x41, 0x00);
}

int ZNP::SYS_PING()
{
	int ret;
	FRAME *result;

	result = sendSREQ(0x21, 0x01);
	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

int ZNP::ZB_WRITE_CONFIGURATION(uint8_t configID, int len, uint8_t *buf)
{
	FRAME *result;
	int ret;

	uint8_t *data = new uint8_t[len+2];

	data[0] = configID;
	data[1] = len;
	memcpy(&data[2], buf, len);
	result = sendSREQ(0x26, 0x05, len+2, data);
	delete data;

	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

int ZNP::ZB_START_REQUEST()
{
	FRAME *result;
	int ret;

	result = sendSREQ(0x26, 0x00);

	return !result;
}

int ZNP::ZDO_IEEE_ADDR_REQ(uint16_t shortaddr, uint8_t type, uint8_t index)
{
	FRAME *result;
	int ret;
	uint8_t *data;

	data = new uint8_t[4];

	((uint16_t *)data)[0] = shortaddr;
	data[2] = type;
	data[3] = index;

	result = sendSREQ(0x25, 0x01, 4, data);
	delete data;

	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

int ZNP::ZDO_NODE_DESC_REQ(uint16_t nwkaddr)
{
	uint16_t data[2];
	FRAME *result;
	int ret;

	data[0] = nwkaddr;
	data[1] = nwkaddr;

	result = sendSREQ(0x25, 0x02, 4, (uint8_t *)data);

	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

int ZNP::ZDO_ACTIVE_EP_REQ(uint16_t nwkaddr)
{
	uint16_t data[2];
	FRAME *result;
	int ret;

	data[0] = nwkaddr;
	data[1] = nwkaddr;

	result = sendSREQ(0x25, 0x05, 4, (uint8_t *)data);
	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

int ZNP::ZDO_SIMPLE_DESC_REQ(uint16_t nwkaddr, uint8_t endpoint)
{
	uint8_t data[5];
	uint16_t *wdata = (uint16_t *)data;
	FRAME *result;
	int ret;

	wdata[0] = nwkaddr;
	wdata[1] = nwkaddr;
	data[4] = endpoint;

	result = sendSREQ(0x25, 0x4, 5, data);
	ret = getRet1Byte(result);
	freeFrame(result);
	return ret;
}

