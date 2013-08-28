#include "speed.h"
#include "titledbox.h"
#include "rotatableimageview.h"
#include <QtWidgets>
#include <math.h>

#define ORIENTATION_WIDGET_HEIGHT 215

Speed::Speed(QWidget *parent): QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	
	setStyleSheet("padding: 0;");
	
		QVBoxLayout *contentLayout = new QVBoxLayout();
		contentLayout->setSpacing(2);
		contentLayout->setMargin(0);
		contentLayout->setContentsMargins(0, 2, 0, 0);
		
		QWidget *content = new QWidget();
		content->setLayout(contentLayout);
		content->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
			
			QStackedLayout *speedometerLayout = new QStackedLayout(contentLayout);
			speedometerLayout->setSpacing(2);
			speedometerLayout->setMargin(0);
			speedometerLayout->setContentsMargins(0, 2, 0, 0);
			speedometerLayout->setStackingMode(QStackedLayout::StackAll);
			
			speedometerNeedle = new RotatableImageView(":/resources/speedometer-needle_160x160.png");
			speedometerNeedle->rotate(-129);
			speedometerLayout->addWidget(speedometerNeedle);
			
			QLabel *speedometer = new QLabel();
			speedometer->setPixmap(QPixmap(":/resources/speedometer_160x160.png"));
			speedometer->setMargin(2);
			speedometerLayout->addWidget(speedometer);
			
			speedometerLabel = new QLabel("N/A");
			speedometerLabel->setAlignment(Qt::AlignCenter);
			speedometerLabel->setStyleSheet("font: 13pt; font-weight: bold;");
			contentLayout->addWidget(speedometerLabel);
		
		TitledBox *box = new TitledBox(tr("Speed"), content);
		layout->addWidget(box);
		
	QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect();
	dropShadow->setBlurRadius(6);
	dropShadow->setColor(QColor(0, 0, 0));
	dropShadow->setOffset(0, 0);
	
	setGraphicsEffect(dropShadow);
	
	setLayout(layout);
}

void Speed::navdataAvailable(AFNavdata *nd)
{
	float speed = sqrt(nd->vx*nd->vx + nd->vy*nd->vy);
	speedometerNeedle->rotate((speed * 51.6f) - 129);
	speedometerLabel->setText(QString::number(speed, 'f', 2) + " m/s");
}
