#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

#include "BatteryManager.h"

#define MAXEVENTS 8

using namespace std;
using namespace android;

int main() {
	//int fd = open("/dev/input/event2", O_RDONLY);
	sp<BatteryManager> bm = BatteryManager::getInstance();
	bm->start();
	
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
}
