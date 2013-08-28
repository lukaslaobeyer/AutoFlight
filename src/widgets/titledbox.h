#ifndef TITLEDBOX_H
#define TITLEDBOX_H

#include <QWidget>

class TitledBox : public QWidget
{
	Q_OBJECT
	
	public:
		explicit TitledBox(const QString &title, QWidget *content, QWidget *parent = 0);
};

#endif
