#ifndef FILTER_H
#define FILTER_H
#include <set>
#include <string>
#include <utils/RefBase.h>
#include <utils/Mutex.h>

using namespace std;
using namespace android;

#define BOOTCLASSPATH "BOOTCLASSPATH"
#define SYS "/sys/"
#define DEV "/dev/"
#define PIPE "pipe"
#define SOCKET "socket"
#define ANON_INODE "anon_inode"
#define FRAMEWORK_RES "/system/framework/framework-res.apk"
class Filter : public RefBase {
public:
	static sp<Filter> getInstance();
	virtual ~Filter();
	bool checkExist(string file);
	void dump();
private:
	Filter();
	bool readBootClassFiles();
	void addPrefixFiles();
	
private:
	set<string> mBootClasses;
	set<string> mPrefix;
	static Mutex gInitLock;
	static sp<Filter> gInstance;
};

#endif
