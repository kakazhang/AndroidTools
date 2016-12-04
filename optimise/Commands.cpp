#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Commands.h"

#define GO_MINSPEED_LOAD "minspeedload"
#define GOVERNOR "governor"

#define LIMITMEMORY "limit"

using namespace std;

//CPufreqCmd used for tune cpufreq
CpufreqCmd::CpufreqCmd(const char* cmd) 
   :ICommand(cmd) {
    
}

CpufreqCmd::~CpufreqCmd() {
    
}

void CpufreqCmd::onCommand(int argc, char *args[]) {
    int ret = 0;
    if (!strcmp(args[2], GOVERNOR)) {
        changeGovernor(argc, args);
		return;
    } else if (!strcmp(args[2], GO_MINSPEED_LOAD)) {
        tuneMinspeedLoad(argc, args);
        return;
    }
}

int CpufreqCmd::changeGovernor(int argc, char **args) {
    int ret = 0;
    char buf[12] = {0};
    const char *scaling_governor = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
    FILE* fp = NULL;

    if (argc != 4) {
         cout << "change governor need only 3 arguments" << endl;
         return -1;
    }
    
    fp = fopen(scaling_governor, "r+");
    if (fp == NULL) {
       perror("fopen");
       return -1;
    }
    
    ret = fread(buf, 1,  sizeof(buf), fp);
    if (ret < 0) {
        perror("fread");
        fclose(fp);
        return -1;
    }

    if (!strcmp(args[3], buf)) {
         cout << "same governor, no need to change" << endl;
         fclose(fp);
         return 0; 
    }

    ret = fwrite(args[3], 1, strlen(args[3]), fp);
    if (ret < 0) {
        perror("fwrite");
		fclose(fp);
        return -1;
    }

	fclose(fp);
    return 0;
}

int CpufreqCmd::tuneMinspeedLoad(int argc, char **args) {
    int ret = 0;
    if (argc != 4) {
         cout << "tune minspeed load need only 4 arguments" << endl;
         return -1;
    }

    const char *minload = "/sys/devices/system/cpu/cpufreq/interactive/go_minspeed_load";
    char *load = args[3];

    int fd = open(minload, O_WRONLY);
    if (fd < 0) {
         perror("open");
         return -1;
    }

    ret = write(fd, load, strlen(load));    
    if (ret < 0) {
        perror("write");
    }

    close(fd);

    return ret;
}

CgroupCmd::CgroupCmd(const char* cmd)
	:ICommand(cmd) {

}

CgroupCmd::~CgroupCmd() {

}

void CgroupCmd::onCommand(int argc, char *args[]) {
    if (!strcmp(args[2], LIMITMEMORY)) {
        tuneLimitMemory(argc, args);
	}
}

int CgroupCmd::tuneLimitMemory(int argc, char **args) {
    int ret = 0;
    if (argc != 4) {
         cout << "tune minspeed load need only 4 arguments" << endl;
         return -1;
    }

    const char* limit = "/sys/fs/cgroup/memory/limit/memory.limit_in_bytes";
	FILE* fp = fopen(limit, "r+");
	
    ret = fwrite(args[3], 1, strlen(args[3]), fp);
    if (ret < 0) {
        perror("fwrite");
		fclose(fp);
        return -1;
    }

	fclose(fp);
	return 0;
}
