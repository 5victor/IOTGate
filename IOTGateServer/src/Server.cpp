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

void Server::waitIndicate(areq_type type)
{
	do {
		mutex->lock();
		cond->wait(*mutex);

		if (type == indicate->type)
			break;
	} while(1);
}
