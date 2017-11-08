/*
* Copyright (C) 2017 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*	   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef VSYNC_H
#define VSYNC_H

#include <gui/DisplayEventReceiver.h>

//#include <media/stagefright/ACodec.h>

#include <utils/Looper.h>
#include <utils/threads.h>

#include <utils/Mutex.h>
#include <utils/Condition.h>

#define STATUS_STOP 0
#define STATUS_RUNNING 1

#define VSYNC_TIMEOUT -1

namespace android {

class  Bandlead : public Thread, public LooperCallback {
public:
    Bandlead(/*sp<ACodec> codec*/);
    virtual ~Bandlead();
    virtual int handleEvent(int fd, int events, void* data);

    void start();
    void stop();

    void enableVsync();
    void disableVsync();
protected:
	virtual bool threadLoop();

private:
    //sp<ACodec> mCodec;
    volatile bool mIsVsyncEnabled;
    volatile bool mStatus;
    sp<Looper> mLooper;
    Mutex mLock;
    Condition mCond;

    DisplayEventReceiver mEventReceiver;
};

}
#endif
