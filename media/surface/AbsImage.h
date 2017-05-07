#ifndef ABSIMAGE_H
#define ABSIMAGE_H

#include <utils/RefBase.h>
namespace android {

class AbsImage : public RefBase {
public:
	virtual void snapshot(const char* name,bool isPng) = 0;
	virtual void rotate(int type) = 0;
};
};
#endif
