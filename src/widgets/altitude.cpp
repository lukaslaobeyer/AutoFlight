#include "altitude.h"
#include "titledbox.h"
#include <QtWidgets>

#include <iostream>

#define ORIENTATION_WIDGET_HEIGHT 215

Altitude::Altitude(QWidget *parent): QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	
	setStyleSheet("padding: 0;");
	
		QHBoxLayout *contentLayout = new QHBoxLayout();
		contentLayout->setSpacing(2);
		contentLayout->setMargin(0);
		contentLayout->setContentsMargins(0, 2, 0, 0);
		
		QWidget *content = new QWidget();
		content->setLayout(contentLayout);
			
			graph = new RTGraph("m");
			contentLayout->addWidget(graph);

		TitledBox *box = new TitledBox(tr("Altitude"), content);
		layout->addWidget(box);
		
	QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect();
	dropShadow->setBlurRadius(6);
	dropShadow->setColor(QColor(0, 0, 0));
	dropShadow->setOffset(0, 0);
	
	setGraphicsEffect(dropShadow);
	
	setLayout(layout);
}

void Altitude::navdataAvailable(AFNavdata *nd)
{
	graph->addPoint(nd->altitude);
}
