#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utils/Vector.h>

#include <binder/IServiceManager.h>

#include "BatteryUpdate.h"

#define POWER_PATH "/sys/class/power_supply/battery"
using namespace std;

namespace android {

BatteryUpdate::BatteryUpdate() {
	sp<IServiceManager> sm = defaultServiceManager();
	if (sm != NULL) {
		mPower = sm->checkService(String16("power"));
	}
	mCapacity = 0;
	mPresent = false;
	
	memset(mHealth, 0, sizeof(mHealth));
	memset(mStatus, 0, sizeof(mStatus));
}

BatteryUpdate::~BatteryUpdate() {
	
}

void BatteryUpdate::notify() {
	update();
	dump();
}

void BatteryUpdate::update() {
	String8 path;
	char buf[24];
	Vector<String16> args;

	//fflush(stdout);
	path.appendFormat("%s/capacity", POWER_PATH);
	if (!access(path.string(), F_OK)) {
		memset(buf, 0, sizeof(buf));
		lastCapacity = mCapacity;
		if (readFromPath(path, buf, sizeof(buf)) > 0) {
			mCapacity = atoi(buf);
		#if 0
			if (/*lastCapacity < mCapacity &&*/ mPower != NULL) {
				args.add(String16("wakelock"));
				mPower->dump(STDOUT_FILENO, args);
			}
		#endif
		}
	}

	path.clear();
	path.appendFormat("%s/present", POWER_PATH);
	if (!access(path.string(), F_OK)) {
		memset(buf, 0, sizeof(buf));
		if (readFromPath(path, buf, sizeof(buf)) > 0)
			mPresent = atoi(buf) == 1;
	}

	path.clear();
	path.appendFormat("%s/health", POWER_PATH);
	if (!access(path.string(), F_OK)) {
		memset(buf, 0, sizeof(buf));
		int count = readFromPath(path, buf, sizeof(buf));
		if (count > 0)
			memcpy(mHealth, buf, count);
	}

	path.clear();
	path.appendFormat("%s/status", POWER_PATH);
	if (!access(path.string(), F_OK)) {
		memset(buf, 0, sizeof(buf));
		
		int count = readFromPath(path, buf, sizeof(buf));
		if (count > 0) {
			memset(mStatus, 0, sizeof(mStatus));
			memcpy(mStatus, buf, count);
		}
	}
}

int BatteryUpdate::readFromPath(String8 path, char *buf, int len) {
	int fd = open(path.string(), O_RDONLY);
	if (fd < 0)
		return -1;
	memset(buf, 0, len);
	int count = read(fd, buf, len);

	close(fd);
	return count;
}

void BatteryUpdate::showWakeLockUsage() {
	
}
}
