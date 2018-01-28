#ifndef LISTENER_H
#define LISTENER_H

#include <utils/RefBase.h>
#include <utils/threads.h>
#include <utils/Vector.h>

#include <sys/epoll.h>

using namespace android;

class Listener;

class ListenerThread : public Thread {
public:
    ListenerThread(sp<Listener> listener) {
        mListener = listener;
    }

    virtual ~ListenerThread() {}
private:
    virtual bool threadLoop();

    sp<Listener> mListener;
};

class Listener : virtual public RefBase {
public:
    Listener(int timeout);
    virtual ~Listener();

    /*
     * @fd: file descriptor to add
     * @inout: poll operation for this fd,true for in, false for out
     * */
    bool addListener(int fd, bool inout);
    bool removeListener(int fd);

    //start and stop Listener thread
    void startListener();
    void stopListener();

    //methods for calling callback
    virtual void onDataAvailable(int fd) = 0;
    virtual void handleTimeout() = 0;
private:
    friend class ListenerThread;

    bool waitForEvents();

    sp<ListenerThread> mThread;
    //epoll wait timeout
    int mTimeout;
    int mWakeFds[2];
    int eFd;
};

#endif
