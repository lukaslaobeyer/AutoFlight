#ifndef INODESEDITORLISTENER_H
#define INODESEDITORLISTENER_H

#include "qneblock.h"

class INodesEditorListener
{
	public:
		virtual ~INodesEditorListener() {}
	    virtual void nodeAdded(QNEBlock *node) = 0;
	    virtual void nodeDeleted(int id, int nodeType) = 0;
	    virtual void connectionMade(QNEBlock *node1, QNEBlock *node2) = 0;
	    virtual void connectionDeleted(QNEBlock *node1, QNEBlock *node2) = 0;
	    virtual void attributeSet(QNEBlock *node, int attribute, double number) = 0;
};

#endif
