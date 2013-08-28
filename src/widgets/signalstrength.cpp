#include "signalstrength.h"
#include "titledbox.h"
#include <QtWidgets>

#define ORIENTATION_WIDGET_HEIGHT 215

SignalStrength::SignalStrength(QWidget *parent): QWidget(parent)
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
		
			signalStrength = new QProgressBar();
			signalStrength->setTextVisible(false);
			signalStrength->setRange(0, 100);
			signalStrength->setValue(0);
			signalStrength->setOrientation(Qt::Vertical);
			signalStrength->setMinimumWidth(50);
			signalStrength->setStyleSheet("QProgressBar {border: 1px solid gray; border-radius: 3px; background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 #C9C9C9, stop: 1 #DBDBDB);} QProgressBar::chunk {background: red;}");
			contentLayout->addWidget(signalStrength);
			
			signalStrengthLabel = new QLabel("N/A");
			signalStrengthLabel->setStyleSheet("font: 13pt; font-weight: bold;");
			signalStrengthLabel->setAlignment(Qt::AlignCenter);
			contentLayout->addWidget(signalStrengthLabel);
		
		TitledBox *box = new TitledBox(tr("WiFi"), content);
		layout->addWidget(box);
		
	QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect();
	dropShadow->setBlurRadius(6);
	dropShadow->setColor(QColor(0, 0, 0));
	dropShadow->setOffset(0, 0);
	
	setGraphicsEffect(dropShadow);
	
	setLayout(layout);
}

void SignalStrength::navdataAvailable(AFNavdata *nd)
{
	signalStrengthLabel->setText(QString::number(nd->wifipercentage, 'f', 0) + "%");
	
	signalStrength->setValue(nd->wifipercentage);
	int hue1 = (int)(nd->wifipercentage * 1.2f), saturation1 = 255, brightness1 = 190;
	int hue2 = hue1, saturation2 = 255, brightness2 = 150;
	signalStrength->setStyleSheet("QProgressBar {border: 1px solid gray; border-radius: 3px; background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 #CCCCCC, stop: 1 #DBDBDB);} QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 hsv(" + QString::number(hue1) + ", " + QString::number(saturation1) + ", " + QString::number(brightness1) + "), stop: 1 hsv(" + QString::number(hue2) + ", " + QString::number(saturation2) + ", " + QString::number(brightness2) + "));}");
}
