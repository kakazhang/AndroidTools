#include <iostream>
#include <unistd.h>

#include "SysEvent.h"
#include "BatteryManager.h"
#include "BatteryListener.h"
#include "BatteryUpdate.h"

using namespace std;
namespace android {
pthread_mutex_t BatteryManager::gLock = PTHREAD_MUTEX_INITIALIZER;
sp<BatteryManager> BatteryManager::gInstance = NULL;
sp<BatteryManager> BatteryManager::getInstance() {
	if (gInstance == NULL) {
		pthread_mutex_lock(&gLock);
		gInstance = new BatteryManager();
		pthread_mutex_unlock(&gLock);
	}
	
	return gInstance;
}

BatteryManager::BatteryManager() {
	init();
}

BatteryManager::~BatteryManager() {
	if (uFd > 0)
		close(uFd);
}

void BatteryManager::init() {
	uFd = createUEvent();
	mListener = new BatteryListener(uFd);

	mUpdater = new BatteryUpdate();
	mListener->registerListener(String8("Battery"), mUpdater);
}

void BatteryManager::start() {
	if (mListener != NULL) {
		mListener->startListener();
	}
}

}
