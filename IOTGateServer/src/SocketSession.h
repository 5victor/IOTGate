/*
 * SocketSession.h
 *
 *  Created on: Apr 8, 2013
 *      Author: victor
 */

#ifndef SOCKETSESSION_H_
#define SOCKETSESSION_H_

#include <openssl/ssl.h>
#include <utils/Thread.h>
using namespace android;

#include "SocketServer.h"

class SocketServer;

class SocketSession : public Thread {
public:
	SocketSession();
	virtual ~SocketSession();
	void init(SSL *ssl, SocketServer *server);
	void start();

private:
	SSL *ssl;
	SocketServer *socketserver;

private:
	bool threadLoop();
};

#endif /* SOCKETSESSION_H_ */
