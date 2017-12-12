#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "recorder.h"

using namespace android;

RecorderHandler::RecorderHandler(sp<MediaRecorder>& recorder, String8& output) 
  :mOutput(output) {
    mRecorder = recorder;
    mLooper = new Looper(false);
}

RecorderHandler::~RecorderHandler() {

}

void RecorderHandler::startRecord() {
    //set audio source as MIC
    mRecorder->setAudioSource(1);
    //AMR_NB
    mRecorder->setOutputFormat(1);
    //3gp
    mRecorder->setAudioEncoder(1);
	
	//set max file size
    char params[64];
    sprintf(params, "max-filesize=%" PRId64, 100*1024);

    mRecorder->setParameters(String8(params));
	///sdcard/
    const char* name = mOutput.string();
    fd = open(name, O_CREAT|O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(-1);
    }
    mRecorder->setOutputFile(fd, 0, 0);
    mRecorder->prepare();
	
    mRecorder->start();
}

void RecorderHandler::handleMessage(const Message& message) {
    switch(message.what) {
	case MSG_START:
       printf("start\n");
       startRecord();
       //mRecorder->setStatus(true);
		break;
	case MSG_STOP:
       printf("stop\n");
		mRecorder->stop();
       //mRecorder->setStatus(false);
		break;
	default:
		break;
	}
}

void RecorderHandler::sendMessage(const Message& message) {
    mLooper->sendMessage(this, message);
}

bool RecorderHandler::threadLoop() {
    mLooper->pollOnce(-1);
    return true;
}

Recorder::Recorder(String8& output) {
    sp<MediaRecorder> recorder = new MediaRecorder(String16("kaka"));
    mHandler = new RecorderHandler(recorder, output);
    mHandler->run("Recorder");
    mStart = false;
}

Recorder::~Recorder() {

}

void Recorder::start() {
    setStatus(true);
    mHandler->sendMessage(Message(MSG_START));
}

void Recorder::stop() {
    if (mStart) {
        setStatus(false);
        mHandler->sendMessage(Message(MSG_STOP));
    }
}
