/*
 * SocketServer.h
 *
 *  Created on: Apr 7, 2013
 *      Author: victor
 */

#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

#include <utils/Thread.h>
using namespace android;

#include <openssl/ssl.h>
#include <vector>
#include "Server.h"
#include "SocketSession.h"

enum {
	QUERY_NODE_NUM = 0x5,
	QUERY_NODES,
	QUERY_NODE_ENDPOINTS,
	SEND_CLUSTER_DATA,
	NOTIFY,
};

struct node {
	unsigned short nwkaddr;
	int type;
	int epnum;
	unsigned char ieeeaddr[8];
}__attribute__ ((packed));

struct endpoint {
	uint8_t index;
	uint16_t nwkaddr;
	uint16_t profileid;
	uint16_t deviceid;
	uint8_t inclusternum;
	uint16_t inclusterlist[MAX_CLUSTER];
	uint8_t outclusternum;
	uint16_t outclusterlist[MAX_CLUSTER];
}__attribute__ ((packed));

struct hdr {
	uint8_t cmd;
	uint8_t id;
	int	data_len;
}__attribute__ ((packed));

class Server;

class SocketSession;

class SocketServer : public Thread {
friend class SocketSession;
public:
	SocketServer();
	virtual ~SocketServer();
	int init(Server *server);
	void start();
	int getState();

public:
	void setPort(int port);

private:
	int sfd;
	int afd;
	int port;
	Server *server;
enum state {
	UNINIT = 0,
	INITED,
	CONNECTED,
};
	int state;
	vector<SocketSession *> sessions;

private:
#define pemfile "/system/etc/iot/server.pem"
#define csrfile "/system/etc/iot/server.csr"
	SSL_CTX *ctx;

private:
	int initSocket();
	int initSSL();
public:
	bool threadLoop();
};

#endif /* SOCKETSERVER_H_ */
