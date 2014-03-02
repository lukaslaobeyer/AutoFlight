#ifndef INODEATTRIBUTELISTENER_H
#define INODEATTRIBUTELISTENER_H

#include "qnenumberblock.h"

class INodeAttributeListener
{
	public:
		virtual ~INodeAttributeListener() {}
	    virtual void attributeChanged(QNENumberBlock *numberNode, double number) = 0;
};

#endif
