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

class Server {
public:
	Server();
	virtual ~Server();

private:
	static struct indicate *indicate;
	static Mutex *mutex;
	static Condition *cond;

private:
	static void indicateHandle(struct indicate *areq);
	void waitIndicate(areq_type type);

private:
	vector<Node> node;
};

#endif /* SERVER_H_ */
