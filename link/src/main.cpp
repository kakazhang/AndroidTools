#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "common.h"
#include "Listener.h"
#include "UsbObserver.h"
#include "LinkManager.h"
#include "TcpServerLinkHandler.h"

class UartListener : public Listener {
public:
    UartListener(): Listener(1000) {}
    virtual ~UartListener() {
    }

    virtual void onDataAvailable(int fd) {}
};

class TestObserver : public IObserver {
public:
    void notify() {
        printf("usb state update\n");
    }
    void startListener() {
        UsbObserver::getInstance()->addListener(this);
    }
    void stopListener() {
        UsbObserver::getInstance()->removeListener(IObserver::id());
    }
};

int main() {
    ProcessState::self()->startThreadPool();

    //sp<Listener> listener = new UartListener();
    //listener->startListener();
#if 0
    sp<TestObserver> obs = new TestObserver();
    obs->startListener();

    sleep(1);
    obs->stopListener();
#endif
    //listener->stopListener();
    sp<LinkManager> lm = LinkManager::getInstance();
    lm->dump();
#if 0
    SocketLinkAttr *attr = new SocketLinkAttr();
    attr->port = 40001;
    attr->address = "127.0.0.1";

    sp<TcpServerLinkHandler> sSocket = new TcpServerLinkHandler(attr);
#endif
    IPCThreadState::self()->joinThreadPool();
}
