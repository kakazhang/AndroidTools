#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <cutils/uevent.h>

#include "BatteryListener.h"
#include "BatteryUpdate.h"
#include "IListener.h"

using namespace std;

#define ACTION_CHANGE "change"
namespace android {
BatteryListener::BatteryListener(int sock) 
	: SockListener(sock){
	mWLocks = new WakeLock();
	mWLocks->run("WakeLock");
}

BatteryListener::~BatteryListener() {
}

void BatteryListener::registerListener(String8 name, sp<IListener> listener) {
	auto it = mListeners.find(name);
	if (it != mListeners.end()) {
		return;
	}

	mListeners[name] = listener;
}

void BatteryListener::onEvent(int fd) {
    char msg[UEVENT_MSG_LEN+2];
    char *cp;
    int n;

    n = uevent_kernel_multicast_recv(fd, msg, UEVENT_MSG_LEN);
    if (n <= 0)
        return;
    if (n >= UEVENT_MSG_LEN)   /* overflow -- discard */
        return;

    msg[n] = '\0';
    msg[n+1] = '\0';

	if (/*strstr(msg, POWER_SUPPLY_SUBSYSTEM) && */strstr(msg, POWER_SUPPLY_SUBSYSTEM)) {
		char* cp = msg;
		cout << cp << endl;
		if (!strncmp(cp, ACTION_CHANGE, strlen(ACTION_CHANGE))) {
			mWLocks->notify();
			//BatteryUpdate::getInstance()->update();
			for (auto it = mListeners.begin(); it != mListeners.end(); it++) {
				String8 name = it->first;
				cout << "name:" << name.string() << " update" << endl;
				sp<IListener> listener = it->second;
				listener->notify();
			}
		}
	}

}
}
