/*
 * MT.h
 *
 *  Created on: Mar 27, 2013
 *      Author: victor
 */

#ifndef MT_H_
#define MT_H_

#include <utils/Thread.h>
#include <utils/Mutex.h>
using namespace android;

#include "common.h"

/*
struct SRSP {
	unsigned char len;
	unsigned char cmd0;
	unsigned char cmd1;
	union {
		unsigned char status;
		unsigned char *data;
	};
};
*/

struct REQ {
	FRAME *frame;
	bool senderror;
	FRAME *result;
};

class MT : public Thread {
public:
	MT();
	int start(void);
	virtual ~MT();
	FRAME *sendSREQ(FRAME *send);
	int sendAREQ(FRAME *send);
	void setAREQHandle(AREQHANDLE handle);

private:
	static int ufd;
	static REQ sreq;
	static Mutex *mutexsend;
	//static Condition *condsend;
	static Mutex *mutexcomp;
	static Condition *condcomp;
	static Mutex *sendcomp; /* send complete */

	AREQHANDLE areqhandle;
	bool threadLoop();

private:
	int initUart(void);
	int initSignal(void);
	static unsigned char calcFCS(unsigned char *pMsg,
			unsigned char len, unsigned char fcs);
	void handleRecvFrame(FRAME *frame);
	static void sigusr1(int sig);
};

#endif /* MT_H_ */
