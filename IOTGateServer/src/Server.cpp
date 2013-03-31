/*
 * Server.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#include "Server.h"

Server::Server() {
	// TODO Auto-generated constructor stub

}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

int Server::initServer()
{
	znp = new ZNP();
	znp->initZNP();
}
