/*
 * Endpoint.h
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#ifndef ENDPOINT_H_
#define ENDPOINT_H_

#include <stdint.h>

class Endpoint {
public:
	Endpoint();
	virtual ~Endpoint();

	void setIndex(uint8_t index);
	uint8_t getIndex();
	void setNWKAddr(uint16_t nwkaddr);
	uint16_t getNWKAddr();
	void setProfileID(uint16_t profileid);
	uint16_t getProfileID();
	void setDeviceID(uint16_t deviceid);
	uint16_t getDeviceID();
	void setInClusters(int num, uint16_t list[]);
	int getInClusters(uint16_t **list);
	void setOutClusters(int num, uint16_t list[]);
	int getOutClusters(uint16_t **list);
private:
#define MAX_CLUSTER 32
	uint8_t index;
	uint16_t nwkaddr;
	uint16_t profileid;
	uint16_t deviceid;
	uint8_t inclusternum;
	uint16_t inclusterlist[MAX_CLUSTER];
	uint8_t outclusternum;
	uint16_t outclusterlist[MAX_CLUSTER];
};

#endif /* ENDPOINT_H_ */
