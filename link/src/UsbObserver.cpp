#define LOG_TAG "UsbObserver"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <utils/Log.h>

#include "UsbObserver.h"
#include "hardware_legacy/uevent.h"

#define STR_MATCH "/devices/virtual/android_usb/android0"
#define USB_STATE "/sys/class/android_usb/android0/state"

#define USB_DISCONNECTED "DISCONNECTED"

using namespace android;

sp<UsbObserver> UsbObserver::gInstance = NULL;
Mutex UsbObserver::gInitLock;
UsbObserver::UsbObserver() {
    eFd = epoll_create(EPOLL_HINT);
    if (eFd < 0) {
        perror("epoll_create");
        exit(-1);
    }

    if (uevent_init())
        gUEventInit = true;

    isStarted = false;
    mTotal = 0;
}

UsbObserver::~UsbObserver() {
    if (eFd > 0)
        close(eFd);
}

sp<UsbObserver> UsbObserver::getInstance() {
    if (gInstance == NULL) {
        Mutex::Autolock __lock(gInitLock);
        gInstance = new UsbObserver();
    }

    return gInstance;
}

void UsbObserver::addListener(sp<IObserver> observer) {
    Mutex::Autolock __lock(mObserverLock);
    observer->setId(mTotal++);
    mObservers.add(observer->id(),observer);
}

void UsbObserver::removeListener(int id) {
    Mutex::Autolock __lock(mObserverLock);
    printf("remove %d\n", id);
    mObservers.removeItem(id);
    mTotal--;
}

bool UsbObserver::start() {
    if (!isStarted)
        run("UsbObserver");

    return true;
}

bool UsbObserver::threadLoop() {
    char buffer[BUFLEN] = {0};

    while (1) {
        int length = uevent_next_event(buffer, sizeof(buffer) - 1);
        if (length <= 0)
            return false;

        buffer[length] = '\0';

        if (strstr(buffer, STR_MATCH)) {
            printf("kaka (%d) %s\n",length,buffer);
            Mutex::Autolock __lock(mObserverLock);
            for (int i = 0; i < mTotal; i++) {
                sp<IObserver> obs = mObservers.valueAt(i);
                obs->notify();
            }
        }
    }
    return true;
}

int UsbObserver::getUsbState() {
    //0 for disconnected
    int state = 0;
    int ret;
    int fd;
    char cmd[16] = {0};

    fd = open(USB_STATE, O_RDONLY);
    if (fd < 0) {
        ALOGE("open usb state failed:%s", strerror(errno));
        return -1;
    }

    ret = read(fd, cmd, sizeof(cmd));
    if (ret < 0) {
        ALOGE("read usb state failed:%s", strerror(errno));
        goto out;
    }
    if (0 != strcmp(cmd, USB_DISCONNECTED))
        //1 for usb non-disconnected
        state = 1;
out:
    if (fd > 0) close(fd);

    return state;
}
