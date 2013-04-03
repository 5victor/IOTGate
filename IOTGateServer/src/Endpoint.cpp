/*
 * Endpoint.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#include "Endpoint.h"
#include <string.h>

Endpoint::Endpoint() {
	// TODO Auto-generated constructor stub

}

Endpoint::~Endpoint() {
	// TODO Auto-generated destructor stub
}


void Endpoint::setIndex(uint8_t index)
{
	this->index = index;
}

uint8_t Endpoint::getIndex()
{
	return index;
}

void Endpoint::setNWKAddr(uint16_t nwkaddr)
{
	this->nwkaddr = nwkaddr;
}

uint16_t Endpoint::getNWKAddr()
{
	return nwkaddr;
}

void Endpoint::setProfileID(uint16_t profileid)
{
	this->profileid = profileid;
}

uint16_t Endpoint::getProfileID()
{
	return profileid;
}

void Endpoint::setDeviceID(uint16_t deviceid)
{
	this->deviceid = deviceid;
}

uint16_t Endpoint::getDeviceID()
{
	return deviceid;
}

void Endpoint::setInClusters(int num, uint16_t list[])
{
	this->inclusternum = num;
	memcpy(this->inclusterlist, list, sizeof(uint16_t) * num);
}

int Endpoint::getInClusters(uint16_t **list)
{
	*list = this->inclusterlist;
	return inclusternum;
}

void Endpoint::setOutClusters(int num, uint16_t list[])
{
	this->outclusternum = num;
	memcpy(this->outclusterlist, outclusterlist, sizeof(uint16_t) * 2);
}

int Endpoint::getOutClusters(uint16_t **list)
{
	*list = this->outclusterlist;
	return outclusternum;
}
