#define LOG_TAG "Bandlead"

#include <utils/Looper.h>
#include <utils/Log.h>

#include "Bandlead.h"

namespace android {
Bandlead::Bandlead(/*sp<ACodec> codec*/)
    : /*mCodec(codec), */mStatus(STATUS_STOP), mIsVsyncEnabled(false) {
    mLooper = new Looper(false);
    mLooper->addFd(mEventReceiver.getFd(), 0, Looper::EVENT_INPUT, this, nullptr);
}

Bandlead::~Bandlead() {
    if (mStatus == STATUS_RUNNING) 
        stop();
}

int Bandlead::handleEvent(int fd, int events, void* data) {
    DisplayEventReceiver::Event buffer[1];
    ssize_t n;
    static nsecs_t oldTimeStamp = 0;

    if (fd != mEventReceiver.getFd()) {
        printf("events:%x, data=%d\n", events, (data != NULL));
    }

    

    while ((n=mEventReceiver.getEvents(buffer, 1)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buffer[i].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC) {
                printf("count:%d",buffer[i].vsync.count);
            }
            if (oldTimeStamp) {
                float t = float(buffer[i].header.timestamp - oldTimeStamp) / s2ns(1);
                printf("interval:%f ms (%f HZ)\n", t*1000, 1.0/t);
            }
            oldTimeStamp = buffer[i].header.timestamp;
        }
    }
    return 1;
}

bool Bandlead::threadLoop() {
    int ret = mLooper->pollOnce(VSYNC_TIMEOUT);
    switch (ret) {
        case Looper::POLL_WAKE:
        printf("poll wake\n");
        break;
        case Looper::POLL_CALLBACK:
        //printf("poll call back\n");
        break;
        case Looper::POLL_TIMEOUT:
        //printf("poll timeout\n");
        break;
        case Looper::POLL_ERROR:
        printf("poll error\n");
        break;
        default:
        printf("poll ret:%d\n", ret);
        break;
    }

    if (mStatus != STATUS_RUNNING) {
        Mutex::Autolock _lock(mLock);
        mCond.signal();
        printf("exit threadLoop\n");
        return false;
    }

    return true;
}
void Bandlead::enableVsync() {
    mIsVsyncEnabled = true;
    mEventReceiver.setVsyncRate(1);
}

void Bandlead::disableVsync() {
    mIsVsyncEnabled = false;
    mEventReceiver.setVsyncRate(0);
}

void Bandlead::start() {
    if (!mStatus) {
        //set status running
        mStatus = STATUS_RUNNING;
        if (!mIsVsyncEnabled)
            enableVsync();
        //start
        run("bandlead",PRIORITY_DISPLAY);
    }
}

void Bandlead::stop() {
    if (mStatus == STATUS_RUNNING) {
        if (!mIsVsyncEnabled)
            enableVsync();

        Mutex::Autolock _lock(mLock);
        mStatus = STATUS_STOP;
        mCond.wait(mLock);
    }
    disableVsync();
    printf("stoped\n");
}

}
