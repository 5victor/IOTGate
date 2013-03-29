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
	ufd = open("/dev/ttySAC1", O_RDWR | O_NOCTTY);
	if (ufd < 0)
		return ufd;

	ioctl(ufd, TIOCMGET, &ctrlbits);
	if (ctrlbits & TIOCM_CTS)
		D("%s:connect", __FUNCTION__);
	else
		D("%s:disconnect", __FUNCTION__);

	tcgetattr(ufd, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
//	options.c_cflag |= C_RTSCTS;
	options.c_cflag |= CRTSCTS;

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

bool MT::threadLoop()
{
#define BUF_SIZE 512
	unsigned char buf[BUF_SIZE];
	bool receving;
	int ret;
	FRAME *frame;
	unsigned char fcs;
	int recevied;
	D("MT::threadLoop start");
	do {
		D("%s:start read", __FUNCTION__);
		ret = read(ufd, buf, sizeof(buf));
		if (ret < 0) {
			D("MT::threadLoop read error:%d", ret);
			continue;
		}
		D("%s:read %d bytes", __FUNCTION__, ret);
		if (!receving) {
			if (buf[0] != 0xFE) {
				D("Frame first byte != 0xFE");
				continue;
			}
			frame = new FRAME;
			frame->len	= buf[1];
			frame->cmd0	= buf[2];
			frame->cmd1	= buf[3];
			frame->data = new unsigned char[frame->len];
			D("%s:len=%d,cmd0=0x%x,cmd1=0x%x", __FUNCTION__, frame->len, frame->cmd0, frame->cmd1);
			if (frame->len+5 > sizeof(buf)) { //FCS,LEN,CMD0,CMD1,FCS
				D("%s:Multiltime read", __FUNCTION__);
				fcs = calcFCS(&buf[1], sizeof(buf)-1, 0);
				memcpy(frame->data, &buf[4], sizeof(buf)-4);
				recevied = sizeof(buf)-4;
				receving = true;
			} else {
				fcs = calcFCS(&buf[1], frame->len+3, 0);
				if (fcs != buf[4+frame->len]) {
					D("Frame FCS Check Fail");
					delete frame->data;
					delete frame;
					continue;
				}
				memcpy(frame->data, &buf[4], frame->len);
				D("%s:recv a good frame", __FUNCTION__);
				handleRecvFrame(frame);
			}
			continue;
		} else {
			//the condition that have more than two recvie
			if (frame->len > (recevied+(sizeof(buf)-1))) {
				fcs = calcFCS(buf, sizeof(buf), fcs);
				memcpy(&frame->data[recevied], buf, sizeof(buf));
				recevied += sizeof(buf);
				continue;
			} else {
				fcs = calcFCS(buf, frame->len-recevied, fcs);
				if (fcs != buf[frame->len-recevied]) {
					D("Frame FCS Check Fail");
					recevied = 0;
					receving = false;
					delete frame->data;
					delete frame;
				}
				memcpy(&frame->data[recevied], buf, frame->len-recevied);
				recevied = 0;
				receving = false;
				handleRecvFrame(frame);
				continue;
			}
		}
	} while(1);

	return true;
}

void MT::handleRecvFrame(FRAME *frame)
{
	if (sreq.frame != NULL) {
		if ((sreq.frame->cmd0 & 0xFF) == (sreq.frame->cmd1 &0xFF)) {
			if (sreq.frame->cmd1 == frame->cmd1) {
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

	mutexcomp->lock();
	condcomp->wait(*mutexcomp);

	//result = sreq.error ? NULL:sreq.result;

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
