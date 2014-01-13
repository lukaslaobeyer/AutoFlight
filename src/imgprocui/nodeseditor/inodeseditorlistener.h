#ifndef INODESEDITORLISTENER_H
#define INODESEDITORLISTENER_H

class INodesEditorListener
{
	public:
		virtual ~INodesEditorListener() {}
	    virtual void nodeAdded(int id, int nodeType);
	    virtual void nodeDeleted(int id, int nodeType);
	    virtual void connectionMade(int node1_id, int node2_id);
};

#endif
