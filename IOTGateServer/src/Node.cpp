/*
 * Node.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: victor
 */

#include "Node.h"
#include <string.h>

Node::Node() {
	// TODO Auto-generated constructor stub

}

Node::~Node() {
	// TODO Auto-generated destructor stub
}


unsigned short Node::getNWKAddr()
{
	return nwkaddr;
}

void Node::setNWKAddr(unsigned short nwkaddr)
{
	this->nwkaddr = nwkaddr;
}

int Node::getType()
{
	return type;
}

void Node::setType(int type)
{
	this->type = type;
}

void Node::setIEEEAddr(unsigned char addr[])
{
	memcpy(ieeeaddr, addr, 8);
}

uint8_t *Node::getIEEEAddr()
{
	return ieeeaddr;
}

/*
void Node::addEndpoint(Endpoint *endpoint)
{
	int i;

}


Endpoint *Node::getEndpoint(unsigned short nwkaddr)
{

}

void Node::removeEndpoint(Endpoint *endpoint)
{

}
*/

vector<Endpoint *> &Node::getEndpoints()
{
	return endpoints;
}
