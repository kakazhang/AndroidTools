#include <iostream>
#include <utils/RefBase.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include "IFileService.h"
#include "FileService.h"
#include "Filter.h"
#include "GetProcessFiles.h"
using namespace std;
using namespace android;

void getInChild(sp<GetProcessFiles> procFiles, const char* cmdline) {	
	string* files = NULL;
	int N = procFiles->getFilesForName(cmdline, &files);//getFilesForPid(pid, &files);
	if (N > 0) {
		for (int i = 0; i < N; i++)
			cout << "file:" << files[i] << endl;
	} else
		cout << "no files" << endl;

	delete[] files;
}

int main(int argc, char**argv) {
	sp<FileService> fileService = new FileService();
	
    sp<IServiceManager> sm(defaultServiceManager());
    sm->addService(String16(FileService::getServiceName()), fileService, false);
	
	fileService->startDetect();

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();	
}
