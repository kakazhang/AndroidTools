#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/eventfd.h>

#define CGROUP_CONTROL "/sys/fs/cgroup/memory/limit50M/cgroup.event_control"
#define CGROUP_OOM "/sys/fs/cgroup/memory/limit50M/memory.oom_control"

#define BUFLEN 256

static void assert(int cond, const char* msg) {
    if (!cond) {
        printf("%s\n", msg);
        exit(-1);
    }
}

int main() {
    //event fd, cgroup.event_control fd and memory.oom_control fd
    int efd, cfd, ofd; 
    int count;
    char buf[BUFLEN] = {0};

    efd = eventfd(0, 0);
    assert((efd != -1), "open eventfd failed");

    cfd = open(CGROUP_CONTROL, O_WRONLY);
    assert((cfd > 0), "open cgroup.event_control failed");

    ofd = open(CGROUP_OOM, O_RDONLY);
    assert((ofd > 0), "open memory.oom_control failed");

    count = snprintf(buf, BUFLEN, "%d %d", efd, ofd);
    
    if (write(cfd, buf, count) < 0) {
        printf("write event_control failed");
        close(efd);
        close(cfd);
        close(ofd);

        return -1;
    }

    uint64_t u;
    while (1) {
        if (read(efd, &u, sizeof(u)) != sizeof(uint64_t)) {
            printf("read error\n");
            exit(-1);
        }

        printf("memory.oom_control receive oom :%llu\n", u);
    }

    return 0;
}
