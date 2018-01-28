#ifndef TCPSERVERLINKHANDLER_H
#define TCPSERVERLINKHANDLER_H

#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ALooper.h>

#include "common.h"
#include "Link.h"
#include "ILinkEventCallback.h"
class TcpServerLinkHandler;

class TcpServerMsgHandler : public AHandler {
public:
    TcpServerMsgHandler(sp<TcpServerLinkHandler> tcpServer) : mTcpServer(tcpServer) {}
    virtual ~TcpServerMsgHandler() {}
protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);
private:
    sp<TcpServerLinkHandler> mTcpServer;
};

class TcpServerLinkHandler : public Link {
public:
    TcpServerLinkHandler(SocketLinkAttr* attr);
    virtual ~TcpServerLinkHandler();
    virtual void init();

    virtual void onEvent(void *buf, size_t count);
    //callback from LinkEvent
    virtual void callback(void *buf, int len);
    virtual void dump();

private:
    virtual void onFirstRef();
    int createSocketInaddrAnyServer(int port, int type);
    virtual bool handleException(int fd);

    sp<ALooper> mLooper;
    SocketLinkAttr* mSockAttr;
    int mServerFd;

    friend class TcpServerMsgHandler;
    sp<TcpServerMsgHandler> mMsgHandler;
};

#endif
