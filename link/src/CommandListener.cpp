#define LOG_TAG "CommandListener"

#include <stdio.h>
#include <utils/Log.h>

#include "CommandListener.h"

using namespace android;

CommandListener :: CommandListener() {
   //register all commands
}

CommandListener::~CommandListener() {

}

void CommandListener::registerCmd(sp<ICommand> cmd) {
    mCommands.add(cmd);
}

void CommandListener::handleCmdWithAttr(LinkAttr* attr, uint8_t* cmdBuf, int32_t cmdLen) {

}

void CommandListener::notify(int32_t linkId, uint8_t *buf, int32_t len) {
   RWLock::AutoRLock __rlock(mRWLock);
   //FIXME: kakazhang 2018-01-15 find the very command to handle this
}

void CommandListener::notifyAll(uint8_t *buf, int32_t len) {
   int size;
   RWLock::AutoRLock __rlock(mRWLock);
   //notify all commands to handle this
   size = mCommands.size();
   for (int i = 0; i < size; i++) {
       sp<ICommand> cmd = mCommands.itemAt(i);
       //find the very receiver
   }
}

void CommandListener::registerCmdLocked(sp<ICommand> cmd) {
    RWLock::AutoWLock __wlock(mRWLock);
    registerCmd(cmd);
}
