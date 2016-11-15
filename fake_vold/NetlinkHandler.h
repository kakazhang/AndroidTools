#ifndef NETLINKHANDLER_H
#define NETLINKHANDLER_H
#include "NetlinkListener.h"

class NetlinkHandler : public NetlinkListener {
public:
	NetlinkHandler(int sock);
	virtual ~NetlinkHandler();

	bool start();
	void stop();

	virtual void onEvent(const char* msg);
};
#endif