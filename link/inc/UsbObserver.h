#ifndef USBOBSERVER_H
#define USBOBSERVER_H

#include <utils/RefBase.h>
#include <utils/Thread.h>
#include <utils/Mutex.h>
#include <utils/KeyedVector.h>

#define EPOLL_HINT 8
#define BUFLEN 1024

using namespace android;

class IObserver : virtual public RefBase {
public:
    virtual void notify() = 0;

    void setId(int id) {mId = id;}
    int id() {return mId;}
private:
    int mId;
};

class UsbObserver : public Thread {
public:
    virtual ~UsbObserver();
    static sp<UsbObserver> getInstance();

    void addListener(sp<IObserver> observer);
    void removeListener(int id);

    //start observer thread
    bool start();

    static int getUsbState();
private:
    UsbObserver();
    virtual bool threadLoop();

private:
    static Mutex gInitLock;
    static sp<UsbObserver> gInstance;

    volatile bool isStarted;
    //epoll fd
    int eFd;
    //usb fd
    int usbFd;
    volatile bool gUEventInit;

    Mutex mObserverLock;
    //observer count
    int mTotal;
    KeyedVector<int32_t, sp<IObserver>> mObservers;
};

#endif
