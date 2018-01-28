#ifndef UARTLINKHANDLER_H
#define UARTLINKHANDLER_H

#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ALooper.h>

#include "Link.h"
#include "UsbObserver.h"
#include "ILinkEventCallback.h"
using namespace android;

//msg ids
enum {
    MSG_OPEN_TTYG = 0,
};

class UartLinkHandler;
class UartMsgHandler : public AHandler {
    public:
        UartMsgHandler(sp<UartLinkHandler> uart):mUart(uart) {
        }
        virtual ~UartMsgHandler() {}
        virtual void onMessageReceived(const sp<AMessage> &msg);
    private:
        sp<UartLinkHandler> mUart;
};

class UartLinkHandler : public Link, public IObserver, public ILinkEventCallback {
public:
    UartLinkHandler(const char* serialDev,
            int speed, int flowCtrl,
            int databits,int stopbits,
            int parity, int timeout);

    virtual ~UartLinkHandler();
    virtual void onFirstRef();

    virtual void init();

    //event handler entry
    virtual void onEvent(void *buf, size_t count);

    //UsbObserver notify
    virtual void notify();

    //callback from LinkEvent
    virtual void callback(void* buf, int len);

    virtual void dump();
private:
    void openSerial();
    void closeSerial();

    friend class UartMsgHandler;
    sp<UartMsgHandler> mMsgHandler;
private:
    sp<ALooper> mLooper;

    //Serial device configurations
    const char* mSerialDev;
    int mSpeed;
    int mFlowCtrl;
    int mDatabits;
    int mStopbits;
    int mParity;

    //serial device fd
    int mFd;
    //ttyGS type
    bool isTTYGS;

    //listen for usb state
    bool isListening;
};

#endif
