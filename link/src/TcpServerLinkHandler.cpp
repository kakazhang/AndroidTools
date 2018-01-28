#define LOG_TAG "TcpServerLinkHandler"

#include <utils/Log.h>

#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>

#include "TcpServerLinkHandler.h"

#define LISTEN_BACKLOG 4
#define BUFLEN 128

using namespace android;

TcpServerLinkHandler::TcpServerLinkHandler(SocketLinkAttr* attr)
    : Link(1000, LISTENER_AS_SERVER) {
    mSockAttr = attr;
    mServerFd = createSocketInaddrAnyServer(attr->port, SOCK_STREAM);
    setup(mServerFd);
}

TcpServerLinkHandler::~TcpServerLinkHandler() {
    if (mServerFd > 0) {
        removeListener(mServerFd);
        close(mServerFd);
        mServerFd = -1;
    }

    stop();
}

void TcpServerLinkHandler::onFirstRef() {
    if (mServerFd > 0)
        setup(mServerFd);
}

void TcpServerLinkHandler::init() {
    //looper for handle message
    mLooper = new ALooper();
    mLooper->setName("TcpServerLinkHandler");
    mLooper->start(false,     //not allow to run on current thread
                   true,     //can call java
                   -2);      //priority
    //register message handler:void onMessageReceived(const sp<AMessage> &msg)
    mMsgHandler = new TcpServerMsgHandler(this);
    mLooper->registerHandler(mMsgHandler);

    //start listener
    start();
}

void TcpServerLinkHandler::onEvent(void *buf, size_t count) {
#ifdef DebugTcpServerLinkHandler
    printf("%s received %s\n", __func__, (char *)buf);
#endif
    char reply[BUFLEN] = {"hello,this is from server"};
    sendData(reply, strlen(reply)+1);
}

int TcpServerLinkHandler::createSocketInaddrAnyServer(int port, int type) {
   struct sockaddr_in addr;
   int s, n;

   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port        = htons(port);

   s = socket(AF_INET, type, 0);
   if (s < 0) {
       ALOGE("create socket failed:%s", strerror(errno));
       return -1;
   }

   n = 1;
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));
   if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
       close(s);
       ALOGE("bind failed:%s", strerror(errno));
       return -1;
   }

   int ret = listen(s, LISTEN_BACKLOG);
   if (ret < 0) {
       close(s);
       ALOGE("listen failed:%s", strerror(errno));
   }

   return s;
}

bool TcpServerLinkHandler::handleException(int fd) {
   bool excepted = false;
   char buf[BUFLEN] = {0};
   int count = recv(fd, buf, BUFLEN, 0);

   if ((count == 0 && errno != EINTR)
         || (count < 0 && errno != EAGAIN)) {
      excepted = true;
   }
   if (excepted) {
      printf("exception in client\n");
   }
   return excepted;
}

void TcpServerLinkHandler::callback(void *buf, int len) {

}

void TcpServerLinkHandler::dump() {
    printf("%s anything can be dump here\n", __func__);
}

void TcpServerMsgHandler::onMessageReceived(const sp<AMessage> &msg) {

}
