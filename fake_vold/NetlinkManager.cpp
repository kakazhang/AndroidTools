#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>

#include "NetlinkManager.h"

pthread_mutex_t NetlinkManager::gMutex = PTHREAD_MUTEX_INITIALIZER;
NetlinkManager* NetlinkManager::sInstance = NULL;

NetlinkManager* NetlinkManager::Instance() {
	pthread_mutex_lock(&gMutex);
	if (sInstance == NULL) {
		sInstance = new NetlinkManager();
	}
	pthread_mutex_unlock(&gMutex);

	return sInstance;
}

NetlinkManager::NetlinkManager() 
	: mSock(-1), mHandler(NULL) {
	
}

NetlinkManager::~NetlinkManager() {
	if (mSock > 0) {
		close(mSock);
		mSock = -1;
	}

	if (mHandler != NULL)
		delete mHandler;
}

void NetlinkManager::start() {
    struct sockaddr_nl nladdr;
    int sz = 64 * 1024;
    int on = 1;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0xffffffff;

    if ((mSock = socket(PF_NETLINK,
                        SOCK_DGRAM,NETLINK_KOBJECT_UEVENT)) < 0) {
        printf("socket");
        return;
    }

	if (setsockopt(mSock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
        printf("setsockopt");
        goto out;
    }

    if (setsockopt(mSock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
        printf("setsockopt");
        goto out;
    }

    if (bind(mSock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
        perror("bind");
        goto out;
    }

    mHandler = new NetlinkHandler(mSock);
    if (!mHandler->start()) {
        printf("Unable to start NetlinkHandler");
        goto out;
    }

    return;
	
out:
	close(mSock);
	return;
}

void NetlinkManager::stop() {
    mHandler->stop();
	
    delete mHandler;
    mHandler = NULL;

    close(mSock);
    mSock = -1;
}

