#include <hardware/hardware.h>
#include <hardware/lights.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BACKLIGHT_ID "backlight"
#define BACKLIGHT_DEV_VERSION HARDWARE_DEVICE_API_VERSION(1, 0)

#define BACKLIGHT_PATH "/sys/class/backlight/pwm-backlight/brightness"

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

static int backlight_close(struct hw_device_t* device);

static int rgb_to_brightness(struct light_state_t const *state)
{
	int color = state->color & 0x00ffffff;
	return ((77 * ((color >> 16) & 0x00ff))
		+ (150 * ((color >> 8) & 0x00ff)) +
		(29 * (color & 0x00ff))) >> 8;
}

int write_int(const char* path, int brightness) {
	char buf[20] = {0};
	int fd = open(path, O_WRONLY);
    if (fd < 0) {
		char *errmsg = strerror(errno);
        printf("%s\n", errmsg);
		return -1;
	}

	int count = snprintf(buf, sizeof(buf), "%d", brightness);
	pthread_mutex_lock(&gMutex);
	int ret = write(fd, buf, count);
	if (ret < 0) {
        char *errmsg = strerror(errno);
        printf("%s\n", errmsg);
	}
	pthread_mutex_unlock(&gMutex);
	
	close(fd);
	return ret;
}

static int set_backlight(struct light_device_t* dev,
		struct light_state_t const* state) {
    int brightness = rgb_to_brightness(state);
	
    return write_int(BACKLIGHT_PATH, brightness);
}

static int backlight_open(const struct hw_module_t* module, const char* id,
            struct hw_device_t** device) {
    struct light_device_t *light_dev = NULL;

	light_dev = (struct light_device_t *)malloc(sizeof(struct light_device_t));
	if (!light_dev) {
        perror("malloc");
		return -1;
	}

	light_dev->common.module = module;
	light_dev->common.tag = HARDWARE_DEVICE_TAG;
	light_dev->common.version = BACKLIGHT_DEV_VERSION;
	light_dev->common.close = backlight_close;
	light_dev->set_light = set_backlight;
    
	*device = (hw_device_t *)light_dev;

	return 0;
}

static int backlight_close(struct hw_device_t* device) {
    if (device) {
        free(device);
	}

	return 0;
}

static hw_module_methods_t gmethods = {
    .open = backlight_open,
};

hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
	.module_api_version = HARDWARE_HAL_API_VERSION,
	.hal_api_version = HARDWARE_HAL_API_VERSION,
	.id = BACKLIGHT_ID,
	.name = "backlight",
	.author = "zhanglin3406@gmail.com",
	.methods = &gmethods,
};
