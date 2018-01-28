#define LOG_TAG "Listener"

#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "Listener.h"

#define EPOLL_HINT 8

Listener::Listener(int timeout)
    : mTimeout(timeout) {
    eFd = epoll_create(EPOLL_HINT);
    if (eFd < 0) {
        ALOGE("epoll_create failed:%s", strerror(errno));
        exit(-1);
    }

    if (pipe(mWakeFds)) {
        ALOGE("Create pipe failed : %s", strerror(errno));
        exit(-1);
    }
    addListener(mWakeFds[0], true);
}

Listener::~Listener() {
    for (int i = 0 ; i < 2; i++)
        close(mWakeFds[i]);

    if (eFd > 0)
        close(eFd);
}

bool Listener::addListener(int fd, bool inout) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = (inout?EPOLLIN:EPOLLOUT);

    if (epoll_ctl(eFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        ALOGE("epoll add %d failed:%s", fd, strerror(errno));
        return false;
    }

    return true;
}

bool Listener::removeListener(int fd) {
    if (epoll_ctl(eFd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        if (errno == EBADF || errno == ENOENT) {
            ALOGD("%p ~ removeFd - EPOLL_CTL_DEL failed due to file descriptor "
                "being closed: %s", this, strerror(errno));
        } else {
            ALOGE("Error removing epoll events for fd %d: %s", fd, strerror(errno));
            return false;
        }
    }
    return true;
}

void Listener::startListener() {
    mThread = new ListenerThread(this);
    mThread->run("Listener");
}

void Listener::stopListener() {
    char cmd[] = {"exit"};
    int len = strlen(cmd);

    if (write(mWakeFds[1], cmd, len+1) < 0) {
        ALOGE("exit failed:%s", strerror(errno));
    } else {
        ALOGD("exit successful");
        printf("exit successful\n");
    }
}

bool Listener::waitForEvents() {
    struct epoll_event events[EPOLL_HINT];
    int nfds;

    for (;;) {
        nfds = epoll_wait(eFd, events, EPOLL_HINT,mTimeout);
        if (nfds > 0) {
            for (int i = 0; i < nfds; i++) {
                uint32_t epollEvents = events[i].events;
                int fd = events[i].data.fd;
                if (fd == mWakeFds[0]) {
                    if (epollEvents & EPOLLIN) {
                        ALOGD("broke this wait loop");
                        printf("broke this wait loop\n");
                        goto out;
                    } else {
                        ALOGW("Ignoring unexpected epoll events 0x%x on wake event fd.", epollEvents);
                    }
                } else {
                    onDataAvailable(fd);
                }
            }
        } else if (nfds == 0) {
            ALOGV("timeout");
       } else {
           if (errno == EINTR) goto out;
           ALOGE("Poll failed with unexcepted exception:%s", strerror(errno));
           goto out;
       }
    }

out:
    return false;
}

bool ListenerThread::threadLoop() {
    return mListener->waitForEvents();
}
