#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H
#include <pthread.h>

#include "Volume.h"

class VolumeManager {
public:
	static VolumeManager* Instance();
	virtual ~VolumeManager();

	void addVolume(Volume* v);
	void listVolume();
	Volume* lookupVolume(const char* label);
	bool mountVolume(const char* label);
	void create_partition(char* fsPath, char* imageName);

private:
	VolumeManager();
	char* asecHash(const char* id, char* buffer, int len);

private:
	static VolumeManager* gInstance;
	VolumeCollection *mVolumes;
	static pthread_mutex_t gMutex;
};
#endif
