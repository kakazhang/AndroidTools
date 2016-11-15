#include <stdio.h>

#include "NetlinkHandler.h"

NetlinkHandler::NetlinkHandler(int sock) 
: NetlinkListener(sock){
	
}

NetlinkHandler::~NetlinkHandler() {
	
}

bool NetlinkHandler::start() {
	this->startListener();
	return true;
}

void NetlinkHandler::stop() {
	this->stopListener();
}

void NetlinkHandler::onEvent(const char* msg) {
	if (msg != NULL)
		printf("onEvent:%s\n",msg);
}

