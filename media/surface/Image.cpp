
#include "Image.h"

namespace android {
Image::Image() {

}

Image::~Image() {

}

void Image::snapshot(const char* name,bool isPng) {
    mImageStrategy->snapshot(name,isPng);
}

void Image::rotate(int type) {
   mImageStrategy->rotate(type);
}


};
