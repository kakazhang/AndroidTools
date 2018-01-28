#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include <utils/RefBase.h>
#include <utils/Mutex.h>
#include <utils/Vector.h>

#include <vector>

#include "common.h"
#include "Link.h"
#include "CommandListener.h"

#define JSON_FILE "/system/etc/link.json"

using namespace std;
using namespace android;

class LinkManager : public RefBase {
public:
    virtual ~LinkManager();
    static sp<LinkManager> getInstance();

    void parseJsonFile();
    sp<CommandListener> getCmdListener() {
	    return mCmdListener;
	}

    void dump();
private:
    LinkManager();

    static sp<LinkManager> gInstance;
    static Mutex gInitLock;

    //Vector<LinkAttr*> mLinkAttrs;
    Vector<LinkAttr*> mLinkAttrs;
    //Link instances list
    KeyedVector<int,sp<Link>> mLinks;
   sp<CommandListener> mCmdListener; 
};

#endif
