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
	uint8_t len;
	uint8_t cmd0;
	uint8_t cmd1;
	union {
		uint8_t status;
		uint8_t *data;
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
	static uint8_t calcFCS(uint8_t *pMsg,
			uint8_t len, uint8_t fcs);
	void handleRecvFrame(FRAME *frame);
	static void sigusr1(int sig);
	FRAME *recvFrame();
};

#endif /* MT_H_ */
