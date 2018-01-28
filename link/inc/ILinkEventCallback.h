#ifndef ILINKEVENTCALLBACK_H
#define ILINKEVENTCALLBACK_H

#include <utils/RefBase.h>

using namespace android;

class ILinkEventCallback : virtual public RefBase {
public:
    virtual void callback(void *buf, int len) = 0;
};

#endif
