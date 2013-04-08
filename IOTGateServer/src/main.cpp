/*
 * main.cpp
 *
 *  Created on: Mar 28, 2013
 *      Author: victor
 */

#include <stdio.h>
#include "Server.h"
#include "SocketServer.h"

int main()
{
	int ret;
	ZNP *znp;

	Server *server = new Server();
	ret = server->initServer();
	server->startServer();
	if (ret)
		printf("initServer fail");

	ret = server->startNetwork(513);
	D("startNetwork:%d\n", ret);

	SocketServer *socketserver = new SocketServer();

	server->setSocketServer(socketserver);
	socketserver->init(server);
	socketserver->threadLoop();

	return 0;
}
