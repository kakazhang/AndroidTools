#ifndef BATTERYUPDATE_H
#define BATTERYUPDATE_H
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <utils/String8.h>
#include <binder/IBinder.h>
#include <binder/TextOutput.h>

#include "IListener.h"

using namespace std;
namespace android {
class BatteryUpdate : public IListener {
public:
	BatteryUpdate();
	virtual ~BatteryUpdate();
	virtual void notify();
	void dump() {
		//cout << "mPresent:" << mPresent << endl;
		//cout << "mCapacity:" << mCapacity << endl;
		//cout << "mHealth:" << mHealth;
		//cout << "mStatus:" << mStatus;

		printf("mPresent:%d\n", mPresent);
		printf("mCapacity:%d\n", mCapacity);
		printf("mHealth:%s\n", mHealth);
		printf("mStatus:%s\n", mStatus);
	}

	void showWakeLockUsage();
protected:
	void update();
	int readFromPath(String8 path, char *buf, int len);
private:
	sp<IBinder> mPower;
		
private:
	int lastCapacity;
	int mCapacity;
	char mHealth[24];
	bool mPresent;
	char mStatus[24];
};
}
#endif