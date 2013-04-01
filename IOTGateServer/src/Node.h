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
public:
	Node();
	virtual ~Node();

	enum {ZC,ZR,ZED};
private:
	vector<Endpoint> endpoint;

	int type;
};


#endif /* NODE_H_ */
