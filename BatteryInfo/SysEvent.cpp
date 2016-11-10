#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <fcntl.h>

#include "SysEvent.h"
using namespace std;
namespace android {

using namespace std;
using namespace android;
#define BUFSIZE 64*1024

int createUEvent() {
#if 0
	int sock;
    struct sockaddr_nl nladdr;
    int sz = BUFSIZE;
    int on = 1;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0xffffffff;

    if ((sock = socket(PF_NETLINK,
                        SOCK_DGRAM,NETLINK_KOBJECT_UEVENT)) < 0) {
        perror("socket");
        return sock;
    }

	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
        perror("setsockopt");
        goto out;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        goto out;
    }

    if (bind(sock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
        perror("bind");
        goto out;
    }
	
	fcntl(sock, F_SETFL, O_NONBLOCK);

out:
	return sock;
#else
	return uevent_open_socket(BUFSIZE, true);
#endif

}
}
