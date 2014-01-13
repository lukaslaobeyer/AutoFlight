#ifndef VISUALPIPELINE_H
#define VISUALPIPELINE_H

#include <QGraphicsView>
#include <QContextMenuEvent>

#include "nodeseditor/qneblock.h"
#include "nodeseditor/qneport.h"
#include "nodeseditor/qnodeseditor.h"
#include "nodeseditor/qnenumberblock.h"

class VisualPipeline : public QGraphicsView
{
	Q_OBJECT

	public:
		enum BlockType {Number, ImageIn, ImageDisplay, GaussianBlur};

	public:
		explicit VisualPipeline(QWidget *parent = 0);

		void addBlock(BlockType type, int x = 0, int y = 0);

		void contextMenuEvent(QContextMenuEvent *e);

	public Q_SLOTS:
		void compile();

	private Q_SLOTS:
		void addBlock(int type);

	private:
		QGraphicsScene *s = NULL;
		QNodesEditor *nodesEditor = NULL;

		int addNodeAtX = 0, addNodeAtY = 0;
};

#endif
