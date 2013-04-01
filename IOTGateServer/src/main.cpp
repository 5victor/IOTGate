/*
 * main.cpp
 *
 *  Created on: Mar 28, 2013
 *      Author: victor
 */

#include <stdio.h>
#include "Server.h"

int main()
{
	int ret;
	ZNP *znp;

	Server *server = new Server();
	ret = server->initServer();
	if (ret)
		printf("initServer fail");

	ret = server->startNetwork(513);
	D("startNetwork:%d\n", ret);

	znp = server->getZNP();

	znp->ZDO_IEEE_ADDR_REQ(0x1234, 0, 0);

	while(1);

	return 0;
}
