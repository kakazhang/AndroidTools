#include <stdio.h>
#include <binder/ProcessState.h>

#include "Image.h"
#include "SFStrategy.h"

using namespace android;

int main() {
   const char* name = "/sdcard/test.png";
  
   ProcessState::self()->startThreadPool();
   sp<SFStrategy> strategy = new SFStrategy();
   sp<Image> image = new Image();

   
   image->setImageAction(strategy);
   image->snapshot(name,true);

   return 0;
}
