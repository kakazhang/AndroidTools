#include <stdio.h>
#include <string.h>

#include "../inc/common.h"
using namespace android;

typedef struct TestLinkAttr {
    LinkAttr common;
    char* dev_path;
    int32_t speed;
    int32_t flowCtrl;
    int32_t dataBits;
} TestLinkAttr;

int getUartLink(LinkAttr** linkAttr) {
    TestLinkAttr* attr = new TestLinkAttr();
    attr->common.type = LINK_TYPE_UART;
    attr->common.linkid = 0;
    attr->common.default_forwardid = 1;
    attr->common.forward_only = 0;

    attr->dev_path = new char[36];
    memset(attr->dev_path, 0, 36);
    memcpy(attr->dev_path, "/dev/ttyS4", strlen("/dev/ttyS4"));

    attr->speed = 1500000;
    attr->flowCtrl = 2;
    attr->dataBits = 8;

    *(linkAttr) = (LinkAttr *)attr;
    return 0;
}

int main() {
    TestLinkAttr* uartAttr;

    getUartLink((LinkAttr **)&uartAttr);
    printf("UartAttr(type=%d,linkid=%d,default_forwardid=%d,forward_only=%d,devPath=%s,speed=%d,flowctrl=%d, databits=%d)\n",
            uartAttr->common.type, uartAttr->common.linkid, uartAttr->common.default_forwardid,uartAttr->common.forward_only,
            uartAttr->dev_path,uartAttr->speed, uartAttr->flowCtrl, uartAttr->dataBits);

    delete[] uartAttr->dev_path;
    delete uartAttr;
    return 0;
}
