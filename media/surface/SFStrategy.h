#ifndef SFSTRATEGY_H
#define SFSTRATEGY_H
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include "AbsImage.h"

namespace android {
typedef struct ScreenInfo {
    uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t stride;
	size_t size;
} ScreenInfo, *pInfo;

class SFStrategy : public AbsImage {
public:
	SFStrategy();
	virtual ~SFStrategy();
	virtual void snapshot(const char* name,bool isPng);
	virtual void rotate(int type);

private:
	void getScreenInfo(ScreenshotClient& screenshot,pInfo info);
private:
	const char* mPicName;
	int mFd;
};

};
#endif
