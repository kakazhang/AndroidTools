#define LOG_TAG "ScreenRecorder"

#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <utils/Log.h>

#include <media/ICrypto.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>

#include "ScreenRecorder.h"
struct AMessage;

namespace android {
ScreenRecorder::ScreenRecorder(const char* mime, uint32_t bitRate, const char* outputName) {
    mWorker = new WorkThread(mime, bitRate, outputName);
}

ScreenRecorder::~ScreenRecorder() {
	stop();
}

void ScreenRecorder::start() {
    mWorker->start();
}

void ScreenRecorder::stop() {
    mWorker->stop();
}

WorkThread::WorkThread(const char* mime, uint32_t bitRate, const char* outputName)
    : kMime(mime), gBitRate(bitRate), mOutputName(outputName) {
    mStatus = STATUS_STOP;
	init();
}

void WorkThread::init() {
    status_t err;
    mDpy = SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain);
    err = SurfaceComposerClient::getDisplayInfo(mDpy, &mDisplayInfo);
    if (err != NO_ERROR) {
        ALOGE("unable to get display characteristics");
        exit(-1);
    }

    prepareEncoder();
    prepareVirtualDisplay();
    prepareMuxer();
}

status_t WorkThread::prepareEncoder() {
    status_t err;

    //configure format
    sp<AMessage> format = new AMessage;
    format->setInt32("width", mDisplayInfo.w);
    format->setInt32("height", mDisplayInfo.h);
    format->setString("mime", kMime);
    format->setInt32("color-format", OMX_COLOR_FormatAndroidOpaque);
    format->setInt32("bitrate", gBitRate);
    format->setFloat("frame-rate", mDisplayInfo.fps);
    format->setInt32("i-frame-interval", 10);

    //init looper
    sp<ALooper> looper = new ALooper;
    looper->setName("ScreenRecorder");
    looper->start();

    //create encoder with mime type
    mCodec = MediaCodec::CreateByType(looper, kMime, true);
    if (mCodec == NULL) {
        ALOGE("Unable to create %s codec", kMime);
        return UNKNOWN_ERROR;
    }

    //configure
    err = mCodec->configure(format, NULL, NULL, MediaCodec::CONFIGURE_FLAG_ENCODE);
    if (err != NO_ERROR) {
        ALOGE("ERROR: unable to configure %s codec at %dx%d (err=%d)\n",
                kMime, mDisplayInfo.w, mDisplayInfo.h, err);
        mCodec->release();
        return err;
    }

    //codec input buffer is input surface
    err = mCodec->createInputSurface(&mBufferProducer);
    if (err != NO_ERROR) {
        ALOGE("ERROR: unable to create encoder input surface (err=%d)\n", err);
        mCodec->release();
        return err;
    }

    err = mCodec->start();
    if (err != NO_ERROR) {
        ALOGE("ERROR: unable to start codec (err=%d)\n", err);
        mCodec->release();
        return err;
    }

    mCodec->getOutputBuffers(&mOutBuffers);
    return NO_ERROR;
}	

status_t WorkThread::prepareVirtualDisplay() {
    sp<IBinder> dpy = SurfaceComposerClient::createDisplay(
            String8("MyScreenRecorder"), false /*secure*/);

    SurfaceComposerClient::openGlobalTransaction();
    SurfaceComposerClient::setDisplaySurface(dpy, mBufferProducer);
    setDisplayProjection();
    SurfaceComposerClient::setDisplayLayerStack(dpy, 0);    // default stack
    SurfaceComposerClient::closeGlobalTransaction();

    mVirtualDpy = dpy;

	return NO_ERROR;
}

status_t WorkThread::prepareMuxer() {
    int fd = open(mOutputName, O_CREAT|O_RDWR, 0750);
    if (fd < 0) {
        fprintf(stderr, "open %s failed:%s\n", mOutputName, strerror(errno));
        return UNKNOWN_ERROR;
    }

    mMuxer = new MediaMuxer(fd, MediaMuxer::OUTPUT_FORMAT_MPEG_4);
    close(fd);
    return NO_ERROR;
}

status_t WorkThread::setDisplayProjection() {
	uint32_t sourceWidth, sourceHeight;
	sourceWidth = mDisplayInfo.w;
	sourceHeight = mDisplayInfo.h;

	Rect layerStackRect(sourceWidth, sourceHeight);
    // We need to preserve the aspect ratio of the display.
    float displayAspect = (float) sourceHeight / (float) sourceWidth;

    uint32_t videoWidth, videoHeight;
    uint32_t outWidth, outHeight;
	videoWidth = sourceWidth;
	videoHeight = sourceHeight;
    if (videoHeight > (uint32_t)(videoWidth * displayAspect)) {
        // limited by narrow width; reduce height
        outWidth = videoWidth;
        outHeight = (uint32_t)(videoWidth * displayAspect);
    } else {
        // limited by short height; restrict width
        outHeight = videoHeight;
        outWidth = (uint32_t)(videoHeight / displayAspect);
    }

    uint32_t offX, offY;
    offX = (videoWidth - outWidth) / 2;
    offY = (videoHeight - outHeight) / 2;
    Rect displayRect(offX, offY, offX + outWidth, offY + outHeight);
    SurfaceComposerClient::setDisplayProjection(mVirtualDpy,
            DISPLAY_ORIENTATION_0,
            layerStackRect, displayRect);
    return NO_ERROR;

}

bool WorkThread::threadLoop() {
    status_t err;
	int kTimeout = 250000;   // be responsive on signal
	ssize_t trackIdx = -1;

    while (mStatus != STATUS_STOP) {
		size_t bufIndex, offset, size;
        int64_t ptsUsec;
        uint32_t flags;

        err = mCodec->dequeueOutputBuffer(&bufIndex, &offset, &size, &ptsUsec,
                &flags, kTimeout);
        switch (err) {
 		case NO_ERROR: {
            // got a buffer
            if ((flags & MediaCodec::BUFFER_FLAG_CODECCONFIG) != 0) {
                ALOGV("Got codec config buffer (%zu bytes)", size);
                if (mMuxer != NULL) {
                    // ignore this -- we passed the CSD into MediaMuxer when
                    // we got the format change notification
                    size = 0;
                }
            }
		    if (size != 0) {
                if (ptsUsec == 0) {
                    ptsUsec = systemTime(SYSTEM_TIME_MONOTONIC) / 1000;
                }
				fprintf(stderr, "trackIdx=%d\n", trackIdx);
				err = mMuxer->writeSampleData(mOutBuffers[bufIndex], trackIdx,
						ptsUsec, flags);
				if (err != NO_ERROR) {
					fprintf(stderr,
						"Failed writing data to muxer (err=%d)\n", err);
					return err;
				}
            }
			err = mCodec->releaseOutputBuffer(bufIndex);
			if (err != NO_ERROR) {
				fprintf(stderr, "Unable to release output buffer (err=%d)\n",
						err);
				return err;
			}
			if ((flags & MediaCodec::BUFFER_FLAG_EOS) != 0) {
				// Not expecting EOS from SurfaceFlinger.  Go with it.
				ALOGI("Received end-of-stream");
				mStatus = STATUS_STOP;
			}
        }
            break;
		case -EAGAIN: {
		    ALOGV("Got -EAGAIN, looping");
			break;
		}
		case INFO_FORMAT_CHANGED: {
			// Format includes CSD, which we must provide to muxer.
			fprintf(stderr, "Encoder format changed\n");
			sp<AMessage> newFormat;
			mCodec->getOutputFormat(&newFormat);
			if (mMuxer != NULL) {
				trackIdx = mMuxer->addTrack(newFormat);
				fprintf(stderr, "start muxer track,idx=%d\n", trackIdx);
				err = mMuxer->start();
				if (err != NO_ERROR) {
					fprintf(stderr, "Unable to start muxer (err=%d)\n", err);
					return err;
				}
			}
		}
		    break;
        case INFO_OUTPUT_BUFFERS_CHANGED:
        {
            // Not expected for an encoder; handle it anyway.
            ALOGV("Encoder buffers changed");
            err = mCodec->getOutputBuffers(&mOutBuffers);
            if (err != NO_ERROR) {
                fprintf(stderr,
                        "Unable to get new output buffers (err=%d)\n", err);
                return err;
            }
        }
            break;
        case INVALID_OPERATION: {
            ALOGW("dequeueOutputBuffer returned INVALID_OPERATION");
            return err;
	    }
        default: {
            fprintf(stderr,
                    "Got weird result %d from dequeueOutputBuffer\n", err);
            return err;
		}
        }

   }

    if (STATUS_STOP == mStatus) {
        Mutex::Autolock _lock(gLock);
        gCond.signal();
        return false;
    }
    return true;
}

void WorkThread::start() {
    Mutex::Autolock _lock(gLock);
    if (mStatus != STATUS_START) {
        mStatus = STATUS_START;
        run("ScreenRecorder worker");
    }
}

void WorkThread::stop() {
    Mutex::Autolock _lock(gLock);
	if (mStatus == STATUS_START) {
		mStatus = STATUS_STOP;
        gCond.wait(gLock);
	}
	
	SurfaceComposerClient::destroyDisplay(mVirtualDpy);

	if (mCodec != NULL)
        mCodec->stop();

	if (mMuxer != NULL)
        mMuxer->stop();

    if (mCodec != NULL)
        mCodec->release();
}

}
