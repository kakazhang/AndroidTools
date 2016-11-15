/*************************************************************************
	> File Name: losetup.c
	> Author: kaka.zhang
	> Mail: kaka.zhang@snowballtech.com 
	> Created Time: Tue 18 Aug 2015 03:37:10 PM CST
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/loop.h>

#define LOOP_MAX 4096

int destroyByDevice(const char *loopDevice) {
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

int create(char *id, const char *loopfile, const char* loopdev, int search) {
	int i;
	char loopdevice[256] = {0};
	int file_fd = -1;
	int devFd = -1;
	int rc = 0;
	
  if (search) {
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
	  } else {
	  	struct loop_info64 li;
	   	devFd = open(loopdev, O_RDWR);
			if (devFd < 0) {
					perror("open");
					return -1;
			}
		
			rc = ioctl(devFd, LOOP_GET_STATUS64, &li);
			if (rc < 0 && errno == ENXIO) {
			    printf("%s is ok\n", loopdev);
			    goto out;
			}
      
      if (rc < 0) {
          printf("%s busy\n", loopdev);	
      }
	  }
	  
out:
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

int main(int argc, char** argv) {
	  printf("argc:%d\n",argc);
	  if (argc != 3) {
	  	printf("too few arguments\n");
	  	return -1;
	  }
	  
	  if (!strcmp(argv[1], "-d")) {
	       destroyByDevice(argv[2]);
	  } else {
	  	    char* loop_file = argv[2];
	  	    char* loop_dev = argv[1];
	      	create(loop_file, loop_file, loop_dev, 0);
	  }
		return 0;
}
