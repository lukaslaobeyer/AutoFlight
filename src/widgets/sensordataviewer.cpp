#include "sensordataviewer.h"
#include <QtWidgets>
#include <qcustomplot/qcustomplot.h>

using namespace std;

SensorDataViewer::SensorDataViewer(QWidget *parent) : QWidget(parent)
{
	QGridLayout *l = new QGridLayout();
	setLayout(l);

	QLabel *noDataLabel = new QLabel(tr("No data available"));
	noDataLabel->setStyleSheet("font-size: 28px;");
	l->addWidget(noDataLabel);
}

void SensorDataViewer::addDataFile(string path)
{

}

void SensorDataViewer::setDisplayTime(float time)
{

}

QWidget *SensorDataViewer::getPlot(QStackedLayout *completeWidget, QString title, QString unit, vector<QString> seriesNames)
{
	int xrange = 10;
	int yrange = 10;

	QStackedLayout *layout = new QStackedLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->setStackingMode(QStackedLayout::StackAll);

	QCustomPlot *plot = new QCustomPlot();
	plot->addGraph();
	plot->xAxis->setRange(0, xrange);
	plot->yAxis->setRange(0, yrange);
	plot->setBackground(Qt::transparent);
	plot->setMinimumWidth(300);
	plot->addGraph();
	plot->xAxis->setVisible(true);

	QFont l_font;
	l_font.setPointSize(13);
	l_font.setBold(true);

	QLabel *label = new QLabel();
	label->setText("N/A");
	label->setFont(l_font);
	label->setAlignment(Qt::AlignRight);
	label->setStyleSheet("padding: 20px;");

	layout->addWidget(label);
	layout->addWidget(plot);

	completeWidget = layout;
	return plot;
}

