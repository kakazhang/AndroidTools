#ifndef SOCKETLISTENER_H
#define SOCKETLISTENER_H
#include <unistd.h>
#include <sys/epoll.h>
/*
*This class is designed to listen to socket messages from others(kernel or user) 
*and send to whoever want the message.
*/
class SocketListener {
public:
	SocketListener(int sock, bool listen);
	virtual ~SocketListener();

	virtual void onAvailable(int fd) = 0;
	void startListener();
	void stopListener();
	
private:
	static void* threadListener(void* args);
	void runListener();
private:
	int mSock;
	bool bListen;
	int pipeFds[2];
	int mEpollFd;
};

#endif