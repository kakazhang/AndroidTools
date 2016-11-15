/*************************************************************************
	> File Name: NetlinkManager.h
	> Author: kaka.zhang
	> Mail: kaka.zhang@snowballtech.com 
	> Created Time: Sun 02 Aug 2015 04:41:51 PM CST
 ************************************************************************/
#ifndef NETLINKMANAGER_H
#define NETLINKMANAGER_H
#include <pthread.h>

#include "NetlinkHandler.h"

class NetlinkManager {
public:
	static NetlinkManager* Instance();
	virtual ~NetlinkManager();

	void start();
	void stop();

private:
	NetlinkManager();
	
private:
	static pthread_mutex_t gMutex;
	static NetlinkManager *sInstance;
	NetlinkHandler *mHandler;
	int mSock;
};
#endif

