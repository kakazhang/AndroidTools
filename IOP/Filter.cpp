#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "Filter.h"

using namespace std;

sp<Filter> Filter::gInstance = NULL;
Mutex Filter::gInitLock("init lock");

sp<Filter> Filter::getInstance() {
	if (gInstance == NULL) {
		Mutex::Autolock _l(gInitLock);
		gInstance = new Filter();
	}

	return gInstance;
}

Filter::Filter() {
	mBootClasses.insert(FRAMEWORK_RES);
	addPrefixFiles();
	readBootClassFiles();
}

Filter::~Filter() {
	
}

bool Filter::checkExist(string fileName) {
	set<string>::iterator it;
	const char* file = fileName.c_str();

	for (it = mPrefix.begin(); it != mPrefix.end(); it++) {
		const char* prefix = (*it).c_str();
		if (!strncmp(prefix, file, strlen(prefix)))
			return true;
	}
	
	it = mBootClasses.find(fileName);
	if (it != mBootClasses.end())
		return true;

	return false;
}

bool Filter::readBootClassFiles() {
	char *begin = NULL, *end = NULL;
	
	char* bootclass = getenv(BOOTCLASSPATH);
	if (bootclass != NULL) {
		begin = bootclass;
		while ( (end = strchr(begin, ':')) != NULL) {
			*end++ = '\0';
			
			mBootClasses.insert(begin);
			begin = end;
		}

		mBootClasses.insert(begin);
		
		return true;
	}

	return false;
}

void Filter::addPrefixFiles() {
	mPrefix.insert(SYS);
	mPrefix.insert(DEV);
	mPrefix.insert(PIPE);
	mPrefix.insert(SOCKET);
	mPrefix.insert(ANON_INODE);
}

void Filter::dump() {
	set<string>::iterator it;
	for (it = mBootClasses.begin(); it != mBootClasses.end(); it++) {
		cout << "file:" << (*it).c_str() << endl;
	}
}

