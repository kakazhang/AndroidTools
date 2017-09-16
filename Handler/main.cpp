#include <stdio.h>
#include <stdlib.h>

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

#include <utils/Thread.h>
#include <utils/Looper.h>

#include "Handler.h"

using namespace android;

class HandlerThread : public Thread {
public:
    HandlerThread(sp<Looper> looper) {
        mLooper = looper;
    }

    virtual ~HandlerThread() {}
private:
    virtual bool threadLoop() {
        do {
            mLooper->pollOnce(-1);
        } while (true);

        return true;
    }

    sp<Looper> mLooper;
};

void test() {
    sp<Looper> looper = new Looper(true);
    sp<HandlerThread> thread = new HandlerThread(looper);
    thread->run("HandlerThread");
    sp<Handler> handler = new Handler(looper);

    sleep(1);
    Message msg;
    msg.what = MSG_INIT;
    handler->sendMessageSync(msg);
	printf("done\n");
}

int main() {
    ProcessState::self()->startThreadPool();
    test();	
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
