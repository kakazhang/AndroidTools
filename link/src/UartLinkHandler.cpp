#define LOG_TAG "UartLinkHandler"

#include <stdio.h>
#include <utils/Log.h>
#include <utils/ThreadDefs.h>

#include "UartLinkHandler.h"

UartLinkHandler::UartLinkHandler(const char* serialDev,
        int speed, int flowCtrl,
        int databits,int stopbits,
        int parity, int timeout)
    : Link(timeout),mSerialDev(serialDev),
     mSpeed(speed), mFlowCtrl(flowCtrl), mDatabits(databits),
     mStopbits(stopbits), mParity(parity) {
    //serial device fd
    mFd = -1;

    //get tty type
    isTTYGS = (strncmp(mSerialDev, "/dev/ttyGS", strlen("/dev/ttyGS")) == 0);
    //is listening usb state
    isListening = false;
}

UartLinkHandler::~UartLinkHandler() {
    if (isTTYGS && isListening) {
        UsbObserver::getInstance()->removeListener(IObserver::id());
    }

    closeSerial();
}

void UartLinkHandler::init() {
    mLooper = new ALooper();
    mLooper->setName("UartLinkHandler");

    mLooper->start(false, //runOnCallingThread
                   true,  //canCallJava
                   PRIORITY_AUDIO/*priority of audio*/);

    //register message handler entry
    mMsgHandler = new UartMsgHandler(this);
    mLooper->registerHandler(mMsgHandler);

    //start listener
	start();
}

void UartLinkHandler::onFirstRef() {

    if (isTTYGS) {
        //get usb state, if usb is connected then open serial device
        int state = UsbObserver::getInstance()->getUsbState();
        //if usb is not connected
        if (!state) {
            //register UsbObserver and wait for notifications
            //when notification comes, and usb is connected open it
            UsbObserver::getInstance()->addListener(this);
            UsbObserver::getInstance()->start();
            isListening = true;
            return;
        }
    }

    //open serial device no matter what type this tty device is
}

/*FIX ME kakazhang 2018-01-11*/
void UartLinkHandler::notify() {
    int state = UsbObserver::getInstance()->getUsbState();
    if (state) {
        //open serial NEED TO REALIZE
    }
}

/*FIX ME kakazhang 2018-01-11*/
void UartLinkHandler::openSerial() {
    //close serial first before open it
    closeSerial();
    //open serial to get fd: mFd
    //add fd to listener : setUp(mFd);

}

void UartLinkHandler::closeSerial() {
    if (mFd > 0) {
        close(mFd);
        mFd = -1;
    }
}

/**
 * FIX ME kakazhang 2018-01-11
 * Messages received from uart have been parsed in Link
 * You need to set parameters of onEvent such as onEvent(UartMessage msg);
 * */
void UartLinkHandler::onEvent(void *buf, size_t count) {
   /**
    *You can do something like this:
    * sp<AMessage> msg = new AMessage(id, this);
    * msg->setXXX("XXX", YYY);
    * msg->post() or msg->postAndAwaitResponse()
    * */
}

void UartLinkHandler::callback(void *buf, int len) {

}

void UartLinkHandler::dump() {
    printf("%s anything can be dump here\n", __func__);
}

/**
 * Everything you want to do in ASYNC/SYNC way can be done here
 * @ sp<AMessage> msg = new AMessage(id, this);
 * @ msg->setPointer("@KeyName", XXXX);
 * @ msg->setXXX("XXX", YYY);
 * @ msg->post or  msg->postAndAwaitResponse but reposonse message must be announced
 * */
void UartMsgHandler::onMessageReceived(const sp<AMessage> &msg) {
    switch(msg->what()) {

    }

}
