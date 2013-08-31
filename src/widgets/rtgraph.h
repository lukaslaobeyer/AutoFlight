#ifndef RTGRAPH_H
#define RTGRAPH_H

#include <QWidget>
#include <QLabel>
#include <string>
#include <qcustomplot/qcustomplot.h>

class RTGraph : public QWidget
{
	Q_OBJECT

	public:
		explicit RTGraph(std::string unit, bool xAxisVisible = false, int xRange = 4, int yRange = 2, QWidget *parent = 0);

		void addPoint(float value);
	private:
		QCustomPlot *plot;
		QLabel *label;
		QVector<double> xData, yData;
		unsigned long long currentTime = 0;
		std::string _unit;
		unsigned int _yRange = 0, _xRange = 0;
		bool _started = false;
		unsigned long long startTime = 0, previousTime = 0;
};

#endif
