#ifndef SPEED_H
#define SPEED_H

#include "rotatableimageview.h"
#include "../ardrone/navdata/inavdatalistener.h"
#include <QWidget>
#include <QLabel>

class Speed : public QWidget, public INavdataListener
{
	Q_OBJECT
	
	public:
		explicit Speed(QWidget *parent = 0);
	public Q_SLOTS:
		void navdataAvailable(AFNavdata *navdata);
	private:
		RotatableImageView *speedometerNeedle;
		QLabel *speedometerLabel;
};

#endif
