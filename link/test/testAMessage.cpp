#include <utils/Looper.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ALooper.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

using namespace android;

class Handler : public AHandler {
public:
    Handler();
    virtual ~Handler();
    void init();
    void send(int what);
    void sendInt(int what, int id);
    void sendPointer(int what, void *value);
protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    sp<ALooper> mLooper;
};

Handler::Handler() {
    mLooper = new ALooper();
    mLooper->setName("Alooper test");

    mLooper->start(false,//no allow to run on current thread
            true,     //can call java
            -2);      //priority
}

Handler::~Handler() {

}

void Handler::init() {
    mLooper->registerHandler(this);
}

void Handler::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case 0:
            printf("message received\n");
            break;
        case 1:
            int id;
            msg->findInt32("id", &id);
            printf("id:%d\n", id);
            break;
        case 2:
            char* buf;
            if (msg->findPointer("key", (void**)&buf))
                printf("buf:%s\n",buf);
            break;
        default:
            break;
    }
}

void Handler::send(int what) {
    sp<AMessage> msg = new AMessage(what, this);
    msg->post();
}

void Handler::sendInt(int what, int id) {
   sp<AMessage> msg = new AMessage(what, this);
   msg->setInt32("id", id);

   msg->post();
}

void Handler::sendPointer(int what, void *value) {
   sp<AMessage> msg = new AMessage(what, this);
   msg->setPointer("key", value);

   msg->post();
}

int main() {
    ProcessState::self()->startThreadPool();

    sp<Handler> handle = new Handler();
    handle->init();
    handle->send(0);
    handle->sendInt(1, 1);

    char buf[] = {"hello,world"};
    handle->sendPointer(2, (void*)buf);

    IPCThreadState::self()->joinThreadPool();
}
