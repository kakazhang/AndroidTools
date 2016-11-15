#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <openssl/md5.h>
#include <dirent.h>

#include "VolumeManager.h"
#include "Loop.h"

pthread_mutex_t VolumeManager::gMutex = PTHREAD_MUTEX_INITIALIZER;
VolumeManager* VolumeManager::gInstance = NULL;

VolumeManager* VolumeManager::Instance() {
	pthread_mutex_lock(&gMutex);
	if (NULL == gInstance) {
		gInstance = new VolumeManager();
	}
	pthread_mutex_unlock(&gMutex);

	return gInstance;
}

VolumeManager::~VolumeManager() {
	
}

void VolumeManager::addVolume(Volume* v) {
	mVolumes->push_back(v);
}

void VolumeManager::listVolume() {
	VolumeCollection::iterator i;
	char* buffer;
	
    for (i = mVolumes->begin(); i != mVolumes->end(); ++i) {
		asprintf(&buffer, "%s %s", (*i)->getLabel(), (*i)->getMountpoint());
		printf("%s\n",buffer);
	}

	free(buffer);
}

Volume* VolumeManager::lookupVolume(const char* label) {
	VolumeCollection::iterator i;

	for (i = mVolumes->begin(); i != mVolumes->end(); i++) {
		if (label[0] == '/') {
			if (!strcmp(label, (*i)->getFuseMountpoint()))
				return (*i);
		} else {
			if (!strcmp(label, (*i)->getLabel()))
                return (*i);
		}
	}

	return NULL;
}

bool VolumeManager::mountVolume(const char* label) {
	unsigned long flags = 0;
	char mountdata[256] = {0};
	int rc = 0;
	
	Volume* volume = lookupVolume(label);
	if (!volume) {
		char system[] = {"/dev/block/platform/msm_sdcc.1/by-name/system"};
		char mount_point[] = {"/data/test"};

		if (mkdir(mount_point, 0700)) {
			if (errno != EEXIST) {
				perror("mkdir");
				return false;
			}
		}
		
		flags |= MS_RDONLY;
		rc = mount(system, mount_point, "ext4", flags, NULL);
		if (rc) {
			perror("mount");
			return false;
		}
	} else {

	}

	return true;
}

void VolumeManager::create_partition(char* fsPath, char* imageName) {
	unsigned  selectorNum = 256;
	unsigned  sectors; 
	char loopDeviceBuffer[256] = {0};
	
	size_t len = sizeof(loopDeviceBuffer);

	if (opendir(fsPath) == NULL) {
		mkdir(fsPath, 0666);
	}

	sectors = Loop::adjustSectorNumExt4(selectorNum);
	Loop::createImage(fsPath, imageName, sectors);
	
	if (Loop::create(imageName, imageName, loopDeviceBuffer, len)) {
		printf("losetup failed\n");
		return;
	} else {
		printf("%s created successfully\n", loopDeviceBuffer);
	}
}

VolumeManager::VolumeManager() {
	mVolumes = new VolumeCollection();
}

char* VolumeManager::asecHash(const char* id, char* buffer, int len) {
	static const char *digits = "0123456789abcdef";
	unsigned char hash[MD5_DIGEST_LENGTH] = {0};

	MD5((const unsigned char*)id, strlen(id), hash);
	char *p = buffer;

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        *p++ = digits[hash[i] >> 4];
        *p++ = digits[hash[i] & 0x0F];
	}
	
	*p = '\0';

	return buffer;
}

