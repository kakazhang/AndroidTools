#include <stdio.h>
#include <stdlib.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include <utils/String8.h>

#include "recorder.h"

using namespace android;

int main(int argc, char** argv) {
    if (argc < 1) {
        printf("no output\n");
        exit(-1);
    }
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    String8 name(argv[1]);
    sp<Recorder> recorder = new Recorder(name);
    recorder->start();

    //sleep(10);
    //recorder->stop();
   
    IPCThreadState::self()->joinThreadPool();
}
