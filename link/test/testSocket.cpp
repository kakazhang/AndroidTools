#include <stdio.h>
#include <string.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "TcpLinkHandler.h"

using namespace android;

int main() {
    ProcessState::self()->startThreadPool();

    SocketLinkAttr* attr = new SocketLinkAttr();
    attr->port = 40005;
    attr->address = "127.0.0.1";
    sp<TcpLinkHandler> handler = new TcpLinkHandler(attr);
    handler->init();

    char buf[128] = {"hello, message from client"};
    handler->sendData(buf, strlen(buf)+1);

    IPCThreadState::self()->joinThreadPool();
}

