#include <stdio.h>
#include <unistd.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "Bandlead.h"

using namespace android;

int main() {
    ProcessState::self()->startThreadPool();

    sp<Bandlead> bandLead = new Bandlead();
    bandLead->start();

    sleep(2);
    bandLead->disableVsync();
    
    //sleep(2);
    //bandLead->enableVsync();
    
    sleep(2);
    bandLead->stop();

    IPCThreadState::self()->joinThreadPool();
    return 0;
}

