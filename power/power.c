/**
*  Power module for control cpu status , 
* 1) init cpu initialize status for user
* 2) change max cpu freq when  screen status changed between on and off
* 3) change goto hispeedload when  screen status changed between on and off
*/
#include <hardware/hardware.h>
#include <hardware/power.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define POWER_MODULE_ID "powersaver"
#define POWER_MODULE_NAME "power module"
#define POWER_API_VERSION HARDWARE_MAKE_API_VERSION(1, 0)

#define DEFAULT_LOAD "85"
#define NORMAL_LOAD "75"
#define LOWPOWER_LOAD "90"
#define TIMER_RATE "20000"
#define MIN_SAMPLE_RATE "30000"

#define DEFAULT_BOOST "0"
#define NORMAL_BOOST "1"

#define LOWPOWER_MAX_FREQ "700000"
#define NORMAL_MAX_FREQ "1300000"


#define BOOST_FACTOR "/sys/devices/system/cpu/cpufreq/interactive/boost_factor" 
#define TIME_RATE "/sys/devices/system/cpu/cpufreq/interactive/timer_rate"
#define MIN_SAMPLE_TIME "/sys/devices/system/cpu/cpufreq/interactive/min_sample_time"

#define INPUT_BOOST "/sys/devices/system/cpu/cpufreq/interactive/input_boost"
#define GO_HISPEED_LOAD "/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load" 
#define GO_MAXSPEED_LOAD "/sys/devices/system/cpu/cpufreq/interactive/go_maxspeed_load" 

#define MAX_CPUS 4

static const char *cpu_path_max[] = {
    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq",
	"/sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq",
	"/sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq",
	"/sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq",
};

static const char *cpu_path_min[] = {
    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq",
	"/sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq",
	"/sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq",
	"/sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq",
};

static void sys_write(const char* path, const char* msg) {
	int fd = open(path, O_WRONLY);
	if (fd < 0) {
        perror("open");
		return;
	}

	write(fd, msg, strlen(msg));
    //printf("write to %s : %s\n", path, msg);
	close(fd);
}

static void power_init(__attribute__((unused)) struct power_module *module) {
	sys_write(INPUT_BOOST, DEFAULT_BOOST);
	
	sys_write(GO_HISPEED_LOAD, DEFAULT_LOAD);
	sys_write(GO_MAXSPEED_LOAD, DEFAULT_LOAD);
	
	sys_write(TIME_RATE, TIMER_RATE);
	sys_write(MIN_SAMPLE_TIME, MIN_SAMPLE_RATE);
}

static void power_setInteractive(__attribute__((unused)) struct power_module *module, int on) {
    if (on) {
        sys_write(INPUT_BOOST, NORMAL_BOOST);
		sys_write(GO_HISPEED_LOAD, NORMAL_LOAD);
		sys_write(GO_MAXSPEED_LOAD, NORMAL_LOAD);
	} else {
        sys_write(INPUT_BOOST, DEFAULT_BOOST);
		sys_write(GO_HISPEED_LOAD, DEFAULT_LOAD);
		sys_write(GO_MAXSPEED_LOAD, DEFAULT_LOAD);
	}
}

//low power status
static void power_save() {
    int i;
	//set max freq
	for (i = 0; i < MAX_CPUS; i++) {
        sys_write(cpu_path_max[i],LOWPOWER_MAX_FREQ);
	}

	//set goto hispeed load
	sys_write(GO_HISPEED_LOAD, LOWPOWER_LOAD);
}

static void powerHint(__attribute__((unused)) struct power_module *module, power_hint_t hint,
				  void *data) {
    switch (hint) {
    case POWER_HINT_VSYNC:
		break;
	case POWER_HINT_LOW_POWER:
		//power save mode
		power_save();
		break;
	default:
		break;
	}
}

static void power_setFeature(__attribute__((unused)) struct power_module *module, feature_t feature, int state) {

}

static hw_module_methods_t gmethods = {
    .open = NULL,
};

power_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
		.hal_api_version = POWER_API_VERSION,
		.id = POWER_MODULE_ID,
		.name = POWER_MODULE_NAME,
		.author = "zhanglin3406@gmail.com",
		.methods = &gmethods,
	},
	
	.init = power_init,
	.setInteractive = power_setInteractive,
	.powerHint = powerHint,
	.setFeature = power_setFeature,
};

