#ifndef LINKFACTORY_H
#define LINKFACTORY_H

#include "ILinkFactory.h"
#include "UartLinkHandler.h"

class LinkFactory {
public:
    LinkFactory() {}
    virtual ~LinkFactory() {}

    static sp<Link> makeNewLink(LinkAttr *attr);
};

#endif
