#include <stdio.h>

#include <android/native_window.h>
#include <cutils/memory.h>

#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <ui/PixelFormat.h>

#include "Show.h"

using namespace android;

GraphicsDisplay::GraphicsDisplay() {
	init();
}

status_t GraphicsDisplay::init() {
    mSurfaceComposerClient = new SurfaceComposerClient();
    sp<IBinder> dpy = mSurfaceComposerClient->getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain);
    mSurfaceComposerClient->getDisplayInfo(dpy, &gDisplayInfo);

    mSurfaceControl = mSurfaceComposerClient->createSurface(String8("Show"), 
		    gDisplayInfo.w, gDisplayInfo.h, PIXEL_FORMAT_RGB_565);
    SurfaceComposerClient::openGlobalTransaction();  
    mSurfaceControl->setLayer(100000);
    mSurfaceControl->setPosition(0, 0);
    SurfaceComposerClient::closeGlobalTransaction();  

	return NO_ERROR;
}

void GraphicsDisplay::startLight() {
    sp<Surface> surface = mSurfaceControl->getSurface();
	mLight = new HorseLight(surface);

    mLight->run("horse light");
}

HorseLight::HorseLight(sp<Surface> surface) {
    mSurface = surface;
}

void HorseLight::showColor(int color) {
    ANativeWindow_Buffer buffer;
    int32_t COLOR;
    if (color == COLOR_RED)
		COLOR = 0xF800;
    else if (color == COLOR_GREEN)
		COLOR = 0x548B54;
    else if (color == COLOR_BLUE)
		COLOR = 0x191970;
    else
		return;

    mSurface->lock(&buffer, NULL);
    size_t size = buffer.stride * buffer.height * bytesPerPixel(buffer.format);
    android_memset16((uint16_t *)buffer.bits, COLOR, size);
    mSurface->unlockAndPost();
}

bool HorseLight::threadLoop() {
    for (int i = 0; i < 3; i++) {
        showColor(i);
        sleep(2);
    }
    return true;
}
