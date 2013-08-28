#include "rtgraph.h"
#include <QtWidgets>
#include <boost/chrono.hpp>

using namespace std;

RTGraph::RTGraph(string unit, bool xAxisVisible, int xRange, int yRange, QWidget *parent): QWidget(parent)
{
	_unit = unit;
	_xRange = xRange;
	_yRange = yRange;

	QStackedLayout *layout = new QStackedLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->setStackingMode(QStackedLayout::StackAll);

	setStyleSheet("padding: 0;");

	plot = new QCustomPlot();
	plot->addGraph();
	plot->xAxis->setRange(0, _xRange);
	plot->yAxis->setRange(0, _yRange);
	plot->setBackground(Qt::transparent);
	plot->setMinimumWidth(300);
	plot->addGraph();
	plot->graph(0)->setData(xData, yData);
	plot->xAxis->setVisible(xAxisVisible);

	QFont l_font;
	l_font.setPointSize(13);
	l_font.setBold(true);

	label = new QLabel();
	label->setText("N/A");
	label->setFont(l_font);
	label->setAlignment(Qt::AlignRight);
	label->setStyleSheet("padding: 20px;");

	layout->addWidget(label);
	layout->addWidget(plot);

	setLayout(layout);
}

void RTGraph::addPoint(float point)
{
	if(!_started)
	{
		_started = true;
		startTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
	}

	currentTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count() - startTime;

	if(point > _yRange)
	{
		plot->yAxis->setRange(0, point + 2);
	}

	if(currentTime / 1000 >= _xRange)
	{
		double growth = (double)(currentTime - previousTime) / 1000.0;
		plot->xAxis->setRange(plot->xAxis->range().lower + growth, plot->xAxis->range().upper + growth);
	}

	xData.append((double) (currentTime / 1000.0));
	yData.append(point);

	plot->graph(0)->setData(xData, yData);
	plot->replot();

	label->setText(QString::number(point, 'f', 2).append(" ").append(_unit.c_str()));

	previousTime = currentTime;
}
