#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <string>
#include "../tools/sessionreader.h"

class TimelineWidget : public QWidget
{
	Q_OBJECT

	public:
		TimelineWidget(float time, QWidget *parent = 0);

		void setTime(float time);

		void addEvent(RecordedEvent e);
		void removeEvents();

	protected:
		void paintEvent(QPaintEvent *event);
		void mouseMoveEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);

	private:
		float _time; // Time (in seconds) to be represented by this timeline
		int _mouseX = 0;
		float _currentTime = 0;

		QImage pictureIcon, videoIcon, navdataIcon;

		std::vector<RecordedEvent> events;

		std::vector<QImage> markers;
		std::vector<float> markerTimes;

		void snapToMarkers(int *value, int snapThreshold);

		void addMarker(QImage marker, float time);
		void drawMarkers(QPainter &painter);
};

#endif
