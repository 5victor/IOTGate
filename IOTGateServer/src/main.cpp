/*
 * main.cpp
 *
 *  Created on: Mar 28, 2013
 *      Author: victor
 */

#include <stdio.h>
#include "ZNP.h"

int main()
{
	int ret;

	ZNP *znp = new ZNP();
	ret = znp->initZNP();
	if (ret)
		printf("initZNP fail");

	ret = znp->SYS_PING();
	printf("SYS_PING:0x%x\n", ret);
}
