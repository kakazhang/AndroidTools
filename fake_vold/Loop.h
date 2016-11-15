#ifndef LOOP_H
#define LOOP_H

class Loop {
public:
	static const int LOOP_MAX = 4096;
public:
	static int adjustSectorNumExt4(unsigned numSectors);
	static int destroyByDevice(const char *loopDevice);
	static int create(char *id, const char *loopfile, char *loopDeviceBuffer, size_t len);
	static int createImage( char* fsPath, char* imageName, unsigned numSectors);
};

#endif
