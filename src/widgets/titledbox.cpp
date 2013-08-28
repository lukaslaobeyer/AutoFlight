#include "titledbox.h"
#include <QtWidgets>

TitledBox::TitledBox(const QString &title, QWidget *content, QWidget *parent): QWidget(parent)
{
	QGridLayout *layout = new QGridLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	
	setLayout(layout);
	
	QLabel *titlelbl = new QLabel(title);
	titlelbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	titlelbl->setStyleSheet("font-size: 12pt; padding: 5px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #B3B3B3, stop: 1 #9C9C9C);");
	titlelbl->setAlignment(Qt::AlignCenter);
	layout->addWidget(titlelbl, 0, 0);
	
	content->setObjectName("titledbox_content");
	content->setStyleSheet("QWidget#titledbox_content {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #DEDEDE, stop: 1 #FFFFFF);}");
	layout->addWidget(content, 1, 0);

}
