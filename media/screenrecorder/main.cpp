#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "ScreenRecorder.h"

using namespace android;

int main() {
	ProcessState::self()->startThreadPool();
	const char* kmimeTypeAvc = "video/avc";
	uint32_t gBitRate = 4000000;     // 4Mbps
    const char* path = "/sdcard/test.mp4";
	
    sp<ScreenRecorder> recorder = new ScreenRecorder(kmimeTypeAvc,gBitRate, path);
    recorder->start();
	sleep(30);
	recorder->stop();
	IPCThreadState::self()->joinThreadPool();
}
