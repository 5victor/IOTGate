/*
 * Server.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#include "Server.h"

Server::Server() {
	// TODO Auto-generated constructor stub
	mutexcmd = new Mutex(Mutex::SHARED);
	condcmd = new Condition(Condition::SHARED);
	cmdlock = new Mutex(Mutex::SHARED);
}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

int Server::initServer()
{
	znp = new ZNP();
	return znp->initZNP(this);
}

void Server::startServer()
{
	this->run();
}

int Server::startNetwork(int panid, unsigned int chanlist)
{
	FRAME *result;
	int ret;
	int arg;

	znp->SYS_RESET_REQ();
	result = znp->waitAREQ(0x41, 0x80);

	D("%s:SYS_RESET_REQ:0x%x", __FUNCTION__, *((unsigned *)&result->data[1]));
	freeFrame(result);

	//ZCD_NV_LOGICAL_TYPE
	arg = 0; //COORDINATOR(0x00), ROUTER (0x01) or ENDDEVICE (0x02)
	ret = znp->ZB_WRITE_CONFIGURATION(0x87, 1, (uint8_t *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_LOGICAL_TYPE fail %d", ret);
		return ret;
	}

	//ZCD_NV_PANID
	arg = panid;
	ret = znp->ZB_WRITE_CONFIGURATION(0x83, 2, (uint8_t *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_PANID fail %d", ret);
		return ret;
	}

	//ZCD_NV_CHANLIST
	arg = chanlist; //bitmask
	ret = znp->ZB_WRITE_CONFIGURATION(0x84, 4, (uint8_t *)&arg);
	if (ret) {
		D("ZB_WRITE_CONFIGURATION:ZCD_NV_CHANLIST fail %d", ret);
		return ret;
	}

	//ZCD_NV_ZDO_DIRECT_CB
	arg = 1;
	ret = znp->ZB_WRITE_CONFIGURATION(0x8F, 1, (uint8_t *)&arg);
	if (ret) {
		D("%s:ZCD_NV_ZDO_DIRECT_CB fail %d", __FUNCTION__, ret);
		return ret;
	}

	ret = znp->ZB_START_REQUEST();

	if (ret) {
		D("ZB_START_REQUEST fail %d", ret);
		return ret;
	}

	/* after test cc2530 dosn't return ZB_START_CONFIRM, it return ZDO_STATE_CHANGE_IND
	result = znp->waitAREQ(0x46, 0x80);
	ret = ((uint8_t *)result->data)[0];
	delete result->data;
	delete result;

	if (ret) {
		D("ZB_START_CONFIRM:%d", ret);
		return ret;
	}
*/
	do {
		result = znp->waitAREQ(0x45, 0xc0);
		ret = ((uint8_t *)result->data)[0];
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

void Server::foundNode(uint16_t nwkaddr)
{
	D("%s:0x%x", __FUNCTION__, nwkaddr);
	setUnsignData(nwkaddr);
	commitCommand(FOUND_NODE);
}

void Server::setSignData(int data)
{
	cmddata.sdata = data;
}
void Server::setUnsignData(unsigned int data)
{
	cmddata.udata = data;
}

void Server::setBufData(uint8_t *buf, int len)
{
	cmddata.buf = buf;
	cmddata.len = len;
}

void Server::commitCommand(enum Command cmd)
{
	cmdlock->lock();
	this->cmd = cmd;
	condcmd->signal();
	cmdlock->unlock();
}

bool Server::threadLoop()
{
	while(1) {
		{
			Mutex::Autolock _l(mutexcmd);
			condcmd->wait(*mutexcmd);
			D("%s:recv cmd:%d", __PRETTY_FUNCTION__, cmd);
		}
		cmdlock->lock();
		switch(cmd) {
		case FOUND_NODE: {
			addNode(cmddata.sdata);
			break;
		}
		default:
			D("Server::threadLoop: unkown command");
		}
		cmdlock->unlock();
	}

	return true;
}

int Server::getNodeEndpoints(uint16_t nwkaddr, vector<Endpoint *> &endpoints)
{
	int ret;
	FRAME *result;
	uint8_t *eplist;
	int epcount;

	ret = znp->ZDO_ACTIVE_EP_REQ(nwkaddr);
	if (ret) {
		LOG("%s:ZDO_ACTIVE_EP_REQ fail %d", __FUNCTION__, ret);
		goto error;
	}
	result = znp->waitAREQ(0x45, 0x85);

	epcount = *(uint8_t *)&result->data[5];
	eplist = &result->data[6];

	for(int i; i < epcount; i++) {
		Endpoint *ep;
		ep = getEndpoint(nwkaddr, eplist[i]);
		if (ep == NULL) {
			D("%s:getEndpoint return NULL", __FUNCTION__);
			continue;
		}
		endpoints.push_back(ep);
	}
	freeFrame(result);
	ret = 0;
error:
	return ret;
}

Endpoint *Server::getEndpoint(uint16_t nwkaddr, int index)
{
	int ret;
	FRAME *result;
	uint8_t *data;

	ret = znp->ZDO_SIMPLE_DESC_REQ(nwkaddr, index);
	if (ret) {
		D("%s:ZDO_SIMPLE_DESC_REQ fail %d", __FUNCTION__, ret);
		return NULL;
	}

	result = znp->waitAREQ(0x45, 0x84);
	if (!result) {
		D("%s:ZDO_SIMPLE_DESC_RSP fail");
		return NULL;
	}

	Endpoint *ep = new Endpoint;

	data = result->data;
	ep->setIndex(data[6]);
	ep->setProfileID(*(uint16_t *)&data[7]);
	ep->setDeviceID(*(uint16_t *)&data[9]);
	ep->setInClusters(data[12], (uint16_t *)&data[13]);
	int outnumoffset = 12 + data[12] * sizeof(uint16_t);
	ep->setOutClusters(data[outnumoffset], (uint16_t *)data[outnumoffset+1]);

	freeFrame(result);

	return ep;
}

int Server::addNode(uint16_t nwkaddr)
{
	int ret;
	FRAME *result;
	Node *node;

	D("%s", __PRETTY_FUNCTION__);
	ret = znp->ZDO_NODE_DESC_REQ(nwkaddr);
	if (ret) {
		LOG("%s:ZDO_NODE_DESC_REQ fail %d", __FUNCTION__, ret);
		return ret;
	}

	result = znp->waitAREQ(0x45, 0x82);

	node = new Node();
	node->setNWKAddr(*(uint16_t *)&result->data[3]);
	if (nwkaddr != node->getNWKAddr()) {
		D("%s:nwkaddr != node->getNWKAddr()", __FUNCTION__);
		ret = -1;
		goto error_wait;
	}
	node->setType((*(uint8_t *)&result->data[4]) & 7);
	freeFrame(result);

	ret = getNodeEndpoints(nwkaddr, node->getEndpoints());

	if (ret) {
		D("%s:getNodeEndpoints fail %d", __FUNCTION__, ret);
		goto error;
	}

	nodes.push_back(node);

	return 0;

error_wait:
	freeFrame(result);
error:
	delete node;
	return ret;
}
