#include "battery.h"
#include "titledbox.h"
#include <QtWidgets>

#include <iostream>

#define ORIENTATION_WIDGET_HEIGHT 215

Battery::Battery(QWidget *parent): QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	
	setStyleSheet("padding: 0;");
	
		QVBoxLayout *contentLayout = new QVBoxLayout();
		contentLayout->setSpacing(2);
		contentLayout->setMargin(0);
		contentLayout->setContentsMargins(4, 4, 4, 4);
		
		QWidget *content = new QWidget();
		content->setLayout(contentLayout);
		
			batteryLevel = new QProgressBar();
			batteryLevel->setTextVisible(false);
			batteryLevel->setRange(0, 100);
			batteryLevel->setValue(0);
			batteryLevel->setOrientation(Qt::Vertical);
			batteryLevel->setMinimumWidth(50);
			batteryLevel->setStyleSheet("QProgressBar {border: 1px solid gray; border-radius: 3px; background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 #C9C9C9, stop: 1 #DBDBDB);} QProgressBar::chunk {background: red;}");
			contentLayout->addWidget(batteryLevel);
			
			batteryLevelLabel = new QLabel("N/A");
			batteryLevelLabel->setStyleSheet("font: 13pt; font-weight: bold;");
			batteryLevelLabel->setAlignment(Qt::AlignCenter);
			contentLayout->addWidget(batteryLevelLabel);
		
		TitledBox *box = new TitledBox(tr("Bat."), content);
		layout->addWidget(box);
		
	QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect();
	dropShadow->setBlurRadius(6);
	dropShadow->setColor(QColor(0, 0, 0));
	dropShadow->setOffset(0, 0);
	
	setGraphicsEffect(dropShadow);
	
	setLayout(layout);
}

void Battery::navdataAvailable(AFNavdata *nd)
{
	batteryLevelLabel->setText(QString::number(nd->vbatpercentage, 'f', 0) + "%");
	
	batteryLevel->setValue(nd->vbatpercentage);
	int hue1 = (int)(nd->vbatpercentage * 1.2f), saturation1 = 255, brightness1 = 190;
	int hue2 = hue1, saturation2 = 255, brightness2 = 150;
	batteryLevel->setStyleSheet("QProgressBar {border: 1px solid gray; border-radius: 3px; background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 #CCCCCC, stop: 1 #DBDBDB);} QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 hsv(" + QString::number(hue1) + ", " + QString::number(saturation1) + ", " + QString::number(brightness1) + "), stop: 1 hsv(" + QString::number(hue2) + ", " + QString::number(saturation2) + ", " + QString::number(brightness2) + "));}");
}
