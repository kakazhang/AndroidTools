#include <binder/Parcel.h>
#include <utils/Errors.h>
#include <iostream>
#include "IFileService.h"

using namespace std;
using namespace android;

enum {
    GETFILE = IBinder::FIRST_CALL_TRANSACTION,
};

class BpFileService: public BpInterface<IFileService>
{
public:
    BpFileService(const sp<IBinder>& impl)
        : BpInterface<IFileService>(impl)
    {
    }

	virtual status_t getFilesForCmdline(const char* cmdline, String16** outFiles, int* fileLen);
};

status_t BpFileService::getFilesForCmdline(const char* cmdline, String16** outFiles, int* fileLen) {
	Parcel data, reply;
	data.writeInterfaceToken(IFileService::getInterfaceDescriptor());
	data.writeCString(cmdline);

	status_t status = remote()->transact(GETFILE, data, &reply);
	if (status == NO_ERROR) {
		status = (status_t)reply.readInt32();
		if (status == NO_ERROR) {
			int fLen = reply.readInt32();
			*fileLen = fLen;
			if (fLen > 0) {
				String16* ofs = new String16[fLen];
				for (int i = 0; i < fLen; i++) {
					ofs[i] = reply.readString16();
				}

				*outFiles = ofs;
			}
		}
	}
	
	return status;
}


IMPLEMENT_META_INTERFACE(FileService, "android.file.IFileService");
status_t BnFileService::onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags) {
    switch (code) {
        case GETFILE: {
            CHECK_INTERFACE(IFileService, data, reply);
			
			const char* cmdline = data.readCString();
			status_t status = NO_ERROR;

			String16* fos = NULL;
			int fLen = 0;
			
			//get files from real service
			status = getFilesForCmdline(cmdline, &fos, &fLen);
			cout << "fLen:" << fLen << endl;
			//1) write back status
			reply->writeInt32(status);
			
			//2) write file number
			reply->writeInt32(fLen);
			//3) write files
			if (fos == NULL)
				cout << "fos is NULL" << endl;
			for (int i = 0; i < fLen; i++) {
				String8 str(fos[i]);
				cout << "IFileService get:" << str << endl;
				reply->writeString16(fos[i]);
			}
			
			if (fos != NULL)
				delete[] fos;
			
            return NO_ERROR;
        } break;

		default:
			return BBinder::onTransact(code, data, reply, flags);
    }
	
}

