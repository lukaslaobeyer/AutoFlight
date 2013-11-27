#ifndef VISUALPIPELINE_H
#define VISUALPIPELINE_H

#include <QGraphicsView>

#include "nodeseditor/qneblock.h"
#include "nodeseditor/qneport.h"
#include "nodeseditor/qnodeseditor.h"

class VisualPipeline : public QGraphicsView
{
	Q_OBJECT

	public:
		enum BlockType {test};

	public:
		explicit VisualPipeline(QWidget *parent = 0);

		void addBlock(BlockType type, int x = 0, int y = 0);
	private:
		QGraphicsScene *s = NULL;

		QNEBlock *imgIn = NULL;
		QNEBlock *dummy = NULL;
		QNEBlock *imgDisp = NULL;
};

#endif
