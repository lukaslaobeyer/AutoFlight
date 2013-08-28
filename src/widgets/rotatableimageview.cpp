#include "rotatableimageview.h"
#include <QtWidgets>

RotatableImageView::RotatableImageView(QString file, QWidget *parent): QWidget(parent)
{
	QGridLayout *layout = new QGridLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	QPixmap imagePixmap(file);
	
	QGraphicsScene *imageScene = new QGraphicsScene();
				
	QGraphicsPixmapItem *imagePixmapItem = new QGraphicsPixmapItem(imagePixmap);
	imagePixmapItem->setTransformationMode(Qt::SmoothTransformation);
	imageScene->addItem(imagePixmapItem);
	
	_width = imagePixmap.size().width();
	_height = imagePixmap.size().height();

	imageView = new QGraphicsView(imageScene);
	imageView->setRenderHints(QPainter::Antialiasing);
	imageView->setSceneRect(0, 0, _width, _height);
	imageView->setFixedSize(imagePixmap.size());
	imageView->setStyleSheet("background: transparent; border: none;");
	imageView->setDisabled(true);
	imageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	imageView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	layout->addWidget(imageView);
}

void RotatableImageView::rotate(int degrees)
{
	QTransform transform;
	transform.rotate(degrees);
	
	if(imageView) {
		imageView->setTransform(transform);
	}
}
