#ifndef ALTITUDE_H
#define ALTITUDE_H

#include "../ardrone/navdata/qnavdatalistener.h"
#include "rtgraph.h"
#include <QWidget>

class Altitude : public QWidget, public QNavdataListener
{
	Q_OBJECT
	
	public:
		explicit Altitude(QWidget *parent = 0);
	public Q_SLOTS:
		void navdataAvailable(AFNavdata *nd);
	private:
		RTGraph *graph;
};

#endif
