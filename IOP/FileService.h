#ifndef FILESERVICE_H
#define FILESERVICE_H
#include <utils/Errors.h>

#include "GetProcessFiles.h"
#include "IFileService.h"

using namespace android;

class FileService : public BnFileService {
public:
	FileService();
	virtual ~FileService();
    static char const* getServiceName() {
        return "FileService";
    }

	virtual status_t getFilesForCmdline(const char* cmdline, String16** outFiles, int* fileLen);

	void startDetect();
private:
	sp<GetProcessFiles> mProcFileDetector;
};

#endif
