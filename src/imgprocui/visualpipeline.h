#ifndef VISUALPIPELINE_H
#define VISUALPIPELINE_H

#include <QGraphicsView>
#include <QContextMenuEvent>
#include <vector>
#include <string>

#include "nodeseditor/qneblock.h"
#include "nodeseditor/qneport.h"
#include "nodeseditor/qnodeseditor.h"
#include "nodeseditor/qnenumberblock.h"
#include "nodeseditor/inodeseditorlistener.h"

class VisualPipeline : public QGraphicsView, public INodesEditorListener
{
	Q_OBJECT

	public:
		enum BlockType {Number, ImageIn, ImageDisplay, GaussianBlur};

	public:
		explicit VisualPipeline(QWidget *parent = 0);

		void contextMenuEvent(QContextMenuEvent *e);

		void nodeAdded(QNEBlock *node);
		void nodeDeleted(int id, int type);
		void connectionMade(QNEBlock *node1, QNEBlock *node2);
		void connectionDeleted(QNEBlock *node1, QNEBlock *node2);
		void attributeSet(QNEBlock *node, int attribute, double value);

	public Q_SLOTS:
		void compile();

	private Q_SLOTS:
		void addBlock(int type);

	private:
		QGraphicsScene *s = NULL;
		QNodesEditor *nodesEditor = NULL;

		int addNodeAtX = 0, addNodeAtY = 0;

		std::vector<std::string> nodes;
};

#endif
