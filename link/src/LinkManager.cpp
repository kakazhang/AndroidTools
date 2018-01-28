#define LOG_TAG "LinkManager"

#include <stdio.h>
#include <stdlib.h>

#include <utils/Log.h>

#include <fstream>
#include <json/reader.h>

#include "common.h"
#include "LinkFactory.h"
#include "LinkManager.h"

using namespace std;
using namespace android;

sp<LinkManager> LinkManager::gInstance = NULL;
Mutex LinkManager::gInitLock;

LinkManager::LinkManager() {
    parseJsonFile();
    int count = mLinkAttrs.size();
	for (int i = 0; i < count; i++) {
        LinkAttr* attr = mLinkAttrs.itemAt(i);
		if (attr->enable) {
		    sp<Link> newLink = LinkFactory::makeNewLink(attr);
			if (newLink != NULL) {
			    mLinks.add(attr->linkid,newLink);
                newLink->init();
			}
		}
    }
}

LinkManager::~LinkManager() {
}

sp<LinkManager> LinkManager::getInstance() {
    if (gInstance == NULL) {
        Mutex::Autolock __lock(gInitLock);
        if (gInstance == NULL)
            gInstance = new LinkManager();
    }
    return gInstance;
}

//all Link configurations are saved in a json file,parse it here
void LinkManager::parseJsonFile() {
    Json::Reader reader;
    Json::Value root;
    ifstream is;

    is.open(JSON_FILE, std::ios::binary);
    if (reader.parse(is, root)) {
        //parse uart config
        if (!root["uarts"].isNull()) {
            int size = root["uarts"].size();
            Json::Value child = root["uarts"];
            for (int i = 0; i < size; i++) {
                UartLinkAttr* uartLink = new UartLinkAttr();
                if (!child[i]["type"].isNull())
                    uartLink->common.type = atoi(child[i]["type"].asCString());
                if (!child[i]["linkid"].isNull())
                    uartLink->common.linkid = atoi(child[i]["linkid"].asCString());
                if (!child[i]["default_forwardid"].isNull())
                    uartLink->common.default_forwardid = atoi(child[i]["default_forwardid"].asCString());
                if (!child[i]["forward_only"].isNull())
                    uartLink->common.forward_only = atoi(child[i]["forward_only"].asCString());
                if (!child[i]["enable"].isNull())
                    uartLink->common.enable = atoi(child[i]["enable"].asCString());
                if (!child[i]["dev_path"].isNull())
                    strcpy(uartLink->dev_path, child[i]["dev_path"].asCString());
                if (!child[i]["speed"].isNull())
                    uartLink->speed = atoi(child[i]["speed"].asCString());
                if (!child[i]["flow_ctrl"].isNull())
                    uartLink->flow_ctrl = atoi(child[i]["flow_ctrl"].asCString());
                if (!child[i]["data_bits"].isNull())
                    uartLink->data_bits = atoi(child[i]["data_bits"].asCString());
                if (!child[i]["stop_bits"].isNull())
                    uartLink->stop_bits = atoi(child[i]["stop_bits"].asCString());
                if (!child[i]["parity"].isNull()) {
                    const char *str = child[i]["parity"].asCString();
                    uartLink->parity = str[0];
                }

                if (!child[i]["targets"].isNull()) {
                    Json::Value tgs = child[i]["targets"];
                    int len = tgs.size();
                    for (int j = 0; j < len; j++)
                        uartLink->common.targets.add(atoi(tgs[j].asCString()));

                }
                mLinkAttrs.add((LinkAttr *)uartLink);
            }
        }
        if (!root["sockets"].isNull()) {
            int count = root["sockets"].size();
            Json::Value socks = root["sockets"];
            for (int i = 0; i < count; i++) {
                SocketLinkAttr* sockAttr = new SocketLinkAttr();
                if (!socks[i]["type"].isNull())
                    sockAttr->common.type = atoi(socks[i]["type"].asCString());
                if (!socks[i]["linkid"].isNull())
                    sockAttr->common.linkid = atoi(socks[i]["linkid"].asCString());
                if (!socks[i]["default_forwardid"].isNull())
                    sockAttr->common.default_forwardid = atoi(socks[i]["default_forwardid"].asCString());
                if (!socks[i]["forward_only"].isNull())
                    sockAttr->common.forward_only = atoi(socks[i]["forward_only"].asCString());
                if (!socks[i]["enable"].isNull())
                    sockAttr->common.enable = atoi(socks[i]["enable"].asCString());
                if (!socks[i]["address"].isNull())
                    strcpy(sockAttr->address, socks[i]["address"].asCString());
                if (!socks[i]["port"].isNull())
                    sockAttr->port = atoi(socks[i]["port"].asCString());

                if (!socks[i]["targets"].isNull()) {
                    Json::Value tgs = socks[i]["targets"];
                    int len = tgs.size();
                    for (int j = 0; j < len; j++)
                        sockAttr->common.targets.add(atoi(tgs[j].asCString()));

                }
				mLinkAttrs.add((LinkAttr *)sockAttr);
            }
        }
    }
}

void LinkManager::dump() {
    //dump mLinkAttrs
    int count = mLinkAttrs.size();
    for (int i = 0; i < count; i++) {
        LinkAttr* attr = mLinkAttrs.itemAt(i);
        if (attr->type == LINK_TYPE_UART) {
            UartLinkAttr* uartAttr = (UartLinkAttr*)attr;
            printf("UartAttr(type=%d,linkid=%d,default_forwardid=%d,forward_only=%d,devPath=%s,speed=%d,flowctrl=%d, databits=%d, stopbits=%d,parity=%c)\n",
                    uartAttr->common.type, uartAttr->common.linkid, uartAttr->common.default_forwardid,uartAttr->common.forward_only,
                    uartAttr->dev_path,uartAttr->speed, uartAttr->flow_ctrl, uartAttr->data_bits, uartAttr->stop_bits,uartAttr->parity);
            int len = uartAttr->common.targets.size();
            if (len > 0) {
                for (int j = 0; j < len; j++) {
                    printf("targets[%d]=%d\n", j, uartAttr->common.targets.itemAt(j));
                }
            }
        } else {
            SocketLinkAttr* sockAttr = (SocketLinkAttr *)attr;
            printf("SockAttr(type=%d,linkid=%d,default_forwardid=%d,forward_only=%d,address=%s,port=%d)\n",
                    sockAttr->common.type, sockAttr->common.linkid, sockAttr->common.default_forwardid,sockAttr->common.forward_only,sockAttr->address,sockAttr->port);
            int len = sockAttr->common.targets.size();
            if (len > 0) {
                for (int j = 0; j < len; j++) {
                    printf("targets[%d]=%d\n", j, sockAttr->common.targets.itemAt(j));
                }
            }
        }
    }

    //dump mLinks
	count = mLinks.size();
    for (int i = 0; i < count; i++) {
        sp<Link> link = mLinks.valueAt(i);
        link->dump();
    }

}
