#define LOG_TAG "Commands"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cutils/sockets.h>
#include <cutils/log.h>
#include <sys/stat.h>
#include <errno.h>
 #include <string.h>

#include "ICommand.h"
#include "Commands.h"

#define BUFFER_MAX    1024
#define SOCKET_PATH "optimise"

//using namespace android;

static ICommand* cpufreq = NULL;
static ICommand* cgroup = NULL;
static ICommand* reclaim = NULL;
static ICommand* scheds = NULL;

void __attribute__((constructor)) init() {
    cpufreq = new CpufreqCmd("cpufreq");
    cgroup = new CgroupCmd("cgroup");
    reclaim = new ReclaimCmd("reclaim");
	scheds = new SchedCmd("sched");
}

void __attribute__((destructor)) destroy() {
    if (cpufreq)
		delete cpufreq;

	if (cgroup)
		delete cgroup;

    if (reclaim)
        delete reclaim;

    if (scheds)
        delete scheds;
}

int parse(char* msg, char **args, int len) {
    const char* delim = " ";
	int count = 0;

	char *str = strtok(msg, delim);
	args[count++] = strdup(str);
	
	while(str != NULL) {
	  str = strtok(NULL, delim);
	  if (str)
	      args[count++] = strdup(str);

	  if (count > len)
	  	break;
	}

    return count;
}

static int writex(int s, const void *_buf, int count) {
    const char *buf =(const char*) _buf;
    int n = 0, r;
    if (count < 0) return -1;
    while (n < count) {
        r = write(s, buf + n, count - n);
        if (r < 0) {
            if (errno == EINTR) continue;
            ALOGE("write error: %s\n", strerror(errno));
            return -1;
        }
        n += r;
    }
    return 0;
}

static int execute(char msg[BUFFER_MAX], const int count) {
    int argc = count;
	char *argv[count];
	
	parse(msg, argv, argc);
	ALOGE("args[0]:%s\n", argv[0]);

	if (!strcmp(argv[0], cpufreq->getCmdName()))
		cpufreq->onCommand(argc,argv);
	else if (!strcmp(argv[0], cgroup->getCmdName()))
		cgroup->onCommand(argc, argv);
    else if (!strcmp(argv[0], reclaim->getCmdName()))
		reclaim->onCommand(argc, argv);
	else if (!strcmp(argv[0], scheds->getCmdName()))
		scheds->onCommand(argc, argv);

    for (int i = 0; i < count; i++) {
		if (argv[i])
            free(argv[i]);
	}

	return 0;
}

int change_mode(int mode) {
    return chmod("/dev/socket/optimise", mode);
}

int main(int argc, char** argv) {
    char buf[BUFFER_MAX];
    struct sockaddr addr;
    socklen_t alen;
	int lsocket, s;
	int size;
	const int LEN = sizeof(int);
    int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH | S_IWOTH;

    lsocket = android_get_control_socket(SOCKET_PATH);
    if (lsocket < 0) {
        ALOGE("Failed to get socket from environment: %s\n", strerror(errno));
    } else {
       lsocket = socket_local_server(SOCKET_PATH, ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
	   if (lsocket < 0) {
           ALOGE("socket create failed");
		   exit(-1);
	   }
	}
	
    if (change_mode(mode)) {
        ALOGE("%s\n", strerror(errno));
        return -1;
    }

    if (listen(lsocket, 5)) {
        ALOGE("Listen on socket failed: %s\n", strerror(errno));
        exit(1);
    }

    fcntl(lsocket, F_SETFD, FD_CLOEXEC);

    for (;;) {
        alen = sizeof(addr);
        s = accept(lsocket, &addr, &alen);
        if (s < 0) {
            ALOGE("Accept failed: %s\n", strerror(errno));
            continue;
        }
        fcntl(s, F_SETFD, FD_CLOEXEC);

        //ALOGD("new connection\n");
        for (;;) {
            char line[LEN] = {0};
            if (read(s, line, LEN) < 0) {
                ALOGE("failed to read size\n");
                break;
            }

			int count = atoi(line);
            if ((count < 1) || (count >= BUFFER_MAX)) {
                ALOGE("invalid size %d\n", count);
                break;
            }
            memset(buf, 0, sizeof(buf));
            if ((size=read(s, buf, sizeof(buf))) < 0) {
                ALOGE("failed to read command\n");
                break;
            }
            buf[size] = 0;
            ALOGD("cmd:%s\n", buf);
            if (execute(buf, count)) break;
        }
        //ALOGE("closing connection\n");
        close(s);
    }

	return 0;
}
