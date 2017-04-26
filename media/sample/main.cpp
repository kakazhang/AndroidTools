/*************************************************************************
	> File Name: main.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年04月26日 星期三 14时29分19秒
 ************************************************************************/

#include <media/SoundPool.h>
#include <stdio.h>
#include <stdlib.h>
#include <system/audio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BG_MUSIC "/sdcard/BGMusic.ogg"

using namespace android;

static void CHECK(bool cond, const char *msg) {
    if (!cond) {
        printf("%s\n",msg);
        exit(-1);
    }
}

int getMusicLength(int fd) {
   int length = lseek(fd, 0, SEEK_END);
    
   return length;
}

int construct_attribute(audio_attributes_t **attr) {
    audio_attributes_t *paa = NULL;
    paa = (audio_attributes_t *)calloc(1, sizeof(audio_attributes_t));
    CHECK((paa!= NULL), "no memory");

    memset(paa, 0, sizeof(audio_attributes_t));
    paa->content_type = AUDIO_CONTENT_TYPE_MUSIC;
    paa->usage = AUDIO_USAGE_MEDIA;
    paa->flags = 0x0;

    *attr = paa;
    return 1;
}

int load(SoundPool* _sp, const char* path) {
    int fd, length;
    
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    length = getMusicLength(fd);
    if (length < 0) {
        printf("length is error\n");
        close(fd);
        return -1;
    }

    int id = _sp->load(fd, 0, length, 0);
    close(fd);
    return id;
}

void start() {
    int id;
    audio_attributes_t *paa = NULL;
    SoundPool* sp = NULL;

    if (!construct_attribute(&paa))
        return;

    sp =  new SoundPool(5, paa);
    CHECK((sp!=NULL), "no memory");

    id = load(sp, BG_MUSIC);

    sleep(5);

    int ret = sp->play(id, 1, 1, 0,0,1);

    free(paa);
    delete sp;
}

int main() {
    start();

    sleep(5);
    return 0;
}
