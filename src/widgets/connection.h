#ifndef CONNECTION_H
#define CONNECTION_H

#include <QWidget>
#include <QPushButton>
#include "../ardrone/navdata/inavdatalistener.h"

class Connection : public QWidget, public INavdataListener
{
	Q_OBJECT
	
	public:
		explicit Connection(QWidget *parent = 0);
		void navdataAvailable(AFNavdata *navdata);
	private:
		QPushButton *connectDrone;
		QPushButton *connectArduino;
	Q_SIGNALS:
		void droneConnectionRequested();
		void arduinoConnectionRequested();
};

#endif
