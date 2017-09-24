#include <stdio.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "recorder.h"

using namespace android;

int main() {
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    sp<Recorder> recorder = new Recorder();
    recorder->start();

    sleep(10);
    recorder->stop();
   
    IPCThreadState::self()->joinThreadPool();
}
