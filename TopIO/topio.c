#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define LEN 256
#define CMDLEN 36
#define INIT_PROCS 120
#define PROC_DIR "/proc"

typedef struct proc_info {
	struct proc_info* next;
    char cmdline[CMDLEN];
	int pid;
	long read_bytes;
	long write_bytes;
	long speed;
} ProcInfo, *proc_info;

static int num_used_procs;
static int total_procs;
static int old_used_procs;
static int old_total_procs;
static proc_info* old_procs;
static proc_info* new_procs;

proc_info free_procs;

static void assert(int cond, const char* msg) {
	if (!cond) {
		printf("%s\n", msg);
		exit(-1);
	}
}

static int num_cmp(long a, long b) {
	if (a < b) return -1;
	if (a > b) return 1;

	return 0;
}

static int speed_cmp(const void* a, const void* b) {
	proc_info pa = *((proc_info *)a);
	proc_info pb = *((proc_info *)b);

	if (!pa && !pb) return 0;
	if (!pa) return 1;
	if (!pb) return -1;

	return -num_cmp(pa->speed, pb->speed);
}

static proc_info find_old_proc(proc_info newproc) {
	if (old_procs == NULL)
		return NULL;

	int i;
	
	for (i = 0; i < old_used_procs; i++) {
		if (!strcmp(newproc->cmdline, old_procs[i]->cmdline)) {
			return old_procs[i];
		}
	}

	return NULL;
}

static void print_procs() {
	int i;
	qsort(new_procs, num_used_procs, sizeof(proc_info), speed_cmp);
	//printf("num_used_procs :%d\n", num_used_procs);
	printf("%5s %31s %31s %31s %s\n", "PID", "READ_BYTES", "WRITE_BYTES", "SPEED", "Name");

	for (i = 0; i < 10; i++) {
		printf("%5d %31ld %31ld %31ld %s\n",
                        new_procs[i]->pid, new_procs[i]->read_bytes, new_procs[i]->write_bytes,
                        new_procs[i]->speed, new_procs[i]->cmdline);
	}
    printf("\n");
}

static proc_info alloc_proc() {
	proc_info proc = NULL;;
	if (free_procs) {
		proc = free_procs;
		free_procs = free_procs->next;
	} else {
		proc = (proc_info)malloc(sizeof(ProcInfo));
		assert((proc != NULL), "No space");
	}

	return proc;
}

static void free_old_procs() {
	int i;
	if (old_procs) {
		for (i = 0; i < old_used_procs; i++) {
			old_procs[i]->next = free_procs;
			free_procs = old_procs[i];
		}
		free(old_procs);
	}
}

static void add_proc(proc_info proc) {
	if (!new_procs) {
		new_procs = (proc_info *)malloc(sizeof(proc_info) * total_procs);
		assert((new_procs != NULL), "No more space");
	}
	
	if (num_used_procs >= total_procs) {
		new_procs = realloc(new_procs, sizeof(proc_info) * total_procs * 2);
		assert((new_procs != NULL), "No space");

		total_procs = total_procs * 2;

		int i;
		for(i = num_used_procs; i < total_procs; i++)
			new_procs[i] = NULL;
	}

	new_procs[num_used_procs++] = proc;
}

static int read_name(proc_info proc) {
	int ret = 0;
	char path[LEN] = {0};
	char name[LEN] = {0};

	snprintf(path, LEN,"/proc/%d/stat", proc->pid);
	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		perror("fopen");
		ret = -1;
		goto out;
	}
	
	if (fread(name, 1, LEN, fp) <= 0) {
		perror("fread");
		ret = -1;
		goto out;
	}

	char* start = strchr(name, '(') + 1;
	char* end = strchr(name, ')') - 1;

	memset(proc->cmdline, 0, CMDLEN);
	strncpy(proc->cmdline, start, (end-start+1));
out:
	if (fp != NULL)
		fclose(fp);

	return ret;
}

static void read_io(proc_info proc) {
	char path[LEN] = {0};
	char line[LEN] = {0};
	
	snprintf(path, LEN,"/proc/%d/io", proc->pid);

	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		perror("fopen");
		return;
	}

	long read_bytes = -1;
	long write_bytes = -1;
	while (fgets(line, LEN, fp)) {
		if (sscanf(line, "read_bytes: %ld", &read_bytes) == 1)
			proc->read_bytes = read_bytes;
		if (sscanf(line, "write_bytes: %ld", &write_bytes) == 1)
			proc->write_bytes = write_bytes;
	}

	proc_info oldproc = find_old_proc(proc);
	if (oldproc) {
		proc->speed = ((proc->read_bytes + proc->write_bytes) - (oldproc->read_bytes + oldproc->write_bytes))/3;
    }else {
		proc->speed = 0;
    }
	fclose(fp);
}

static void read_proc(int pid) {
	proc_info proc = alloc_proc();
	proc->pid = pid;

	read_name(proc);
	read_io(proc);

	add_proc(proc);
}

static void read_procs() {
	DIR* di = opendir(PROC_DIR);
	if (di == NULL) {
		perror("opendir");
		return;
	}
	
	struct dirent* d = NULL;
	while ( (d=readdir(di)) != NULL) {
		if (!isdigit(d->d_name[0]))
			continue;

		int pid = atoi(d->d_name);
		read_proc(pid);
	}

	closedir(di);
}

static void init_procs() {
	old_procs = NULL;
	new_procs = (proc_info*)malloc(sizeof(proc_info) * INIT_PROCS);
	assert((new_procs != NULL), "No space for new procs");
	
	num_used_procs = 0;
	old_used_procs = 0;
	total_procs = INIT_PROCS;
	old_total_procs = 0;

	free_procs = NULL;
}

static void update() {
	old_procs = new_procs;
	new_procs = NULL;
	
	old_used_procs = num_used_procs;
	old_total_procs = total_procs;
	
	num_used_procs = 0;
}

void sanity_check() {
    if (access("/proc/self/io", F_OK)) {
        printf("need to config kernel:\n");
        printf("CONFIG_BOOT_TRACER\n");
        printf("CONFIG_TASKSTATS\n");
        printf("CONFIG_TASK_IO_ACCOUNTING\n");
        printf("CONFIG_TASK_XACCT\n");
        printf("CONFIG_TASK_DELAY_ACCT\n");

        exit(-1);
    }
}

int main() {
	int iterate = 10;
	int delay = 3;

	sanity_check();
    
    init_procs();

	while (1) {
		read_procs();
		print_procs();
		free_old_procs();

		update();
		
		sleep(delay);
	}
	return 0;
}
