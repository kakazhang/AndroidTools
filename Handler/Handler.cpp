#include <unistd.h>

#include <utils/Looper.h>

#include "Handler.h"
#include "Barrier.h"

using namespace android;

Handler::Handler(sp<Looper> looper) {
    mLooper = looper;
}

Handler::~Handler() {

}

void Handler::handleMessage(const Message& message) {
    handle(message);
    mBarrier.open();
}

void Handler::handle(const Message& message) {
    switch(message.what) {
    case MSG_INIT:
		printf("message init\n");
		sleep(2);
		break;
	default:
		break;
    }
}

status_t Handler::sendMessageAsync(const Message& message) {
    mLooper->sendMessage(this, message);
    return NO_ERROR;
}

status_t Handler::sendMessageSync(const Message& message) {
    mLooper->sendMessage(this, message);
    mBarrier.wait();

    return NO_ERROR;
}
