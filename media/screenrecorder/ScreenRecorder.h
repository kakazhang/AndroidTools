#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <binder/IBinder.h>

#include <media/stagefright/foundation/ABuffer.h>

#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/MediaMuxer.h>

#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <ui/DisplayInfo.h>

#include <utils/Condition.h>
#include <utils/Looper.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>
#include <utils/threads.h>
#include <utils/Vector.h>

#define STATUS_START 0x00
#define STATUS_STOP  0x01

namespace android {

class WorkThread;
//refereced from ScreenRecord.cpp of android
typedef enum OMX_COLOR_FORMATTYPE {
    OMX_COLOR_FormatUnused,
    OMX_COLOR_FormatMonochrome,
    OMX_COLOR_Format8bitRGB332,
    OMX_COLOR_Format12bitRGB444,
    OMX_COLOR_Format16bitARGB4444,
    OMX_COLOR_Format16bitARGB1555,
    OMX_COLOR_Format16bitRGB565,
    OMX_COLOR_Format16bitBGR565,
    OMX_COLOR_Format18bitRGB666,
    OMX_COLOR_Format18bitARGB1665,
    OMX_COLOR_Format19bitARGB1666,
    OMX_COLOR_Format24bitRGB888,
    OMX_COLOR_Format24bitBGR888,
    OMX_COLOR_Format24bitARGB1887,
    OMX_COLOR_Format25bitARGB1888,
    OMX_COLOR_Format32bitBGRA8888,
    OMX_COLOR_Format32bitARGB8888,
    OMX_COLOR_FormatYUV411Planar,
    OMX_COLOR_FormatYUV411PackedPlanar,
    OMX_COLOR_FormatYUV420Planar,
    OMX_COLOR_FormatYUV420PackedPlanar,
    OMX_COLOR_FormatYUV420SemiPlanar,
    OMX_COLOR_FormatYUV422Planar,
    OMX_COLOR_FormatYUV422PackedPlanar,
    OMX_COLOR_FormatYUV422SemiPlanar,
    OMX_COLOR_FormatYCbYCr,
    OMX_COLOR_FormatYCrYCb,
    OMX_COLOR_FormatCbYCrY,
    OMX_COLOR_FormatCrYCbY,
    OMX_COLOR_FormatYUV444Interleaved,
    OMX_COLOR_FormatRawBayer8bit,
    OMX_COLOR_FormatRawBayer10bit,
    OMX_COLOR_FormatRawBayer8bitcompressed,
    OMX_COLOR_FormatL2,
    OMX_COLOR_FormatL4,
    OMX_COLOR_FormatL8,
    OMX_COLOR_FormatL16,
    OMX_COLOR_FormatL24,
    OMX_COLOR_FormatL32,
    OMX_COLOR_FormatYUV420PackedSemiPlanar,
    OMX_COLOR_FormatYUV422PackedSemiPlanar,
    OMX_COLOR_Format18BitBGR666,
    OMX_COLOR_Format24BitARGB6666,
    OMX_COLOR_Format24BitABGR6666,
    OMX_COLOR_FormatKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    OMX_COLOR_FormatVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    /**<Reserved android opaque colorformat. Tells the encoder that
     * the actual colorformat will be  relayed by the
     * Gralloc Buffers.
     * FIXME: In the process of reserving some enum values for
     * Android-specific OMX IL colorformats. Change this enum to
     * an acceptable range once that is done.
     * */
    OMX_COLOR_FormatAndroidOpaque = 0x7F000789,
    /** Flexible 8-bit YUV format.  Codec should report this format
     *  as being supported if it supports any YUV420 packed planar
     *  or semiplanar formats.  When port is set to use this format,
     *  codec can substitute any YUV420 packed planar or semiplanar
     *  format for it. */
    OMX_COLOR_FormatYUV420Flexible = 0x7F420888,

    OMX_TI_COLOR_FormatYUV420PackedSemiPlanar = 0x7F000100,
    OMX_QCOM_COLOR_FormatYVU420SemiPlanar = 0x7FA30C00,
    OMX_QCOM_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka = 0x7FA30C03,
    OMX_SEC_COLOR_FormatNV12Tiled = 0x7FC00002,
    OMX_QCOM_COLOR_FormatYUV420PackedSemiPlanar32m = 0x7FA30C04,
    OMX_COLOR_FormatMax = 0x7FFFFFFF
} OMX_COLOR_FORMATTYPE;

class ScreenRecorder : public RefBase {
public:
    ScreenRecorder(const char* mime, uint32_t bitRate, const char* outputName);
    virtual ~ScreenRecorder();

	void start();
    void stop();

private:
    sp<WorkThread> mWorker;
};

class WorkThread : public Thread {
public:
    WorkThread(const char* mime, uint32_t bitRate, const char* outputName);
    virtual ~WorkThread() {}

	void start();
    void stop();
	
protected:
    void init();
    status_t prepareEncoder();
	status_t prepareVirtualDisplay();
    status_t prepareMuxer();

    virtual bool threadLoop();

private:
	status_t setDisplayProjection();
private:
    const char* mOutputName;
    const char* kMime;
    uint32_t gBitRate;

    //main Display
    sp<IBinder> mDpy;
    DisplayInfo mDisplayInfo;

    //virtual display
    sp<IBinder> mVirtualDpy;
	
    //encode video
    sp<MediaCodec> mCodec;
    //save video muxer
    sp<MediaMuxer> mMuxer;
	//dequeue outputbuffers
    Vector<sp<ABuffer> > mOutBuffers;

    sp<IGraphicBufferProducer> mBufferProducer;

    volatile int mStatus;
    Mutex gLock;
    Condition gCond;
};
}

#endif
