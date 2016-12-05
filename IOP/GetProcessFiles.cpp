#define LOG_TAG "GetProcessFiles"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <sys/mman.h>
#include <errno.h>

#include "GetProcessFiles.h"

#define CMDLINE "/proc/%d/cmdline"
#define PROCFD "/proc/%d/fd/"

#define SLEEP_TIMEOUT  30*60

#define PATHLEN 64
#define CMDLEN 64
#define LEN 256
#define MAX_MEM 80*1024*1024

using namespace std;
using namespace android;

static void showErrMsg(int err) {
	switch(err) {
	case EACCES:
		cout << "permission denied" << endl;
		break;
	case EMFILE:
		cout << "Too many file descriptors in use by process." << endl;
		break;
	case ENFILE:
		cout << "Too many files are currently open in the system." << endl;
		break;
	case ENOENT:
		cout << "Directory does not exist, or name is an empty string." << endl;
		break;		
	case ENOMEM:
		cout << "Insufficient memory to complete the operation." << endl;
		break;
	default:
		break;
	}
}

GetProcessFiles::GetProcessFiles() {
	pthread_rwlock_init(&mProcessesLock, NULL);
}

GetProcessFiles::~GetProcessFiles() {
	pthread_rwlock_destroy(&mProcessesLock);
}

int GetProcessFiles::getFilesForName(const char* cmdline, string** fs) {
	pthread_rwlock_rdlock(&mProcessesLock);
	auto mi = mProcFiles.find(cmdline);
	auto end = mProcFiles.end();
	pthread_rwlock_unlock(&mProcessesLock);

	string* files = NULL;
	int i = 0;
	if (mi != end) {
		Set procfiles = mi->second;
		int size = procfiles.size();
		files = new string[size];	

		for (auto fi=procfiles.begin(); fi != procfiles.end(); fi++)
			files[i++] = *fi;
	}

	*fs = files;
	return i;

}

int GetProcessFiles::getFilesForPid(int pid, string** fs) {
	char cmdline[CMDLEN] = {0};
	if (!readCmdline(pid,cmdline))
		return 0;

	return getFilesForName(cmdline, fs);
}

void GetProcessFiles::mapFilesForNiceName(const char* niceName) {
	size_t total_size = 0;
	string* files = NULL;
	//get files with name @niceName
	int count = getFilesForName(niceName, &files);
	ALOGE("get file count(%d) for %s", count, niceName);

	if (count > 0 && files) {
		for (int i = 0; i < count; i++) {
			const char* fileName = files[i].c_str();
			struct stat st;
			stat(fileName, &st);
			size_t sizeInBytes = st.st_size;

			if (total_size > MAX_MEM)
				break;
			int fd = open(fileName, O_RDONLY);
			if (fd > 0) {
				void* addr = mmap(NULL, sizeInBytes, PROT_READ, MAP_PRIVATE, fd , 0);
				if (MAP_FAILED != addr) {
					total_size += sizeInBytes;
					ALOGE("map for %s success", fileName);
				}
				close(fd);
			} else {
				ALOGE("open  %s failed(errno:%d)", fileName, errno);
			}
		}
	}

	if (files)
		delete[] files;
}


bool GetProcessFiles::threadLoop() {
	sleep(SLEEP_TIMEOUT);
	return scanDir();
}

bool GetProcessFiles::scanDir() {
	DIR *dir = opendir("/proc");
	if (NULL == dir) {
		showErrMsg(errno);
		return false;
	}

	int total = 0;
	struct dirent* di;
	while ((di=readdir(dir)) != NULL) {
		if (!isLegalDir(di, true))
			continue;
			
		int pid = atoi(di->d_name);
		if (readFilesForPid(pid))
			total++;
	}

	closedir(dir);

	return total > 0;
}

bool GetProcessFiles::isLegalDir(const struct dirent* procDir, bool proc) {
	if (proc && !isdigit(procDir->d_name[0]))
		return false;
	
	if (procDir->d_name[0] == '.' && (procDir->d_name[1] == '\0' || procDir->d_name[1] == '.'))
		return false;

	return true;
}

/**
* Just need to check if this cmdline is an application name
*/
bool GetProcessFiles::isLegalCmdline(const char* cmdline) {
	return (cmdline[0] == '_'  || isalpha(cmdline[0]) || isdigit(cmdline[0]));
}

bool GetProcessFiles::readCmdline(int pid, char cmdline[]) {
	char path[PATHLEN] = {0};
	snprintf(path, sizeof(path), CMDLINE, pid);
	if (access(path, F_OK)) {
		perror("access");
		return false;
	}
	char line[LEN] = {0};
	FILE* fp = fopen(path, "r");
	if (!fp) {
		perror("fopen");
		return false;
	}

	if (fgets(line, sizeof(line), fp) != NULL) {
		strncpy(cmdline, line, strlen(line)+1);
	}

	fclose(fp);
	
	return true;
}

void GetProcessFiles::readFdLinks(int pid, const char* cmdline) {
	bool changed = false;
	bool contains = false;
	
	Set fileSet;
	MapIter it;

	char path[PATHLEN] = {0};
	snprintf(path, sizeof(path), PROCFD, pid);
	DIR* dir = opendir(path);
	if (dir == NULL) {
		showErrMsg(errno);
		return;
	}

	pthread_rwlock_rdlock(&mProcessesLock);
	if ((it=mProcFiles.find(cmdline)) != mProcFiles.end()) {
		contains = true;
		fileSet = it->second;
	}
	pthread_rwlock_unlock(&mProcessesLock);

	struct dirent *di = NULL;
	int plen = strlen(path);
	while ( (di=readdir(dir)) ) {
		if (!isLegalDir(di, false))
			continue;
		
		char link[LEN] = {0};
		strncpy(path+plen, di->d_name, strlen(di->d_name));
		if (readlink(path, link, sizeof(link)) > 0) {
			if (Filter::getInstance()->checkExist(link))
				continue;
			
			if (addToSet(fileSet, link) && !changed)
				changed = true;
		}
	}

	pthread_rwlock_wrlock(&mProcessesLock);
	if (contains && changed) {
		mProcFiles.erase(it);
	}
	mProcFiles.insert(std::pair<string, Set >(cmdline, fileSet));
	pthread_rwlock_unlock(&mProcessesLock);
	
	closedir(dir);
}

bool GetProcessFiles::readFilesForPid(int pid) {
	char cmdline[CMDLEN] = {0};

	if (!readCmdline(pid, cmdline) || !isLegalCmdline(cmdline)) {
		return false;
	}

	readFdLinks(pid, cmdline);

	return true;
}

bool GetProcessFiles::addToSet(Set& fileSet, const char* fileName) {
	struct stat st;
	int ret = stat(fileName, &st);
	if (ret || (st.st_size <= 0) 
		|| (fileSet.find(fileName)) != fileSet.end()) {
		return false;
	}

	fileSet.insert(fileName);
	return true;
}
