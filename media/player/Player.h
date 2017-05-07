#ifndef PLAYER_H
#define PLAYER_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <system/audio.h>
#include <utils/threads.h>
#include <media/mediaplayer.h>
#include <media/AudioSystem.h>
#include <utils/Looper.h>

#include "IPlayerInterface.h"

using namespace android;

enum {
    MSG_SEEK = 0,
	MSG_PREPARE_ASYNC,
	MSG_PREPARE,
	MSG_START,
	MSG_PAUSE,
	MSG_STOP,
};

class PlayerHandler;

class Callback : public MediaPlayerListener {
public:
	Callback(sp<PlayerHandler>& handler) {
		mHandler = handler;
	}
    virtual void notify(int msg, int ext1, int ext2, const Parcel *obj);

private:
	sp<PlayerHandler> mHandler;
};


class Player : public IPlayerInterface {
public:
	Player(const char* source);
	virtual ~Player();
	virtual status_t prepareAsync();
	virtual status_t prepare();
    virtual status_t start();
	virtual status_t pause();
	virtual bool isPlaying();
	virtual status_t stop();
	virtual status_t seekTo(int msec);

private:
	const char* mDataSource;
	sp<MediaPlayer> mPlayer;

	sp<Callback> mCallback;
	sp<PlayerHandler> mHandler;

	int mFd;
	int mLength;
};


class PlayerHandler : public MessageHandler, public Thread {
public:
	PlayerHandler(sp<MediaPlayer>& player);
	virtual ~PlayerHandler(){}
	virtual void handleMessage(const Message& message);

	void sendMessage(const Message& message);
private:
	virtual bool threadLoop();
private:
	sp<MediaPlayer> mMediaPlayer;
	sp<Looper> mLooper;
};

#endif
