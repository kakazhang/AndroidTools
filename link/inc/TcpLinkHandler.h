#ifndef TCPLINKHANDLER_H
#define TCPLINKHANDLER_H

#include <utils/Looper.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>

#include "common.h"
#include "Link.h"
#include "ILinkEventCallback.h"

#define MSG_HANDLE_PACKET 0

using namespace android;

class TcpLinkHandler;
class TcpMsgHandler : public AHandler {
public:
    TcpMsgHandler(sp<TcpLinkHandler> tcp) : mTcp(tcp) {}
    virtual ~TcpMsgHandler() {}
protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);
private:
    sp<TcpLinkHandler> mTcp;
};

class TcpLinkHandler : public Link, public ILinkEventCallback {
public:
    TcpLinkHandler(SocketLinkAttr* socketAttr);
    virtual ~TcpLinkHandler();
    virtual void init();

    virtual void onEvent(void* buf, size_t count);

    virtual void callback(void *buf, int len);
    virtual void dump();

private:
    virtual void onFirstRef();
    int createSocketClient(int port, int type);
    virtual bool handleException(int fd);
    virtual void handleTimeout();
	//check if socket is connected
    bool isConnected();

    int mSock;
    SocketLinkAttr* mSocketAttr;

    sp<ALooper> mLooper;

    friend class TcpMsgHandler;
    sp<TcpMsgHandler> mMsgHandler;
};

#endif
