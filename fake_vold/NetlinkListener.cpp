#include <unistd.h>

#include "NetlinkListener.h"

NetlinkListener::NetlinkListener(int sock) 
	: SocketListener(sock,false){
	
}

NetlinkListener::~NetlinkListener() {
	
}

void NetlinkListener::onAvailable(int fd) {
	char buffer[1024] = {0};

	int count = read(fd, buffer, sizeof(buffer));
	if (count > 0)
		onEvent(buffer);
}

