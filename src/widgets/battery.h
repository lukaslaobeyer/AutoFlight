#ifndef BATTERY_H
#define BATTERY_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include "../ardrone/navdata/qnavdatalistener.h"

class Battery : public QWidget, public QNavdataListener
{
	Q_OBJECT
	
	public:
		explicit Battery(QWidget *parent = 0);
	public Q_SLOTS:
		void navdataAvailable(AFNavdata *navdata);
	private:
		QProgressBar *batteryLevel;
		QLabel *batteryLevelLabel;
};

#endif
