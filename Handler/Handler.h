#ifndef HANDLER_H
#define HANDLER_H

#include <utils/Errors.h>
#include <utils/Looper.h>

#include "Barrier.h"
#define MSG_INIT 0

namespace android {

class Handler : public MessageHandler {
public:
    Handler(sp<Looper> looper);
    virtual ~Handler();
    virtual void handleMessage(const Message& message);

    void handle(const Message& message);
    status_t sendMessageAsync(const Message& message);
    status_t sendMessageSync(const Message& message);

private:
	sp<Looper> mLooper;
    Barrier mBarrier;
};

};

#endif
