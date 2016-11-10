#include <iostream>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <map>
#include <cutils/uevent.h>

#include "SockListener.h"

#define MAXEVENTS 8
#define BUFSIZE 64*1024

using namespace std; 
namespace android {

SockListener::SockListener(int sock) {
	if (!init() || pipe(mWakeFds))
		exit(-1);
	mSock = sock;

	addListener(mWakeFds[0], NULL);
}

SockListener::~SockListener() {
	for (int i = 0; i < 2; i++) {
		if (mWakeFds[i] > 0)
			close(mWakeFds[i]);
	}

	if (mSock > 0)
		close(mSock);
}

bool SockListener::addListener(int fd, Callback callback) {
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;

	int ret = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event);
	if (-1 == ret) {
		perror("epoll_ctl");
		return false;
	}
	
	return true;
}

int SockListener::startListener() {
	addListener(mSock, NULL);

	run("SocketListenr");
	return 0;
}

void SockListener::stopListener() {
	char w = 'w';

	if (0 >= write(mWakeFds[1], &w, sizeof(w)))
		cout << "wake failed" << endl;
}

bool SockListener::init() {
	mEpollFd  = epoll_create(MAXEVENTS);
	if (mEpollFd < 0) {
		cout << "create epoll events failed" << endl;
		return false;
	}

	return true;
}

void SockListener::waitForEvent() {
	while(1) {
		int nfds;
		struct epoll_event events[MAXEVENTS];

		nfds = epoll_wait(mEpollFd, events, MAXEVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR)
                continue;
           cout << "waitForEvent: epoll_wait failed" << endl;
           break;
        } else if (nfds == 0)
        	continue;

		for (int i = 0; i < nfds; i++) {
			int fd = events[i].data.fd;
			if (mWakeFds[0] == fd) {
				cout << "wake" << endl;
				return;
			} else if (fd == mSock){
				onEvent(fd);
			}
		}
	}
}

bool SockListener::threadLoop() {
	waitForEvent();
	return false;
}
}
