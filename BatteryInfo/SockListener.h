#ifndef SOCKLISTENER_H
#define SOCKLISTENER_H

#include <map>
#include <utils/String8.h>
#include <utils/Thread.h>

#include "IListener.h"
using namespace std;

typedef void (*Callback)(int);
namespace android {
class SockListener : public Thread {
public:
	SockListener(int sock);
	virtual ~SockListener();

	bool addListener(int fd, Callback callback);
	int startListener();
	void stopListener();
	
	virtual void registerListener(String8 name, sp<IListener> listener) = 0;
protected:
	bool init();
	virtual void onEvent(int fd) = 0;
private:
	virtual bool threadLoop();
	void waitForEvent();
private:
	int mEpollFd;
	/*@mWakeFds[0] for read
	* @mWakeFds[1] for write
	*/
	int mWakeFds[2];
	map<int, Callback> mCallbacks;
	int mSock;
};
}
#endif
