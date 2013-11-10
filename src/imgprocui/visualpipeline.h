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
		explicit VisualPipeline(QWidget *parent = 0);
	private:
		QNEBlock *imgIn = NULL;
		QNEBlock *dummy = NULL;
		QNEBlock *imgDisp = NULL;
};

#endif
