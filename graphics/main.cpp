#include <stdio.h>

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

#include "Show.h"
using namespace android;

int main() {
   ProcessState::self()->startThreadPool();
  
   sp<GraphicsDisplay> gd = new GraphicsDisplay();
   gd->startLight();

   IPCThreadState::self()->joinThreadPool();
   return 0;
}
