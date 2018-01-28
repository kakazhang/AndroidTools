#ifndef LINKEVENT_H
#define LINKEVENT_H

#include <utils/RefBase.h>
#include <media/stagefright/foundation/AHandler.h>

#include "ILinkEventCallback.h"

#define PROTOCOL_MAGIC     0x55

#define MAX_CMD_LEN        20*1024
#define MAX_PACKET_LEN     2048

using namespace android;

// event parser status
enum {
    NO_MAGIC,
    FIND_VER_LEN_CRC,
    FIND_BODY,
};

/*For parse link events*/
class LinkEvent : public RefBase {
public:
    LinkEvent(sp<ILinkEventCallback> callback) {
        mCallback = callback;
    }
    virtual ~LinkEvent() {}

    void parseData(uint8_t* buf, int32_t len);

private:
    sp<ILinkEventCallback> mCallback;

    uint8_t mRecvBuffer[MAX_CMD_LEN];
    //current position in mRecvBuffer
    int mPos;
};
#endif
