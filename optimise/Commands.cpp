#define LOG_TAG "Commands"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utils/Log.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "Commands.h"

#define BUFLEN 128
#ifndef DEBUG
#define DEBUG 1
#endif
#define _1M  1024 * 1024
#define CLEAR_REFS_ALL "1"
#define CLEAR_REFS_ANON "2"
#define CLEAR_REFS_MAPPED "3"

//governor
#define PATH_CPU_GOVERNOR "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define PATH_CPU_GO_MINSPEED_LOAD "/sys/devices/system/cpu/cpufreq/interactive/go_minspeed_load"
//cgroup
#ifdef AOSP_BULLHEAD
#define PATH_LIMIT_MEMORY "/dev/memcg/apps/memory.limit_in_bytes"
#define BLKIO_LIMIT_WEIGHT "/dev/blkio/blkio.weight"
#define BLKIO_LIMIT "/dev/blkio/limit/tasks"
#define BLKIO_NORMAL "/dev/blkio/tasks"
#else
#define PATH_LIMIT_MEMORY "/sys/fs/cgroup/memory/limit/memory.limit_in_bytes"
#define BLKIO_LIMIT_WEIGHT "/sys/fs/cgroup/blkio/limit/blkio.weight"
#define BLKIO_LIMIT "/sys/fs/cgroup/blkio/limit/tasks"
#define BLKIO_NORMAL "/sys/fs/cgroup/blkio/tasks"

#endif

#define GO_MINSPEED_LOAD "minspeedload"
#define GOVERNOR "governor"

#define LIMITMEMORY "limit"

#define CGROUP_BLKIO "blkio"
#define CGROUP_REDUCE "reduce"

#define FORCERECLAIM "reclaim"

#define SETPRIO "setprio"

using namespace std;

static int isSameValue(FILE* fp, const char* setValue) {
    char buf[BUFLEN] = {0};

    int ret = fread(buf, 1,  sizeof(buf), fp);
    if (ret < 0) {
        ALOGE("%s\n", strerror(errno));
        return 0;
    }

    if (!strncmp(setValue, buf, strlen(setValue))) {
         ALOGD("same governor, no need to change\n");
         return 1;
    }

    return 0;
}

static int setValue(const char* path, char* value) {
    int ret = 0;
    FILE* fp = NULL;
#ifdef AOSP_BULLHEAD
    fp = fopen(path, "w");
#else
   fp = fopen(path, "r+");
#endif
    if (fp == NULL) {
	   ALOGE("%s\n", strerror(errno));
       return -1;
    }

#ifndef AOSP_BULLHEAD
    if (isSameValue(fp, value)) {
        ALOGE("same value\n");
        goto out;
    }
#endif
    ret = fwrite(value, 1, strlen(value), fp);
    if (ret < 0) {
        ALOGE("%s\n", strerror(errno));
		 goto out;
    }

out:
    if (fp)
        fclose(fp);
    return ret;
}

static long getFreeMemory() {
    static const char* const sums[] = { "MemFree:", "Cached:", NULL };
    static const size_t sumsLen[] = { strlen("MemFree:"), strlen("Cached:"), 0 };
    size_t num = 2;

    int fd = open("/proc/meminfo", O_RDONLY);

    if (fd < 0) {
        ALOGW("Unable to open /proc/meminfo");
        return -1;
    }

    char buffer[256];
    const int len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);

    if (len < 0) {
        ALOGW("Unable to read /proc/meminfo");
        return -1;
    }
    buffer[len] = 0;

    size_t numFound = 0;
    long mem = 0;

    char* p = buffer;
    while (*p && numFound < num) {
        int i = 0;
        while (sums[i]) {
            if (strncmp(p, sums[i], sumsLen[i]) == 0) {
                p += sumsLen[i];
                while (*p == ' ') p++;
                char* num = p;
                while (*p >= '0' && *p <= '9') p++;
                if (*p != 0) {
                    *p = 0;
                    p++;
                    if (*p == 0) p--;
                }
                mem += atoll(num) * 1024;
                numFound++;
                break;
            }
            i++;
        }
        p++;
    }

    return mem;
}

//CPufreqCmd used for tune cpufreq
CpufreqCmd::CpufreqCmd(const char* cmd) 
   :ICommand(cmd) {
    
}

CpufreqCmd::~CpufreqCmd() {
    
}

void CpufreqCmd::onCommand(int argc, char *args[]) {
    int ret = 0;
    if (!strcmp(args[1], GOVERNOR)) {
        changeGovernor(argc, args);
		return;
    } else if (!strcmp(args[1], GO_MINSPEED_LOAD)) {
        tuneMinspeedLoad(argc, args);
        return;
    }
}

/**
* governor cmd: "cpufreq governor cpu_governor"
*/
int CpufreqCmd::changeGovernor(int argc, char **args) {
    int ret = 0;
    char buf[12] = {0};
    const char *path = PATH_CPU_GOVERNOR;
    char *value = args[2];

    if (argc != 3) {
         cout << "change governor need only 3 arguments" << endl;
         return -1;
    }

    return setValue(path, value);
}

/**
*minspeed load only for nexus 7: cpufreq minspeedload load
*/
int CpufreqCmd::tuneMinspeedLoad(int argc, char **args) {
    int ret = 0;
    if (argc != 3) {
         cout << "tune minspeed load need only 3 arguments" << endl;
         return -1;
    }

    const char *minload = PATH_CPU_GO_MINSPEED_LOAD;
    char *load = args[2];

    ret = setValue(minload, load);

    return ret;
}

CgroupCmd::CgroupCmd(const char* cmd)
	:ICommand(cmd) {
#ifndef AOSP_BULLHEAD
    //try to change mode
	if (access("/sys/fs/cgroup/blkio/limit/tasks", F_OK)) {
		mkdir("/sys/fs/cgroup/blkio/limit", 0750);

		FILE* fp = fopen(BLKIO_LIMIT_WEIGHT, "r+");
		if (fp != NULL) {
			char limit_weight[] = {"200"};
            int ret = fwrite(limit_weight, sizeof(char), strlen(limit_weight), fp);
			fclose(fp);
		}
	}
#endif
}

CgroupCmd::~CgroupCmd() {

}

void CgroupCmd::onCommand(int argc, char *args[]) {
    if (!strcmp(args[1], LIMITMEMORY)) {
        tuneLimitMemory(argc, args);
	} else if (!strcmp(args[1], CGROUP_BLKIO)) {
        tuneBlkio(argc, args, 0);
	} else if (!strcmp(args[1], CGROUP_REDUCE)) {
        tuneBlkio(argc, args, 1);
	}
}

/**
*tune limit memory cmd: cgroup limit limit_in_bytes
*/
int CgroupCmd::tuneLimitMemory(int argc, char **args) {
    int ret = 0;
    const char* path = PATH_LIMIT_MEMORY;
    char* value = args[2];

    if (argc != 3) {
         cout << "tune minspeed load need only 3 arguments" << endl;
         return -1;
    }

    return setValue(path, value);
}

int CgroupCmd::tuneBlkio(int argc, char **args, int reduce) {
    int ret = 0;
    FILE* fp = NULL;
	char* pidstr = args[2];

    if (argc != 3) {
         cout << "tune minspeed load need only 3 arguments" << endl;
         return -1;
    }

    if (reduce) {
        fp = fopen(BLKIO_LIMIT, "r+");
    } else {
	    fp = fopen(BLKIO_NORMAL, "r+");
    }

    if (fp == NULL) {
        ALOGE("%s\n", strerror(errno));
		return -1;
	}

    ret = fwrite(pidstr, 1, strlen(pidstr), fp);
    if (ret < 0) {
        ALOGE("%s\n", strerror(errno));
    }

	fclose(fp);
	return ret;
}

ReclaimCmd::ReclaimCmd(const char* cmd) 
	:ICommand(cmd) {

}

ReclaimCmd::~ReclaimCmd() {
}

void ReclaimCmd::onCommand(int argc, char *args[]) {
	//only two arguments, and args[0] is reclaim,then just reclaim it
    if (argc == 2 && !strcmp(args[0], FORCERECLAIM))
	    forceReclaim(argc, args);
}

void ReclaimCmd::forceReclaim(int argc, char **args) {
#if 1
	char reclaim[BUFLEN] = {"1"};
    char filename[BUFLEN] = {0};

	if (argc != 2)
		return;
#ifdef AOSP_BULLHEAD
    long freeMem = getFreeMemory();
    if (400 * _1M < freeMem && freeMem < 500 * _1M)
       strncpy(reclaim, CLEAR_REFS_ANON, strlen(CLEAR_REFS_ANON) + 1);
    else if (300 * _1M < freeMem && freeMem <= 400 * _1M)
       strncpy(reclaim, CLEAR_REFS_MAPPED, strlen(CLEAR_REFS_MAPPED) + 1);
    else if (freeMem <= 300*_1M)
       strncpy(reclaim, CLEAR_REFS_ALL, strlen(CLEAR_REFS_ALL) + 1);

    //snprintf(filename, sizeof(filename), "/proc/%s/clear_refs", args[1]);
#endif
    snprintf(filename, sizeof(filename), "/proc/%s/reclaim", args[1]);
    ALOGE("filename:%s\n", filename);
    int ret = setValue(filename, reclaim);

    ALOGD("force reclaim return:%d\n", ret);
#endif
}

SchedCmd::SchedCmd(const char* cmd)
    :ICommand(cmd) {
}
SchedCmd::~SchedCmd() {
}

void SchedCmd::onCommand(int argc, char *args[]) {
    if (!strcmp(args[1], SETPRIO)) {
        setPrio(argc, args);
    }
}

void SchedCmd::setPrio(int argc, char **args) {
#if 0
   if (argc != 4) {
       ALOGE("setPrio need only 4 arguments\n");
       return;
   }

   int ret = 0;
   int pid = atoi(args[2]);
   int prio = atoi(args[3]);

   ret = setpriority(PRIO_PROCESS, pid, prio);
   if (ret) {
       ALOGE("setpriority:%s\n", strerror(errno));
   }
#endif
}

