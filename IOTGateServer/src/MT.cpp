/*
 * MT.cpp
 *
 *  Created on: Mar 27, 2013
 *      Author: victor
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>

#include "MT.h"
#include "debug.h"

int MT::ufd;
REQ MT::sreq;
Mutex *MT::mutexsend;
Mutex *MT::mutexcomp;
Condition *MT::condcomp;
Mutex *MT::sendcomp;

MT::MT() {
	// TODO Auto-generated constructor stub
	MT::mutexsend = new Mutex(Mutex::SHARED);
//	condsend = new Condition(SHARED);
	MT::mutexcomp = new Mutex(Mutex::SHARED);
	MT::condcomp = new Condition(Condition::SHARED);
	MT::sendcomp = new Mutex(Mutex::SHARED);
	MT::sendcomp->lock();

	MT::sreq.frame = NULL;
//	sreq.error = false;
	MT::sreq.senderror = false;
	areqhandle = NULL;
}

MT::~MT() {
	// TODO Auto-generated destructor stub
}

int MT::start(void)
{
	int ret;
	ret = initUart();
	if (ret) {
		D("initUart return %d", ret);
		return ret;
	}

	ret = initSignal();
	if (ret) {
		D("initSignal return %d", ret);
		return ret;
	}

	this->run();
	return 0;
}

int MT::initUart(void)
{
	struct termios options;
	int ctrlbits;
	ufd = open("/dev/ttySAC3", O_RDWR | O_NOCTTY);
	if (ufd < 0)
		return ufd;

	ioctl(ufd, TIOCMGET, &ctrlbits);
	if (ctrlbits & TIOCM_CTS)
		D("%s:connect", __FUNCTION__);
	else
		D("%s:disconnect", __FUNCTION__);

	tcgetattr(ufd, &options);
	cfsetispeed(&options, B38400);
	cfsetospeed(&options, B38400);
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
//	options.c_cflag |= CRTSCTS;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
	options.c_oflag &= ~OPOST;   /*Output*/

	tcsetattr(ufd, TCSANOW, &options);
	return 0;
}

int MT::initSignal(void)
{
	return -(SIG_ERR == signal(SIGUSR1,MT::sigusr1));
}

unsigned char MT::calcFCS(unsigned char *pMsg,
		unsigned char len, unsigned char fcs)
{
//	unsigned char result = fcs;
	while(len--)
	{
		fcs ^= *pMsg++;
	}
	return fcs;
}

FRAME *MT::recvFrame()
{
#define BUF_SIZE 512
	int ret;
	int i;
	unsigned char ch;
	unsigned char buf[BUF_SIZE];
	int len;
	int num;
	int start;
	unsigned char fcs;
	FRAME *frame;

	D("%s", __FUNCTION__);

	do {
		ret = read(ufd, &ch, 1);
		if (ret == 1 && ch == 0xFE)
			break;
		else if (ret < 0)
			return NULL;
		D("%s:char != 0xFE", __FUNCTION__);
	} while(1);
	D("%s:recv 0xFE", __FUNCTION__);
	do {
		ret = read(ufd, &ch, 1);
		if (ret == 1) {
			len = ch;
			break;
		} else if (ret < 0)
			return NULL;
	} while (1);
	D("%s:len=%d", __FUNCTION__, len);
	num = len + 3; /* CMD0, CMD1, DATA, FCS */
	start = 0;
	do {
		ret = read(ufd, &buf[start], num);
		if (ret > 0) {
			start += ret;
			num -= ret;
			if (num)
				continue;
			else
				break;
		} else if (ret < 0)
			return NULL;
	} while(1);

	fcs = calcFCS((unsigned char *)&len, 1, 0); /* len */
	fcs = calcFCS(buf, len + 2, fcs); /* cmd0, cmd1, data */
	if (fcs != buf[len + 2]) {
		D("%s:FCS check fail, should %d, but %d", __FUNCTION__, fcs, buf[len + 2]);
		D("%s:bad packet len=%d, cmd0=0x%x, cmd1=0x%x", __FUNCTION__, len, buf[0], buf[1]);
		return NULL;
	}

	frame = new FRAME();
	frame->len = len;
	frame->cmd0 = buf[0];
	frame->cmd1 = buf[1];
	frame->data = new unsigned char[len];
	memcpy(frame->data, &buf[2], len);
	D("%s:Get one Frame len=%d,cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->len, frame->cmd0, frame->cmd1);

	return frame;
}

bool MT::threadLoop()
{
	unsigned char buf[BUF_SIZE];
	bool receving;
	int ret;
	FRAME *frame;
	unsigned char fcs;
	int recevied;
	D("MT::threadLoop start");
	do {
			frame = recvFrame();
			if (frame != NULL)
				handleRecvFrame(frame);
	} while(1);

	return true;
}

void MT::handleRecvFrame(FRAME *frame)
{
	if (sreq.frame != NULL) {
		if ((sreq.frame->cmd0 & 0xFF) == (sreq.frame->cmd0 &0xFF)) {
			if (sreq.frame->cmd1 == frame->cmd1) {
				D("%s:SRSP cmd0=0x%x, cmd1=0x%x", __FUNCTION__, frame->cmd0, frame->cmd1);
				sreq.result = frame;
				condcomp->signal();
				return;
			}
		}
	}
	if ((frame->cmd0 & 0x40) == 0x40) {
		if (areqhandle != NULL) {
			areqhandle(frame);
			return;
		} else {
			D("%s:areqhandle == NULL", __FUNCTION__);
		}
	}
	D("handle RecvFrame have unprocess frame");
	return;
}

FRAME *MT::sendSREQ(FRAME *send)
{
	FRAME *result;
/*
	if (sreq.frame) {
		mutexsend->lock();
		//condsend->wait(*mutexsend);
		D("Some packet complete");
	}
*/
	mutexsend->lock();

	sreq.frame = send;
	kill(getpid(), SIGUSR1);

	sendcomp->lock();
	if (sreq.senderror) {
		result = NULL;
		goto out;
	}
	D("%s:send complete and have no error\n", __FUNCTION__);

	{
		Mutex::Autolock _l(*mutexcomp);
		condcomp->wait(*mutexcomp);
		D("%s: recv SRSP", __FUNCTION__);
	}
	//result = sreq.error ? NULL:sreq.result;
	result = sreq.result;
out:
	mutexsend->unlock();
//	delete sreq.frame->data;
//	delete sreq.frame;
	sreq.frame = NULL; /* need lock ?*/
//	sreq.senderror = false;
	return result;
}

void MT::sigusr1(int sig)
{
	int ret;
	unsigned char sof = 0xFE;
	FRAME *frame = sreq.frame;
	unsigned char fcs;

	D("sigusr1 enter");

	ret = write(ufd, &sof, 1);
	if (ret != 1)
		goto error;
	ret = write(ufd, &frame->len, 1);
	if (ret != 1)
		goto error;
	ret = write(ufd, &frame->cmd0, 1);
	if (ret != 1)
		goto error;
	ret = write(ufd, &frame->cmd1, 1);
	if (ret != 1)
		goto error;

	if (frame->len) {
		ret = write(ufd, frame->data, frame->len);
		if (ret != frame->len)
			goto error;
	}

	fcs = calcFCS(&frame->len, 1, 0);
	fcs = calcFCS(&frame->cmd0, 1, fcs);
	fcs = calcFCS(&frame->cmd1, 1, fcs);
	if (frame->len)
		fcs = calcFCS(frame->data, frame->len, fcs);

	ret = write(ufd, &fcs, 1);
	if (ret != 1)
		goto error;

	sreq.senderror = false;
	sendcomp->unlock();
	D("sigusr1 return");
	return;
error:
	sendcomp->unlock();
	sreq.senderror = true;
	D("sigusr1 error return");
	return;
}

void MT::setAREQHandle(AREQHANDLE handle)
{
	areqhandle = handle;
}

int MT::sendAREQ(FRAME *send)
{
	int ret;

	MT::mutexsend->lock();
	sreq.frame = send;
	kill(getpid(), SIGUSR1);

	sendcomp->lock();
	if (sreq.senderror) {
		ret = -1;
		goto out;
	}

	ret = 0;
out:
	sreq.frame = NULL;
	mutexsend->unlock();
	return ret;
}
