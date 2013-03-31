/*
 * Server.h
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <utils/Thread.h>
using namespace android;

#include <vector>
using namespace std;
#include "Node.h"
#include "ZNP.h"

class Server {
public:
	Server();
	virtual ~Server();
	int initServer();

private:
//	static struct indicate *indicate;

private:
	static void indicateHandle(struct indicate *areq);
//	void waitIndicate(areq_type type);

private:
	static vector<Node> node;
	ZNP *znp;
};

#endif /* SERVER_H_ */
