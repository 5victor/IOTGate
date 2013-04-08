/*
 * SocketSession.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: victor
 */

#include "SocketSession.h"

SocketSession::SocketSession() {
	// TODO Auto-generated constructor stub

}

SocketSession::~SocketSession() {
	// TODO Auto-generated destructor stub
}

void SocketSession::init(SSL *ssl, SocketServer *server)
{
	this->ssl = ssl;
	this->socketserver = server;
}

void SocketSession::start()
{
	this->run();
}

bool SocketSession::threadLoop()
{

	return true;
}
