#ifndef IMAGE_H
#define IMAGE_H
#include <stdio.h>
#include <utils/RefBase.h>
#include "AbsImage.h"

namespace android {

class Image : public RefBase {
public:
    Image();
	virtual ~Image();

	void setImageAction(sp<AbsImage> action){mImageStrategy = action;}
    void snapshot(const char* name, bool isPng);
	void rotate(int type);

private:
	sp<AbsImage> mImageStrategy;
};

};

#endif
