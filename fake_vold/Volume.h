#ifndef VOLUME_H
#define VOLUME_H
#include <utils/List.h>
#include <fs_mgr.h>
#define PATH_MAX 4096

class VolumeManager;

class Volume {
public:
	Volume(VolumeManager *vm, const fstab_rec* rec, int flags);
	virtual ~Volume();
    //int mountVol();
    //int unmountVol(bool force, bool revert);
    //int formatVol();

    const char* getLabel() { return mLabel; }
    
    virtual const char *getMountpoint() = 0;
    virtual const char *getFuseMountpoint() = 0;
    virtual void handleVolumeShared();
    virtual void handleVolumeUnshared();
private:
	bool isMountpointMounted(const char *path);
private:
	char* mLabel;
};

typedef android::List<Volume *> VolumeCollection;

#endif
