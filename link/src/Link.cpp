#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "Link.h"
#define BUFLEN 128

Link::Link(int timeout, int type)
    : Listener(timeout) {
    mListenerType = type;
}

Link::~Link() {
}

//for main listener
void Link::setup(int fd) {
    mFd = fd;
    addListener(fd, true);
}

void Link::start() {
    startListener();
}

void Link::stop() {
    stopListener();
}

void Link::onDataAvailable(int fd) {
    /**
     * If data available,this is callback ,
     * and EventHandler is used to parse data,
     * then,we can deal with events
     * EventHandler will be realized later
     * */
    if ((mListenerType == LISTENER_AS_SERVER)
        && (fd == mFd)) {
        //accept client connect request and then add client fd for epoll wait
        mClientFd = accept(mFd, NULL, NULL);
        addListener(mClientFd,true);
#ifdef DebugLink
    printf("accept :%d\n", mClientFd);
#endif
        return;
    }

    //when events come,read it and pass to child
    char buf[BUFLEN] = {0};
    int cfd = (mListenerType ? mClientFd : mFd);
    if (fd == cfd) {
        int count = read(fd, buf, BUFLEN);
        if (count > 0) {
            onEvent(buf, count);
        } else {
            if (handleException(fd)) {
                if (mListenerType == LISTENER_AS_SERVER) {
                    removeListener(mClientFd);
                    close(mClientFd);
                    mClientFd = -1;
                }
            }
        }
    }
}

int Link::sendData(void* buf, size_t count) {
#ifdef DebugLink
    printf("send data:%s\n", (char*)buf);
#endif
    int fd = -1;
    fd = (mListenerType ? mClientFd : mFd);
    int ret =write(fd, buf, count);
    if (ret < 0) perror("write");

    return ret;
}

bool Link::handleException(int fd) {
    return false;
}
