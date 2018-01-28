#define LOG_TAG "TcpLinkHandler"

#include <string.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utils/Log.h>
#include <sys/ioctl.h>

#include "TcpLinkHandler.h"
#include "LinkEvent.h"

TcpLinkHandler::TcpLinkHandler(SocketLinkAttr* socketAttr)
    :Link(1000) {
    mSocketAttr = socketAttr;
    mSock = createSocketClient(mSocketAttr->port, SOCK_STREAM);
}

TcpLinkHandler::~TcpLinkHandler() {
    if (mSock > 0) {
        removeListener(mSock);
        close(mSock);
        //stop listener
        stop();
    }
}

void TcpLinkHandler::init() {
    //looper for handle message
    mLooper = new ALooper();
    mLooper->setName("TcpLinkHandler");
    mLooper->start(false,     //not allow to run on current thread
                   true,     //can call java
                   -2);      //priority

    //register message handler:void onMessageReceived(const sp<AMessage> &msg)
    mMsgHandler = new TcpMsgHandler(this);
    mLooper->registerHandler(mMsgHandler);

    //start listener
	printf("%s start listener\n", __func__);
    start();
}

void TcpLinkHandler::onFirstRef() {
    if (mSock > 0) {
        setup(mSock);
    }
}

int TcpLinkHandler::createSocketClient(int port, int type) {
    struct hostent* host;
    struct sockaddr_in saddr;

    bzero(&saddr,sizeof(struct sockaddr_in));
    saddr.sin_addr.s_addr = inet_addr(mSocketAttr->address);
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);

    int s = socket(AF_INET,type,0);
    if (s < 0) {
        ALOGE("create socketed failed:%s", strerror(errno));
        return -1;
    }

	unsigned long mode = 1;
    ioctl(s, FIONBIO, &mode);
    int ret = connect(s, (struct sockaddr *)&saddr,  sizeof(struct sockaddr));
    if (ret < 0) {
        ALOGE("connect to %s failed:%s", mSocketAttr->address, strerror(errno));
        close(s);
        return -1;
    }

    return s;
}

void TcpLinkHandler::onEvent(void* buf, size_t count) {
#ifdef DebugTcpLinkHandler
    printf("%s received :%s\n", __func__, (char *)buf);
#endif
    //Use LinkEvent to parse data
    sp<LinkEvent> linkEvent = new LinkEvent(this);
    linkEvent->parseData((uint8_t *)buf,count);
}

bool TcpLinkHandler::handleException(int fd) {
    if (!isConnected()) {
        printf("disconnected\n");
        removeListener(mSock);
        close(mSock);
        stop();
    }
    return true;
}

void TcpLinkHandler::handleTimeout() {
    if (!isConnected()) {
       mSock = createSocketClient(mSocketAttr->port, SOCK_STREAM);
	   if (mSock > 0)
           setup(mSock);
	}
}

bool TcpLinkHandler::isConnected() {
    if(mSock < 0)
        return false;

    do {
        char a;
        int ret = recv(mSock, &a, 1, MSG_PEEK|MSG_DONTWAIT);
        if(ret > 0) return true;

        if(ret == -1) {
            if(errno == EAGAIN) return true;

            if(errno == EINTR) {
                usleep(1);
                continue;
            }
            return false;
        }

        break;
    } while(1);

    return false;
}

void TcpLinkHandler::callback(void *buf, int len) {
    sp<AMessage> msg = new AMessage(MSG_HANDLE_PACKET, mMsgHandler);
    msg->setPointer("packet", buf);
    msg->setInt32("packetLen", len);

    printf("callback:%s\n", (char *)buf);
    msg->post();
}

void TcpLinkHandler::dump() {
    printf("%s anything can be dump here\n", __func__);
}

void TcpMsgHandler::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
    case MSG_HANDLE_PACKET:
        int len;
        uint8_t cmd[20*1024];
        msg->findInt32("packetLen", &len);
        msg->findPointer("packet", (void **)&cmd);
        
        break;
    default:
        break;
    }
}
