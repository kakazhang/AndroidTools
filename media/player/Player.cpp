#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <system/audio.h>
#include <media/mediaplayer.h>
#include <media/AudioSystem.h>

#include "Player.h"

using namespace android;
void Callback::notify(int msg, int ext1, int ext2, const Parcel *obj) {
	printf("msg:%d\n", msg);
    if (MEDIA_PREPARED == msg) {
		mHandler->sendMessage(Message(MSG_START));
	}
}

PlayerHandler::PlayerHandler(sp<MediaPlayer>& player) {
    mMediaPlayer = player;
    mLooper = new Looper(false);
}

void PlayerHandler::handleMessage(const Message& message) {
    switch(message.what) {
	case MSG_PREPARE_ASYNC:
		mMediaPlayer->prepareAsync();
		break;
    case MSG_PREPARE:
        mMediaPlayer->prepare();
		break;
	case MSG_START:
	    mMediaPlayer->start();
		break;
	case MSG_PAUSE:
		mMediaPlayer->pause();
		break;

	case MSG_STOP:
		mMediaPlayer->stop();
		break;

	default:
		break;
	}
}

void PlayerHandler::sendMessage(const Message& message) {
    mLooper->sendMessage(this, message);
}

bool PlayerHandler::threadLoop() {
    mLooper->pollOnce(-1);
	return true;
}


Player::Player(const char* source)
   :mDataSource(source) {
   mPlayer = new MediaPlayer();

   mHandler = new PlayerHandler(mPlayer);
   mHandler->run("PlayerHandler");
   mCallback = new Callback(mHandler);
  
   mPlayer->setListener(mCallback);

   mFd = open(source, O_RDONLY);
   if (mFd < 0) {
       perror("open");
	   exit(-1);
   }

   mLength = lseek(mFd, 0, SEEK_END);
   mPlayer->setDataSource(mFd, 0, mLength);
   mPlayer->setAudioStreamType(AUDIO_STREAM_MUSIC);
}

Player::~Player() {
    if (mFd > 0)
		close(mFd);
}

status_t Player::prepareAsync() {
    mHandler->sendMessage(Message(MSG_PREPARE_ASYNC));
	return NO_ERROR;
}

status_t Player::prepare() {
    mHandler->sendMessage(Message(MSG_PREPARE));
	return NO_ERROR;
}

status_t Player::start() {
    mHandler->sendMessage(Message(MSG_START));
	return NO_ERROR;
}

status_t Player::pause() {
    mHandler->sendMessage( Message(MSG_PAUSE));
	return NO_ERROR;  
}

bool Player::isPlaying() {
    return mPlayer->isPlaying();
}

status_t Player::stop() {
    mHandler->sendMessage(Message(MSG_STOP));
	return NO_ERROR;
}

status_t Player::seekTo(int msec) {
	mPlayer->seekTo(msec);
    return NO_ERROR;
}
