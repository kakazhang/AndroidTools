#ifndef BATTERYLISTENER_H
#define BATTERYLISTENER_H
#include <map>
#include <utils/String8.h>

#include "WakeLock.h"
#include "SockListener.h"
#include "IListener.h"

#define BUFSIZE 64*1024
#define UEVENT_MSG_LEN 2048
#define POWER_SUPPLY_SUBSYSTEM "power_supply"
#define ACTION_BATTERY "battery"

using namespace std;
namespace android {
class BatteryListener : public SockListener {
public:
	BatteryListener(int sock);
	virtual ~BatteryListener();

	virtual void registerListener(String8 name, sp<IListener> listener);
	
protected:
	virtual void onEvent(int fd);
	
private:
	sp<WakeLock> mWLocks;
	map<String8, sp<IListener>> mListeners;
};
}
#endif