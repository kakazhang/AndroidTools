#ifndef LINK_H
#define LINK_H

#include "Listener.h"

#define LISTENER_AS_CLIENT 0
#define LISTENER_AS_SERVER 1

class Link : public Listener {
public:
    Link(int timeout, int type = 0);
    virtual ~Link();

    void setup(int fd);

    virtual void init() {}

    void start();
    void stop();

    //handle events comes
    virtual void onDataAvailable(int fd);
    virtual void onEvent(void *buf, size_t count) = 0;

    //send events
    virtual int sendData(void *buf, size_t count);

    virtual void dump() {}
private:
    //true for exception ,false for nothing
    virtual bool handleException(int fd);
    virtual void handleTimeout() {}

    int mFd;
    int mClientFd;
    //first listener type
    int mListenerType;
};

#endif
