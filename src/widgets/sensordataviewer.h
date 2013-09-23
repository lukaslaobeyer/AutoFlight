#ifndef SENSORDATAVIEWER_H
#define SENSORDATAVIEWER_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QStackedLayout>
#include <string>
#include <vector>

class SensorDataViewer : public QWidget
{
	Q_OBJECT

	public:
		SensorDataViewer(QWidget *parent = 0);

		void addDataFile(std::string path);
		void setDisplayTime(float time);
	private:
		QLabel *noDataLabel;

		QWidget *getPlot(QStackedLayout *completeWidget, QString title, QString unit, std::vector<QString> seriesNames);
};

#endif
