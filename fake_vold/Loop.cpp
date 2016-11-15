#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/loop.h>
#include <logwrap/logwrap.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "Loop.h"
#define MKEXT4FS_PATH "/system/bin/make_ext4fs"
#define RESIZE2FS_PATH "/system/bin/resize2fs"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
#define ROUND_UP_POWER_OF_2(number, po2) (((!!(number & ((1U << po2) - 1))) << po2)\
                                         + (number & (~((1U << po2) - 1))))

int Loop::adjustSectorNumExt4(unsigned numSectors) {
    // Ext4 started to reserve 2% or 4096 clusters, whichever is smaller for
    // preventing costly operations or unexpected ENOSPC error.
    // Ext4::format() uses default block size without clustering.
    unsigned clusterSectors = 4096 / 512;
    unsigned reservedSectors = (numSectors * 2)/100 + (numSectors % 50 > 0);
    numSectors += reservedSectors > (4096 * clusterSectors) ? (4096 * clusterSectors) : reservedSectors;
    return ROUND_UP_POWER_OF_2(numSectors, 3);
}

int Loop::destroyByDevice(const char *loopDevice) {
    int device_fd;

    device_fd = open(loopDevice, O_RDONLY);
    if (device_fd < 0) {
        printf("Failed to open loop (%d)", errno);
        return -1;
    }

    if (ioctl(device_fd, LOOP_CLR_FD, 0) < 0) {
        printf("Failed to destroy loop (%d)", errno);
        close(device_fd);
        return -1;
    }

    close(device_fd);
    return 0;
}

int Loop::create(char *id, const char *loopfile, char *loopDeviceBuffer, size_t len) {
	int i;
	char loopdevice[256] = {0};
	int file_fd = -1;
	int devFd = -1;
	int rc = 0;
	
	for (i = 0; i < LOOP_MAX; i++) {
		struct loop_info64 li;
		snprintf(loopdevice, sizeof(loopdevice), "/dev/block/loop%d", i);

        mode_t mode = 0660 | S_IFBLK;
        unsigned int dev = (0xff & i) | ((i << 12) & 0xfff00000) | (7 << 8);
        if (mknod(loopdevice, mode, dev) < 0) {
            if (errno != EEXIST) {
                printf("Error creating loop device node (%s)\n", strerror(errno));
                return -1;
            }
        }

		devFd = open(loopdevice, O_RDWR);
		if (devFd < 0) {
			perror("open");
			return -1;
		}

		rc = ioctl(devFd, LOOP_GET_STATUS64, &li);
		if (rc < 0 && errno == ENXIO)
			break;

		close(devFd);

		if (rc < 0) {
			printf("unable to get status\n");
			return -1;
		}
	}

	if (i == LOOP_MAX) {
		printf("No enough loop device\n");
		return -1;
	}
	
	memcpy(loopDeviceBuffer, loopdevice, len-1);
	
	file_fd = open(loopfile, O_RDWR);
	if (file_fd < 0) {
		perror("open");
		return -1;
	}

	rc = ioctl(devFd, LOOP_SET_FD, file_fd);
	if (rc < 0) {
		close(devFd);
		close(file_fd);
		printf("unable to set loop device\n");
		return -1;
	}

	struct loop_info64 li;
	memset(&li, 0, sizeof(li));
    strlcpy((char*) li.lo_crypt_name, id, LO_NAME_SIZE);
    strlcpy((char*) li.lo_file_name, loopfile, LO_NAME_SIZE);

    if (ioctl(devFd, LOOP_SET_STATUS64, &li) < 0) {
        printf("Error setting loopback status (%s)\n", strerror(errno));
        close(file_fd);
        close(devFd);
        return -1;
    }

    close(devFd);
    close(file_fd);

    return 0;
}

int Loop::createImage(char* fsPath, char* imageName, unsigned numSectors) {
#if 0
	int fd = creat(imageName, 0700);
	if (fd < 0) {
		perror("creat");
		return -1;
	}

	if (0 > ftruncate(fd, selectorItems+1)) {
		perror("ftruncate");
		return -1;
	}
#endif
    int pid;
	int status;	
	char tmp[32];
	snprintf(tmp, sizeof(tmp), "%u", numSectors * 512);
    char *size = tmp;
    char *args[] = {MKEXT4FS_PATH, "-l", "10M", imageName, fsPath, 0};
#if 0
    args[0] = MKEXT4FS_PATH;
	
    
    args[1] = "-l";
    args[2] = size;
    args[3] = imageName;
	args[4] = fsPath;
	args[5] = NULL;
#endif

	pid = fork();
	if (pid == 0) {
		execvp(args[0], args);
	} else if (pid > 0) {
		wait(&status);
		if (WIFEXITED(status)) {
			printf("child exited normallly\n");
		} else {
			printf("child exited unexpected\n");
		}
	}

	return 0;
}

