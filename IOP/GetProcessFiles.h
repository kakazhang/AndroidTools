#ifndef GETPROCESSFILES_H
#define GETPROCESSFILES_H
#include <pthread.h>
#include <utils/Thread.h>
#include <sys/types.h>
#include <dirent.h>

#include <map>
#include <set>

#include "Filter.h"

using namespace std;
using namespace android;
typedef map<string, set<string> > Map;
typedef set<string> Set;
typedef map<string, set<string> >::iterator MapIter;

class GetProcessFiles : public Thread {
public:
	GetProcessFiles();
	virtual ~GetProcessFiles();
	void mapFilesForNiceName(const char* niceName);
	int getFilesForName(const char* cmdline, string** fs);

private:
	bool isLegalCmdline(const char* cmdline);
	bool isLegalDir(const struct dirent* procDir, bool proc);
private:
	virtual bool threadLoop();
	
	bool scanDir();
	void readFdLinks(int pid, const char* cmdline);
	bool addToSet(set<string>& fileSet, const char* fileName);
	
	bool readCmdline(int pid, char cmdline[]);
	bool readFilesForPid(int pid);
	int getFilesForPid(int pid, string** fs);
	
private:
	bool first;
	Map mProcFiles;
	pthread_rwlock_t mProcessesLock;
};

#endif
