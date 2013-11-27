#include "visualpipeline.h"

#include <QtWidgets>

VisualPipeline::VisualPipeline(QWidget *parent) : QGraphicsView(parent)
{
	s = new QGraphicsScene();
	setScene(s);
	setRenderHint(QPainter::Antialiasing);

	///// Initialize and add default nodes

	QNodesEditor *nodesEditor = new QNodesEditor(this);
	nodesEditor->install(s);

	imgIn = new QNEBlock;
	s->addItem(imgIn);
	imgIn->addPort(tr("Image In"), 0, QNEPort::NamePort);
	imgIn->addPort(" ", 0, QNEPort::TypePort);
	imgIn->addOutputPort("out 1");
	imgIn->setPos(-200, 0);

	imgDisp = new QNEBlock;
	s->addItem(imgDisp);
	imgDisp->addPort(tr("Display Image"), 0, QNEPort::NamePort);
	imgDisp->addPort(" ", 0, QNEPort::TypePort);
	imgDisp->addInputPort("in 1");
	imgDisp->setPos(200, 0);

	///// Initialize node types added by the user

	dummy = new QNEBlock;
	s->addItem(dummy);
	dummy->addPort("Hello, World!", 0, QNEPort::NamePort);
	dummy->addPort("test", 0, QNEPort::TypePort);
	dummy->addInputPort("in1");
	dummy->addInputPort("in2");
	dummy->addInputPort("in3");
	dummy->addOutputPort("out1");
	dummy->addOutputPort("out2");
	dummy->addOutputPort("out3");
	dummy->setVisible(false);
}

void VisualPipeline::addBlock(BlockType type, int x, int y)
{
	if(type == test)
	{
		QNEBlock *newnode = dummy->clone();
		s->addItem(newnode);
		newnode->setPos(x, y);
	}
}
