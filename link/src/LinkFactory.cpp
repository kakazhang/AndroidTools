#define LOG_TAG "LinkFactory"
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <utils/Log.h>

#include "LinkFactory.h"
#include "UartLinkHandler.h"
#include "TcpLinkHandler.h"
#include "TcpServerLinkHandler.h"

sp<Link> LinkFactory::makeNewLink(LinkAttr *attr) {
    sp<Link> link = NULL;

    if (attr == NULL) {
        ALOGE("attr is NULL\n");
        return NULL;
    }

    int type = attr->type;
    if (type == LINK_TYPE_UART) {
        UartLinkAttr * uartLinkAttr = (UartLinkAttr *)attr;
        const char *devPath = uartLinkAttr->dev_path;
        int speed = uartLinkAttr->speed;
        int flow_ctrl = uartLinkAttr->flow_ctrl;
        int data_bits = uartLinkAttr->data_bits;
        int stop_bits = uartLinkAttr->stop_bits;
        int parity = uartLinkAttr->parity;

        link = new UartLinkHandler(devPath, speed, flow_ctrl, data_bits, stop_bits, parity, 1000);
    } else if (type == LINK_TYPE_TCP || type == LINK_TYPE_TCPSERVER) {
        SocketLinkAttr* sockAttr = (SocketLinkAttr *)attr;
        if (type == LINK_TYPE_TCP)
            link = new TcpLinkHandler(sockAttr);
        else if (type == LINK_TYPE_TCPSERVER)
            link = new TcpServerLinkHandler(sockAttr);
    }

    return link;
}
