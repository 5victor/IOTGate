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

int ZNP::initZNP()
{
	mt = new MT();
	mt->setAREQHandle(&ZNP::handleAREQ);
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

void ZNP::setINDICATEhandle(INDICATE handle)
{
	indicate = handle;
}

int ZNP::SYS_PING()
{
	int ret;
	FRAME *result;
	FRAME *frame = new FRAME();
	frame->len = 0;
	frame->cmd0 = 0x21;
	frame->cmd1 = 0x01;

	result = mt->sendSREQ(frame);
	ret = ((short *)result->data)[0];
	delete result->data;
	delete result;
	return ret;
}

int ZNP::ZB_WRITE_CONFIGURATION(unsigned char configID, int len, unsigned char *buf)
{
	FRAME *result;
	FRAME *frame = new FRAME();
	int ret;

	frame->len = len + 2;
	frame->cmd0 = 0x26;
	frame->cmd1 = 0x05;
	frame->data = (unsigned char*)malloc(frame->len);
	frame->data[0] = configID;
	frame->data[1] = len;
	memcpy(&frame->data[2], buf, len);

	//delete buf; may be: int a; &a;
	result = mt->sendSREQ(frame);
	ret = ((unsigned char *)result->data)[0];
	delete result->data;
	delete result;
	return ret;
}

int ZNP::ZB_START_REQUEST()
{
	FRAME *frame = new FRAME();
	FRAME *result;
	int ret;

	frame->len = 0;
	frame->cmd0 = 0x26;
	frame->cmd1 = 0x00;

	result = mt->sendSREQ(frame);
	ret = ((unsigned char *)result->data)[0];
	delete result->data;
	delete result;
	return ret;
}

int ZNP::ZDO_IEEE_ADDR_REQ(unsigned short shortaddr, unsigned char type, unsigned char index)
{
	FRAME *frame = new FRAME();
	FRAME *result;
	int ret;

	frame->len = 4;
	frame->cmd0 = 0x25;
	frame->cmd1 = 0x01;
	frame->data = new unsigned char[4];
	((unsigned short *)frame->data)[0] = shortaddr;
	frame->data[2] = type;
	frame->data[3] = index;

	result = mt->sendSREQ(frame);
	ret = ((unsigned char *)result->data)[0];
	delete result->data;
	delete result;
	return ret;
}

void ZNP::handleAREQ(FRAME *frame)
{
	D("%s", __FUNCTION__);

	if ((frame->cmd0 == cmd0) && (frame->cmd1 == cmd1)) {
		ZNP::waitframe = frame;
		condwait->signal();
		return ;
	}

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
		switch (frame->cmd1) {
		default:
			D("%s:unprocess cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
			break;
		}
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

	if (frame->len)
		delete frame->data;
	delete frame;
}
