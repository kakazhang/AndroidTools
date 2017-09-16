#ifndef SHOW_H
#define SHOW_H

#include <gui/Surface.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>

#include <ui/PixelFormat.h>
#include <ui/DisplayInfo.h>

#include <utils/Thread.h>

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

using namespace android;

class HorseLight;
class GraphicsDisplay : public RefBase {
public:
	GraphicsDisplay();
    virtual ~GraphicsDisplay() {}
    void startLight();
protected:
   status_t init();
   
private:
    sp<SurfaceComposerClient> mSurfaceComposerClient;
    sp<SurfaceControl> mSurfaceControl;
    DisplayInfo gDisplayInfo;
	sp<HorseLight> mLight;
};

class HorseLight : public Thread {
public:
	HorseLight(sp<Surface> surface);
    virtual ~HorseLight() {}
private:
	void showColor(int color);
	virtual bool threadLoop();
	sp<Surface> mSurface;
};

#endif
