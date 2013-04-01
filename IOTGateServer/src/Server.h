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

class ZNP;

class Server : public Thread {
public:
	Server();
	virtual ~Server();
	int initServer();
	void startServer();
	ZNP *getZNP();
	int startNetwork(int panid, unsigned int chanlist = 0x00008000);

//commit command, run in threadLoop()
public:
	void foundNode(unsigned short nwkaddr);

private:
	void addNode(unsigned short nwkaddr);

private:
//	static struct indicate *indicate;

private:
	void indicateHandle(struct indicate *areq);
//	void waitIndicate(areq_type type);
	int getNodelist(vector<unsigned short> list);

private:
	vector<Node *> nodes;
	ZNP *znp;

private:
	enum Command {FOUND_NODE,};
	Mutex *mutexcmd;
	Condition *condcmd;

	Mutex *cmdlock;

	enum Command cmd;
	struct {
		union {
			unsigned int udata;
			int sdata;
			unsigned char *buf;
		};
		int len;
	}cmddata;

	bool threadLoop();
	void commitCommand(enum Command cmd);
	void setSignData(int data);
	void setUnsignData(unsigned int data);
	void setBufData(unsigned char *buf, int len);
};

#endif /* SERVER_H_ */
