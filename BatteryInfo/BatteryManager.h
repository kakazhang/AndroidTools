#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H
#include <pthread.h>
#include <utils/RefBase.h>

#include "SockListener.h"
#include "IListener.h"

namespace android {

class BatteryManager : public RefBase {
public:
	static sp<BatteryManager> getInstance();
	virtual ~BatteryManager();
	void start();
private:
	BatteryManager();
	void init();
	
private:
	static pthread_mutex_t gLock;
	static sp<BatteryManager> gInstance;
	sp<SockListener> mListener;
	sp<IListener> mUpdater;
	int uFd;
};

}
#endif
