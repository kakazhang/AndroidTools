#ifndef WAKELOCK_H
#define WAKELOCK_H

#include <pthread.h>
#include <utils/Thread.h>

using namespace std;
using namespace android;
class WakeLock : public Thread {
public:
	WakeLock();
	virtual ~WakeLock();
	void notify();
	
private:
	virtual bool threadLoop();
	void showWakeLocks();
private:
	pthread_mutex_t gLock;
	pthread_cond_t gCond;
};

#endif