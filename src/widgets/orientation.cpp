#include "orientation.h"
#include "titledbox.h"
#include <QtWidgets>

#define ORIENTATION_WIDGET_HEIGHT 215

Orientation::Orientation(QWidget *parent): QWidget(parent)
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
		
			/* Yaw indicator */
			QVBoxLayout *yawContentLayout = new QVBoxLayout();
			yawContentLayout->setSpacing(0);
			yawContentLayout->setMargin(0);
		
			QWidget *yawContent = new QWidget();
			yawContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			yawContent->setLayout(yawContentLayout);
				
				yawLabel = new QLabel("N/A");
				yawLabel->setAlignment(Qt::AlignCenter);
				yawLabel->setMargin(4);
				yawLabel->setStyleSheet("font: 13pt; font-weight: bold;");
				yawContentLayout->addWidget(yawLabel);
				
				yawIndicator = new RotatableImageView(":/resources/indicator_gyroYAW_128x128.png");
				yawContentLayout->addWidget(yawIndicator);
		
			TitledBox *yaw = new TitledBox(tr("Yaw"), yawContent);
			
			contentLayout->addWidget(yaw);
		
			/* Pitch indicator */
			QVBoxLayout *pitchContentLayout = new QVBoxLayout();
			pitchContentLayout->setSpacing(0);
			pitchContentLayout->setMargin(0);
			
			QWidget *pitchContent = new QWidget();
			pitchContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			pitchContent->setLayout(pitchContentLayout);
			
				pitchLabel = new QLabel("N/A");
				pitchLabel->setAlignment(Qt::AlignCenter);
				pitchLabel->setMargin(4);
				pitchLabel->setStyleSheet("font: 13pt; font-weight: bold;");
				pitchContentLayout->addWidget(pitchLabel);
				
				pitchIndicator = new RotatableImageView(":/resources/indicator_gyroPITCH_128x128.png");
				pitchContentLayout->addWidget(pitchIndicator);
		
			TitledBox *pitch = new TitledBox(tr("Pitch"), pitchContent);
			
			contentLayout->addWidget(pitch);
		
			/* Roll indicator */
			QVBoxLayout *rollContentLayout = new QVBoxLayout();
			rollContentLayout->setSpacing(0);
			rollContentLayout->setMargin(0);
		
			QWidget *rollContent = new QWidget();
			rollContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			rollContent->setLayout(rollContentLayout);
				
				rollLabel = new QLabel("N/A");
				rollLabel->setAlignment(Qt::AlignCenter);
				rollLabel->setMargin(4);
				rollLabel->setStyleSheet("font: 13pt; font-weight: bold;");
				rollContentLayout->addWidget(rollLabel);
				
				rollIndicator = new RotatableImageView(":/resources/indicator_gyroROLL_128x128.png");
				rollContentLayout->addWidget(rollIndicator);
		
			TitledBox *roll = new TitledBox(tr("Roll"), rollContent);
			
			contentLayout->addWidget(roll);
		
		TitledBox *box = new TitledBox(tr("Orientation"), content);
		layout->addWidget(box);
		
	QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect();
	dropShadow->setBlurRadius(6);
	dropShadow->setColor(QColor(0, 0, 0));
	dropShadow->setOffset(0, 0);
	
	setGraphicsEffect(dropShadow);
	
	setLayout(layout);
}

void Orientation::navdataAvailable(AFNavdata *nd)
{
	yawLabel->setText(QString::number(nd->psi, 'f', 0) + "\u00B0");
	pitchLabel->setText(QString::number(nd->theta, 'f', 0) + "\u00B0");
	rollLabel->setText(QString::number(nd->phi, 'f', 0) + "\u00B0");
	
	yawIndicator->rotate((int) nd->psi);
	pitchIndicator->rotate((int) nd->theta);
	rollIndicator->rotate((int) nd->phi);
}
