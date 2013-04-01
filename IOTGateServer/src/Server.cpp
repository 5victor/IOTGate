/*
 * Server.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#include "Server.h"

Server::Server() {
	// TODO Auto-generated constructor stub

}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

int Server::initServer()
{
	znp = new ZNP();
	return znp->initZNP();
}

int Server::startNetwork(int panid, unsigned int chanlist)
{
	FRAME *result;
	int ret;
	int arg;

	//ZCD_NV_LOGICAL_TYPE
	arg = 0; //COORDINATOR(0x00), ROUTER (0x01) or ENDDEVICE (0x02)
	ret = znp->ZB_WRITE_CONFIGURATION(0x87, 1, (unsigned char *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_LOGICAL_TYPE fail %d", ret);
		return ret;
	}

	//ZCD_NV_PANID
	arg = panid;
	ret = znp->ZB_WRITE_CONFIGURATION(0x83, 2, (unsigned char *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_PANID fail %d", ret);
		return ret;
	}

	//ZCD_NV_CHANLIST
	arg = chanlist; //bitmask
	ret = znp->ZB_WRITE_CONFIGURATION(0x84, 4, (unsigned char *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_CHANLIST fail %d", ret);
		return ret;
	}

	ret = znp->ZB_START_REQUEST();

	if (ret) {
		D("ZB_START_REQUEST fail %d", ret);
		return ret;
	}

	/* after test cc2530 dosn't return ZB_START_CONFIRM, it return ZDO_STATE_CHANGE_IND
	result = znp->waitAREQ(0x46, 0x80);
	ret = ((unsigned char *)result->data)[0];
	delete result->data;
	delete result;

	if (ret) {
		D("ZB_START_CONFIRM:%d", ret);
		return ret;
	}
*/
	do {
		result = znp->waitAREQ(0x45, 0xc0);
		ret = ((unsigned char *)result->data)[0];
		D("ZDO_STATE_CHANGE_IND return %d", ret);
		delete result->data;
		delete result;
		if (ret == 9) { //DEV_ZB_COORD Started as Zigbee Coordinator
			D("%s:start network ok", __FUNCTION__);
			break;
		}
	} while(1);

	return 0;
}


ZNP *Server::getZNP()
{
	return znp;
}
