#include <iostream>
#include <utils/Errors.h>
#include <string>

#include "FileService.h"

#define TESTLEN 2

using namespace std;
using namespace android;

FileService::FileService() 
  : BnFileService() {
	mProcFileDetector = new GetProcessFiles();
}

FileService::~FileService() {
	
}

void FileService::startDetect() {
	mProcFileDetector->run();
}

status_t FileService::getFilesForCmdline(const char* cmdline, String16** outFiles, int* fileLen) {
	string *files = NULL;
	String16 *ofs = NULL;
	int count = mProcFileDetector->getFilesForName(cmdline, &files);
	cout << "count:" << count << endl;
	if (count > 0 && files) {
		ofs = new String16[count];
		for (int i = 0; i < count; i++) {
			cout << "FileService get files:" << files[i].c_str() << endl;
			ofs[i] = String16(files[i].c_str());
		}
		delete[] files;
	}

	*fileLen = count;
	*outFiles = ofs;
	
	return NO_ERROR;
}

