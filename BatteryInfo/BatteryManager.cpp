#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "SysEvent.h"
#include "BatteryManager.h"
#include "BatteryListener.h"
#include "BatteryUpdate.h"

#define GO_HISPEED_LOAD "/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load"
#define GO_MAXSPEED_LOAD "/sys/devices/system/cpu/cpufreq/interactive/go_maxspeed_load"
#define DEFAULT_LOAD "80"

using namespace std;
namespace android {
pthread_mutex_t BatteryManager::gLock = PTHREAD_MUTEX_INITIALIZER;
sp<BatteryManager> BatteryManager::gInstance = NULL;

int BatteryManager::sys_write(const char* path, const char* s) {
	int fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	int len = write(fd, s, strlen(s));
	if (len < 0) {
		perror("write");
		return -1;
	}

	close(fd);
	
	return 0;
}

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
	initCpuLoad();
	
	uFd = createUEvent();
	mListener = new BatteryListener(uFd);

	mUpdater = new BatteryUpdate();
	mListener->registerListener(String8("Battery"), mUpdater);
}

void BatteryManager::initCpuLoad() {
    sys_write(GO_HISPEED_LOAD,DEFAULT_LOAD);
	sys_write(GO_MAXSPEED_LOAD,DEFAULT_LOAD);
}

void BatteryManager::start() {
	if (mListener != NULL) {
		mListener->startListener();
	}
}

}
