#ifndef IPLAYERINTERFACE_H
#define IPLAYERINTERFACE_H

#include <utils/RefBase.h>
using namespace android;

class IPlayerInterface : public RefBase {
public:
	IPlayerInterface() {}
	virtual ~IPlayerInterface(){}
	virtual status_t prepareAsync() = 0;
    virtual status_t prepare() = 0;
    virtual status_t start() = 0;
	virtual status_t pause() = 0;
	virtual bool isPlaying() = 0;
	virtual status_t stop() = 0;
	virtual status_t seekTo(int msec) = 0;

};

#endif
