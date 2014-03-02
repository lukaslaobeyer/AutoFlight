#include "visualpipeline.h"
#include <iostream>
#include <QtWidgets>

using namespace std;

VisualPipeline::VisualPipeline(QWidget *parent) : QGraphicsView(parent)
{
	s = new QGraphicsScene();
	setScene(s);
	setRenderHint(QPainter::Antialiasing);

	s->setBackgroundBrush(QBrush(QPixmap(":/resources/visualpipeline_bg.png")));

	nodesEditor = new QNodesEditor(this);
	nodesEditor->install(s);
	nodesEditor->addNodesEditorListener(this);
}

void VisualPipeline::compile()
{

}

void VisualPipeline::addBlock(int type)
{
	nodesEditor->addBlock((QNodesEditor::BlockType) type, addNodeAtX, addNodeAtY);
}

void VisualPipeline::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);

	addNodeAtX = mapToScene(event->pos()).x();
	addNodeAtY = mapToScene(event->pos()).y();

	QMenu *addItem = menu.addMenu(tr("Add item"));
		QMenu *basic = addItem->addMenu(tr("Basic"));
			QAction *addImageIn = basic->addAction(tr("Image In"));
			QAction *addImageDisp = basic->addAction(tr("Image Display"));
			QAction *addNumber = basic->addAction(tr("Number"));
		QMenu *imgproc = addItem->addMenu(tr("Image Processing"));
			QAction *addGaussianBlur = imgproc->addAction(tr("Gaussian Blur"));
	QAction *compile = menu.addAction(tr("Compile"));

	QSignalMapper *signalMapper = new QSignalMapper(this) ;
	QObject::connect(addImageIn, SIGNAL(triggered()), signalMapper, SLOT(map()));
	QObject::connect(addImageDisp, SIGNAL(triggered()), signalMapper, SLOT(map()));
	QObject::connect(addNumber, SIGNAL(triggered()), signalMapper, SLOT(map()));
	QObject::connect(addGaussianBlur, SIGNAL(triggered()), signalMapper, SLOT(map()));

	signalMapper->setMapping(addImageIn, VisualPipeline::ImageIn);
	signalMapper->setMapping(addImageDisp, VisualPipeline::ImageDisplay);
	signalMapper->setMapping(addNumber, VisualPipeline::Number);
	signalMapper->setMapping(addGaussianBlur, VisualPipeline::GaussianBlur);

	QObject::connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(addBlock(int)));
	QObject::connect(compile, SIGNAL(triggered()), this, SLOT(compile()));

	menu.exec(event->globalPos());
}

void VisualPipeline::nodeAdded(QNEBlock *node)
{
	cout << "Node added: " << node->id() << endl;
	//nodes.
}

void VisualPipeline::nodeDeleted(int id, int type)
{
	cout << "Node deleted: " << id << endl;
}

void VisualPipeline::connectionMade(QNEBlock *node1, QNEBlock *node2)
{
	cout << "Connected " << node1->id() << " and " << node2->id() << endl;
}

void VisualPipeline::connectionDeleted(QNEBlock *node1, QNEBlock *node2)
{
	cout << "Disconnected " << node1->id() << " and " << node2->id() << endl;
}

void VisualPipeline::attributeSet(QNEBlock *node, int attribute, double value)
{

}
