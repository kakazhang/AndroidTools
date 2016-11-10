#ifndef ILISTENER_H
#define ILISTENER_H
#include <utils/String8.h>
#include <utils/RefBase.h>

namespace android {

class IListener : public RefBase {
public:
	IListener() {
	}

	virtual ~IListener() {
	}

	virtual void notify() = 0;
};
}
#endif