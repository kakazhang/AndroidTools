#ifndef RECORDER_H
#define RECORDER_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <media/mediarecorder.h>

#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/threads.h>

using namespace android;
enum {
	MSG_START = 0,
	MSG_STOP,
};

class RecorderHandler : public MessageHandler, public Thread {
public:
	RecorderHandler(sp<MediaRecorder>& recorder);
	virtual ~RecorderHandler();
	virtual void handleMessage(const Message& message);

	void sendMessage(const Message& message);
private:
	virtual bool threadLoop();

    void startRecord();
private:
    int fd;
	sp<MediaRecorder> mRecorder;
	sp<Looper> mLooper;
};

class Recorder : public RefBase {
public:
    Recorder();
    virtual ~Recorder();

    void start();
	void stop();

    void setStatus(int start) {
        mStart = start;
    }
private:
    volatile bool mStart;
	sp<RecorderHandler> mHandler;
};

#endif
