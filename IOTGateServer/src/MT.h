/*
 * MT.h
 *
 *  Created on: Mar 27, 2013
 *      Author: victor
 */

#ifndef MT_H_
#define MT_H_

#include <utils/Thread.h>
using namespace android;

#include "common.h"

struct FRAME {
	unsigned char len; /* length of data */
	unsigned char cmd0;
	unsigned char cmd1;
	unsigned char *data;
};

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

struct SREQ {
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
	static SREQ sreq;
	static Mutex *mutexsend;
	//static Condition *condsend;
	static Mutex *mutexcomp;
	static Condition *condcomp;
	static Mutex *sendcomp; /* send complete */

	AREQHANDLE areqhandle;

private:
	int initUart(void);
	int initSignal(void);
	static unsigned char calcFCS(unsigned char *pMsg,
			unsigned char len, unsigned char fcs = 0);
	void handleRecvFrame(FRAME *frame);
	static void sigusr1(int sig);
};

#endif /* MT_H_ */
