#include <stdio.h>
#include <string.h>

#include "Volume.h"

Volume::Volume(VolumeManager *vm, const fstab_rec* rec, int flags) {
	mLabel = strdup(rec->label);
}

Volume::~Volume() {
	free(mLabel);
}

void Volume::handleVolumeShared() {
	
}

void Volume::handleVolumeUnshared() {
	
}

bool Volume::isMountpointMounted(const char *path) {
	char device[256] = {0};
	char mount_path[256] = {0};
	char rest[256] = {0};

	char line[PATH_MAX] = {0};

	FILE* fp = fopen("/proc/mounts", "r");
	if (NULL == fp) {
		perror("fopen");
		return false;
	}

	while (fgets(line, sizeof(line), fp)) {
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
		
		if (!strcmp(mount_path, path)) {
			fclose(fp);
			fp = NULL;

			return true;
		}
	}

	fclose(fp);

	return false;
	
}

