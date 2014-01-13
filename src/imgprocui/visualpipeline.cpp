#include "visualpipeline.h"

#include <QtWidgets>

VisualPipeline::VisualPipeline(QWidget *parent) : QGraphicsView(parent)
{
	s = new QGraphicsScene();
	setScene(s);
	setRenderHint(QPainter::Antialiasing);

	s->setBackgroundBrush(QBrush(QPixmap(":/resources/visualpipeline_bg.png")));

	nodesEditor = new QNodesEditor(this);
	nodesEditor->install(s);
}

void VisualPipeline::compile()
{

}

void VisualPipeline::addBlock(BlockType type, int x, int y)
{
	static int id_count = 0;

	if(type == ImageIn)
	{
		QNEBlock *imgIn = new QNEBlock;
		s->addItem(imgIn);
		imgIn->addPort(tr("Image In"), 0, QNEPort::NamePort);
		imgIn->addPort(" ", 0, QNEPort::TypePort);
		imgIn->addOutputPort("out [img]");
		imgIn->setPos(x, y);

		imgIn->setID(id_count++);
		imgIn->setType(ImageIn);
	}
	else if(type == ImageDisplay)
	{
		QNEBlock *imgDisp = new QNEBlock;
		s->addItem(imgDisp);
		imgDisp->addPort(tr("Display Image"), 0, QNEPort::NamePort);
		imgDisp->addPort(" ", 0, QNEPort::TypePort);
		imgDisp->addInputPort("in [img]");
		imgDisp->setPos(x, y);

		imgDisp->setID(id_count++);
		imgDisp->setType(ImageDisplay);
	}
	else if(type == GaussianBlur)
	{
		QNEBlock *gaussianBlur = new QNEBlock;
		s->addItem(gaussianBlur);
		gaussianBlur->addPort(tr("Gaussian Blur"), 0, QNEPort::NamePort);
		gaussianBlur->addPort(" ", 0, QNEPort::TypePort);
		gaussianBlur->addInputPort("in [img]");
		gaussianBlur->addInputPort("kernel width [uint]");
		gaussianBlur->addInputPort("kernel height [uint]");
		gaussianBlur->addPort("", 0, QNEPort::TypePort);
		gaussianBlur->addOutputPort("out [img]");
		gaussianBlur->setPos(x, y);

		gaussianBlur->setID(id_count++);
		gaussianBlur->setType(GaussianBlur);
	}
	else if(type == Number)
	{
		QNENumberBlock *number = new QNENumberBlock;
		s->addItem(number);
		number->initialize();
		number->setPos(x, y);

		number->setID(id_count++);
		number->setType(Number);
	}
}

void VisualPipeline::addBlock(int type)
{
	addBlock((VisualPipeline::BlockType) type, addNodeAtX, addNodeAtY);
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
