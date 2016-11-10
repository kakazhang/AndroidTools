#ifndef IFILESERVICE_H
#define IFILESERVICE_H
#include <binder/IInterface.h>
#include <utils/String16.h>
#include <utils/String8.h>
using namespace android;

class IFileService : public IInterface {
public:
	DECLARE_META_INTERFACE(FileService);
	virtual status_t getFilesForCmdline(const char* cmdline, String16** outFiles, int* fileLen) = 0;
	
};

class BnFileService: public BnInterface<IFileService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

#endif
