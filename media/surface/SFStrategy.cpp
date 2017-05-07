#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <binder/ProcessState.h>

#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <ui/PixelFormat.h>

#include <SkImageEncoder.h>
#include <SkBitmap.h>
#include <SkData.h>
#include <SkStream.h>


#include "SFStrategy.h"

namespace android {
static uint32_t DEFAULT_DISPLAY_ID = ISurfaceComposer::eDisplayIdMain;
static SkColorType flinger2skia(PixelFormat f)
{
    switch (f) {
        case PIXEL_FORMAT_RGB_565:
            return kRGB_565_SkColorType;
        default:
            return kN32_SkColorType;
    }
}

SFStrategy::SFStrategy() {

}

SFStrategy::~SFStrategy() {
    if (mFd > 0)
		close(mFd);
}

void SFStrategy::snapshot(const char* name,bool isPng) {
	ScreenshotClient screenshot;
    ScreenInfo info;
	mFd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (mFd < 0) {
        perror("open");
		exit(-1);
	}

	getScreenInfo(screenshot, &info);
	void const* base = screenshot.getPixels();
    if (base == NULL)
		return;

	if (isPng) {
		const SkImageInfo skinfo = SkImageInfo::Make(info.width, info.height, flinger2skia(info.format),
												   kPremul_SkAlphaType);
		SkBitmap b;
		b.installPixels(skinfo, const_cast<void*>(base), (info.stride)*bytesPerPixel(info.format));
		SkDynamicMemoryWStream stream;
		SkImageEncoder::EncodeStream(&stream, b,
				SkImageEncoder::kPNG_Type, SkImageEncoder::kDefaultQuality);
		SkData* streamData = stream.copyToData();
		write(mFd, streamData->data(), streamData->size());
		streamData->unref();
	} else {
		write(mFd, &info.width, 4);
		write(mFd, &info.height, 4);
		write(mFd, &info.format, 4);
		size_t Bpp = bytesPerPixel(info.format);
		for (size_t y=0 ; y< info.height ; y++) {
			write(mFd, base, info.width*Bpp);
			base = (void *)((char *)base + info.stride*Bpp);
		}
	}

}

void SFStrategy::rotate(int type) {

}

void SFStrategy::getScreenInfo(ScreenshotClient& screenshot,pInfo info) {
	sp<IBinder> display = SurfaceComposerClient::getBuiltInDisplay(DEFAULT_DISPLAY_ID);
    if (display != NULL && screenshot.update(display, Rect(), false) == NO_ERROR) {
		//const void* base = screenshot.getPixels();
        info->width = screenshot.getWidth();
        info->height = screenshot.getHeight();
        info->stride = screenshot.getStride();
        info->format = screenshot.getFormat();
        info->size = screenshot.getSize();
    } else {
    	info->width = info->height = info->stride = info->format = info->size = 0;
	}
}

};
