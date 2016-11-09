#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOGPATH "/mnt/sdcard/power.txt"

#define POWER_WL_PATH "/sys/power/wake_lock"

#define WAKE_LOCKS "Wake Locks"
#define SUSPENDBLOCKS "Suspend Blockers"
#define DISPLAYPOWER "Display Power"

#define OFF "OFF"

#define LEN 256
#define BLOCKNAMELEN 64
#define BLOCKSIZE 4

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct sblocks {
    char name[BLOCKNAMELEN];
	int refcount;
} sblocks;

typedef struct powerinfo {
	int state;
	int wakelocks;
	sblocks blocks[BLOCKSIZE];
	int realcount;
} powerinfo, *pinfo;

static void assert(int cond, const char* msg) {
	if (!cond) {
		printf("%s\n", msg);
		exit(-1);
	}
}

static void show_kernel_wakelocks() {
    char line[LEN] = {0};
    FILE *fp = fopen(POWER_WL_PATH, "r");
    if (!fp) {
        perror("fopen");
        return;
    }

    printf("/sys/power/wake_lock:\n");
    while (fgets(line, LEN, fp)) 
        printf("%s\n", line);

    fclose(fp);
}

static void diagnose(pinfo info) {
	int i;
	if (info->state == 0) {
        show_kernel_wakelocks();

		printf("current display is blanked\n");
		
		printf("%s:%d\n", WAKE_LOCKS, info->wakelocks);
		
		int size = MIN(info->realcount, BLOCKSIZE);
		for (i = 0; i < size; i++) {
			printf("%s ref count:%d\n", info->blocks[i].name, info->blocks[i].refcount);
		}
	}
}

static void dump_powerinfo() {
    system("dumpsys power >> /mnt/sdcard/power.txt &");
}

static int parse_sb(FILE* fp, pinfo info) {
	int i;
	char line[LEN] = {0};
	
	for (i = 0; i < BLOCKSIZE; i++) {
		if (fgets(line, LEN, fp) == NULL) {
			printf("parse error\n");
			return i;
		}

		char *delim = strchr(line, ':');
		memset(info->blocks[i].name, 0, BLOCKNAMELEN);
		strncpy(info->blocks[i].name, line, (delim-line+1));

		char *p = strchr(line, '=') + 1;
		info->blocks[i].refcount = atoi(p);
	}

	return i;
}

static void analysis_powerinfo() {
	char line[LEN] = {0};
	pinfo info;
	int sb_count = 0;
	
	FILE* fp = fopen(LOGPATH, "r");
	if (NULL == fp) {
		perror("fopen");
		return;
	}

    info = (pinfo)malloc(sizeof(powerinfo));
	assert((info != NULL), "no space");
	
	while (fgets(line, LEN, fp)) {
		if (strstr(line, WAKE_LOCKS) != NULL) {
			char *p = strchr(line, '=') + 1;
			info->state = atoi(p);
		} else if (strstr(line, SUSPENDBLOCKS)) {
			sb_count = parse_sb(fp,info);
			info->realcount = sb_count;
		} else if (strstr(line, DISPLAYPOWER)) {
			char *st = strchr(line, '=') + 1;
			if (!strncmp(st, OFF, 3))
				info->state = 0;
			else
				info->state = 1;
		}
	}

	diagnose(info);
	
	free(info);
	fclose(fp);

	system("rm -r /mnt/sdcard/power.txt");
}

static void start() {
	dump_powerinfo();
	printf("waiting for analysis result...\n");
	sleep(1);
	
	analysis_powerinfo();
}

int main() {
	start();

	return 0;
}
