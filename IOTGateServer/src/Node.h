/*
 * Node.h
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#ifndef NODE_H_
#define NODE_H_

#include <vector>
using namespace std;

#include "Endpoint.h"

class Node {
enum {ZC = 0,ZR = 1,ZED = 2};

public:
	Node();
	virtual ~Node();

	unsigned short getNWKAddr();
	void setNWKAddr(uint16_t nwkaddr);
	int getType();
	void setType(int type);
	void setIEEEAddr(uint8_t addr[]);
	uint8_t *getIEEEAddr();
/*
	void addEndpoint(Endpoint *endpoint);
	Endpoint *getEndpoint(uint16_t nwkaddr);
	void removeEndpoint(Endpoint *endpoint);
*/
	vector<Endpoint *> &getEndpoints();

private:
	unsigned short nwkaddr;
	int type;

	vector<Endpoint *> endpoints;
	unsigned char ieeeaddr[8];
};


#endif /* NODE_H_ */
