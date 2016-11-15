#ifndef NETLINKLISTENER_H
#define NETLINKLISTENER_H
#include "SocketListener.h"

class NetlinkListener : public SocketListener {
public:
	NetlinkListener(int sock);
	virtual ~NetlinkListener();

	virtual void onAvailable(int fd);
	virtual void onEvent(const char* msg) = 0;
};
#endif
