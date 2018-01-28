#ifndef ILINKFACTORY_H
#define ILINKFACTORY_H

#include "common.h"
#include "Link.h"

/*interface of link factory*/
class ILinkFactory {
public:
    ILinkFactory() {}
    virtual ~ILinkFactory() {}

    virtual sp<Link> makeNewLink(LinkAttr* attr) = 0;
};

#endif
