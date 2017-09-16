#ifndef BARRIER_H
#define BARRIER_H

#include <utils/Mutex.h>
#include <utils/Condition.h>

#define STATUS_CLOSE 0
#define STATUS_OPEN 1

namespace android {

class Barrier {
public:
	Barrier() : mStatus(STATUS_CLOSE){}
    virtual ~Barrier() {}

    void open() {
        Mutex::Autolock autoLock(gLock);
        if (mStatus == STATUS_CLOSE) {
			mStatus = STATUS_OPEN;
			mCond.broadcast();
        }
    }

    void close() {
        Mutex::Autolock autoLock(gLock);
        mStatus = STATUS_CLOSE;
    }

    void wait() {
        Mutex::Autolock autoLock(gLock);
        if (mStatus == STATUS_CLOSE)
            mCond.wait(gLock);
    }

private:
    mutable Mutex gLock;
    mutable Condition mCond;
    volatile int mStatus;
};

};

#endif
