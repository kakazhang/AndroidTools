#ifndef COMMANDLISTENER_H
#define COMMANDLISTENER_H

#include <utils/RefBase.h>
#include <utils/RWLock.h>
#include <utils/Vector.h>

#include "common.h"

using namespace android;

class ICommand : virtual public RefBase {
public:
    ICommand(unsigned char ucCmdSet, unsigned char ucCmdID)
        : mCmdSet(ucCmdSet), mCmdID(ucCmdID) {
    }
    virtual ~ICommand() {
    }

    virtual int onCommand(LinkAttr* attr, uint8_t* cmdBuf, int32_t cmdLen) = 0;

    inline unsigned char getCmdSet() {return mCmdSet;}
    inline unsigned char getCmdID() {return mCmdID;}
private:
    unsigned char mCmdSet;
    unsigned char mCmdID;
};

class CommandListener : virtual public RefBase {
public:
    CommandListener();
    virtual ~CommandListener();

    //api for out users
    void registerCmdLocked(sp<ICommand> cmd);
    void handleCmdWithAttr(LinkAttr* attr, uint8_t* cmdBuf, int32_t cmdLen);
	void notify(int32_t linkId, uint8_t *buf, int32_t len);
    void notifyAll(uint8_t* buf, int32_t len);
private:
    void registerCmd(sp<ICommand> cmd);
    //RWLock for mCommands
    RWLock mRWLock;
    Vector<sp<ICommand>> mCommands;
};

#endif
