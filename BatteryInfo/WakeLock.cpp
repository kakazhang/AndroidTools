#include <iostream>
#include <pthread.h>
#include <stdio.h>

#include "WakeLock.h"

#define BUFLEN 512
#ifndef KITKAT
#define WAKELOCKS "/proc/wakelocks"
#else
#define WAKELOCKS "/sys/power/wake_lock"
#endif
using namespace std;

WakeLock::WakeLock() {
	pthread_cond_init(&gCond, NULL);
	pthread_mutex_init(&gLock, NULL);
}

WakeLock::~WakeLock() {
	pthread_cond_destroy(&gCond);
	pthread_mutex_destroy(&gLock);
}

void WakeLock::notify() {
	pthread_mutex_lock(&gLock);
	pthread_cond_broadcast(&gCond);
	pthread_mutex_unlock(&gLock);
}

void WakeLock::showWakeLocks() {
	char buf[BUFLEN] = {0};
	FILE* fp = fopen(WAKELOCKS, "r");

	if (fp == NULL)
		return;
#ifndef KITKAT
	while (!feof(fp) && fgets(buf, BUFLEN, fp)) {
		if (strstr(buf, "alarm_rtc") || strstr(buf, "PowerManagerService.WakeLocks")
			|| strstr(buf, "alarm"))
			cout << buf; 
	}
#else	
	while (!feof(fp) && fgets(buf, BUFLEN, fp)) {
		cout << buf << endl;
	}
#endif
	fclose(fp);
}

bool WakeLock::threadLoop() {
	while (true) {
		pthread_mutex_lock(&gLock);
		pthread_cond_wait(&gCond, &gLock);
		pthread_mutex_unlock(&gLock);

		showWakeLocks();
	}
	return false;
}

