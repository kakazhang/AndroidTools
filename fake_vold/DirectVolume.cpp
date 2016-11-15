#include <stdio.h>
#include <string.h>

#include "DirectVolume.h"
#define MEDIA_DIR  "/mnt/media_rw"
#define FUSE_DIR   "/storage"

DirectVolume::DirectVolume(VolumeManager *vm, const fstab_rec* rec, int flags) 
	: Volume(vm, rec, flags) {

	char mount[PATH_MAX] = {0};
	if (strcmp(rec->mount_point, "auto") != 0) {
		printf("mount point:%s must be auto,ignore invalid path\n", rec->mount_point);
	}
	
	snprintf(mount, PATH_MAX, "%s/%s",MEDIA_DIR, rec->label);
	mMountPoint = strdup(mount);

	snprintf(mount, PATH_MAX, "%s/%s",FUSE_DIR, rec->label);
	mFuseMountPoint = strdup(mount);
	
	mFlags = flags;
}

DirectVolume::~DirectVolume() {
	free(mMountPoint);
	free(mFuseMountPoint);
}

const char * DirectVolume::getMountpoint() {
	return mMountPoint;
}

const char * DirectVolume::getFuseMountpoint() {
	return mFuseMountPoint;
}

