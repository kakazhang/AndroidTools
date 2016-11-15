#ifndef DIRECTVOLUME_H
#define DIRECTVOLUME_H
#include "Volume.h"

class VolumeManager;

class DirectVolume : public Volume {
public:
	DirectVolume(VolumeManager *vm, const fstab_rec* rec, int flags);
	virtual ~DirectVolume();

	const char *getMountpoint();
	const char *getFuseMountpoint();

private:
	char* mMountPoint;
	char* mFuseMountPoint;
	int mFlags;
	VolumeManager* mVM;
};
#endif
