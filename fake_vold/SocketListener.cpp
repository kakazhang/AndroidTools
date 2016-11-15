#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "SocketListener.h"
#define HINT 8

SocketListener::SocketListener(int sock, bool listen) 
	:mSock(sock),bListen(listen) {

	mEpollFd = epoll_create(HINT);
	if (mEpollFd < 0) {
		perror("epoll_create");
		exit(-1);
	}
	
	if (pipe(pipeFds)) {
		perror("pipe");
		exit(-2);
	}
}

SocketListener::~SocketListener() {
	if (mSock > 0) {
		close(mSock);
		mSock = -1;
	}

	close(pipeFds[0]);
	close(pipeFds[1]);
}

void SocketListener::startListener() {
	pthread_t tListener;
	struct epoll_event ev;
	
	ev.events = EPOLLIN;
	ev.data.fd = pipeFds[0];
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, pipeFds[0], &ev) == -1) {
		perror("epoll_ctl: pipeFds[0]");
		exit(-1);
	}

	ev.events = EPOLLIN;
	ev.data.fd = mSock;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mSock, &ev) == -1) {
		perror("epoll_ctl: mSock");
		exit(-1);
	}
	
	pthread_create(&tListener, NULL, SocketListener::threadListener, this);
}

void* SocketListener::threadListener(void* args) {
	SocketListener* listener = (SocketListener *)args;
	listener->runListener();

	pthread_exit(NULL);
	return NULL;
}

void SocketListener::runListener() {
	struct epoll_event events[HINT];
	int nFds = -1;
	int nFd = 0;
	
	while (1) {
		nFds = epoll_wait(mEpollFd, events, HINT, -1);
		if (-1 == nFds) {
			perror("epoll_wait");
			exit(-1);
		}

		for (nFd = 0; nFd < nFds; nFd++) {
			if (events[nFd].data.fd == pipeFds[0]) {
				goto out;
			} else if (events[nFd].data.fd == mSock){
				onAvailable(mSock);
			}
		}
		continue;
	out:
		printf("Stop SocketListener looper\n");
		break;
	}
}

void SocketListener::stopListener() {
	char CtrlPipe_Shutdown = 0;

	int ret = write(pipeFds[0],&CtrlPipe_Shutdown, sizeof(CtrlPipe_Shutdown));
	if (ret < 0) {
		perror("write");
		exit(-1);
	}

	close(pipeFds[0]);
	close(pipeFds[1]);

	pipeFds[0] = -1;
	pipeFds[1] = -1;
}

