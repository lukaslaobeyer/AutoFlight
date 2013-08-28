#ifndef SIGNALSTRENGTH_H
#define SIGNALSTRENGTH_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include "../ardrone/navdata/qnavdatalistener.h"

class SignalStrength : public QWidget, public QNavdataListener
{
	Q_OBJECT
	
	public:
		explicit SignalStrength(QWidget *parent = 0);
	public Q_SLOTS:
		void navdataAvailable(AFNavdata *navdata);
	private:
		QProgressBar *signalStrength;
		QLabel *signalStrengthLabel;
};

#endif
