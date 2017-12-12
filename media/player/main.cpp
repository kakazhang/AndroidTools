/*************************************************************************
	> File Name: main.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年05月06日 星期六 17时57分11秒
 ************************************************************************/

#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include "Player.h"

using namespace android;
int main(int argc, char** argv) {
    if (argc < 1) {
        printf("Input arguments:\n");
        return -1;
    }

    char* path = argv[1];
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    
    sp<Player> player = new Player(path);

    player->prepare();

    sleep(10);

    //player->pause();
    //sleep(2);
    player->start();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
