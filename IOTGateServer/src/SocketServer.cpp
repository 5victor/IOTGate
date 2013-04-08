/*
 * SocketServer.cpp
 *
 *  Created on: Apr 7, 2013
 *      Author: victor
 */

#include "SocketServer.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

SocketServer::SocketServer() {
	// TODO Auto-generated constructor stub
	port = 1013;
	state = UNINIT;
}

SocketServer::~SocketServer() {
	// TODO Auto-generated destructor stub
}

int SocketServer::init(Server *server)
{
	int ret;
	this->server = server;
	ret = initSocket();
	if (ret)
		return ret;

	ret = initSSL();
	return ret;
}

void SocketServer::start()
{
	this->run();
}

int SocketServer::initSocket()
{
	struct sockaddr_in serv_addr;
	int ret;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		D("%s:create socket fail %d",__FUNCTION__ , sfd);
		return sfd;
	}

	memset(&serv_addr, 0x0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    ret = bind(sfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (ret) {
    	D("%s:bind fail %d", __FUNCTION__, ret);
    	return ret;
    }

    state = INITED;

	return 0;
}

int passwd_cb(char *buf, int size, int flag, void *data)
{
	strcpy(buf, "123456");
	return strlen(buf);
}

int SocketServer::initSSL()
{
	int ret;

	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	ERR_load_SSL_strings();
	OpenSSL_add_all_algorithms();

	ctx = SSL_CTX_new(SSLv23_server_method());
	if (!ctx) {
		LOG("SSL_CTX_new fail");
		return -1;
	}
	SSL_CTX_set_default_passwd_cb(ctx, passwd_cb);
	ret = SSL_CTX_use_certificate_file(ctx, pemfile, SSL_FILETYPE_PEM);
	if (ret != 1) {
		LOG("SSL_CTX_use_certificate_file %d", ret);
		return ret;
	}
	ret = SSL_CTX_use_PrivateKey_file(ctx, csrfile, SSL_FILETYPE_PEM);
	if (ret != 1) {
		LOG("SSL_CTX_use_PrivateKey_file %d", ret);
		return ret;
	}

	return 0;
}

bool SocketServer::threadLoop()
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int ret;

	do {
		ret = listen(sfd, 5);
		afd = accept(sfd, (struct sockaddr *)&addr, &addr_len);
		if (afd < 0) {
			LOG("%s:accept fail", __PRETTY_FUNCTION__);
			continue;
		}
//		fcntl(afd,F_SETFL,fcntl(afd,F_GETFL,0) | O_NONBLOCK);
		D("%s:connect from %s", __PRETTY_FUNCTION__, inet_ntoa(addr.sin_addr));
		SSL *ssl = SSL_new(ctx);
		if (!ssl) {
			LOG("SSL_new fail");
			SSL_free(ssl);
			continue;
		}

		ret = SSL_set_fd(ssl, afd);
		if (ret != 1) {
			LOG("SSL_set_fd fail %d", ret);
			SSL_free(ssl);
			continue;
		}

		ret = SSL_accept(ssl);
		if (ret != 1) {
			LOG("SSL_accept fail %d", ret);
			SSL_free(ssl);
			continue;
		}

		SocketSession *ss = new SocketSession();
		ss->init(ssl, this);
		sessions.push_back(ss);
		ss->start();
	} while (1);


	return true;
}

