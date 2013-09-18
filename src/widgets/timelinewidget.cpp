#include "timelinewidget.h"
#include <QPainter>
#include <QApplication>
#include <cmath>

using namespace std;

TimelineWidget::TimelineWidget(float time, QWidget *parent) : QWidget(parent)
{
	_time = time;
	setMinimumHeight(50);
	setMouseTracking(true);

	pictureIcon = QImage(":/resources/sessionviewer_picture.png", "png");
	videoIcon = QImage(":/resources/sessionviewer_video.png", "png");
	navdataIcon = QImage(":/resources/sessionviewer_navdata.png", "png");
	takeoffIcon = QImage(":/resources/sessionviewer_takeoff.png", "png");
	landIcon = QImage(":/resources/sessionviewer_land.png", "png");
	emergencyIcon = QImage(":/resources/sessionviewer_emergency.png", "png");
}

void TimelineWidget::setTime(float time)
{
	_time = time;
	Q_EMIT update();
}

void TimelineWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// Paint a nice background
	QLinearGradient bgGradient(0, 0, 0, height());
	bgGradient.setColorAt(0, QColor::fromRgb(100, 100, 100, 20));
	bgGradient.setColorAt(1, QColor::fromRgb(0, 0, 0, 0));
	QBrush bgBrush(bgGradient);
	painter.fillRect(0, 0, width(), height(), bgBrush);

	// Draw the events
	QLinearGradient navGradient(0, 0, 0, height());
	navGradient.setColorAt(0, QColor::fromRgb(0, 0, 120, 50));
	navGradient.setColorAt(1, QColor::fromRgb(0, 0, 0, 0));
	QBrush navdataRecordingBrush(navGradient);
	QLinearGradient vidGradient(0, 0, 0, height());
	vidGradient.setColorAt(0, QColor::fromRgb(0, 120, 0, 50));
	vidGradient.setColorAt(1, QColor::fromRgb(0, 0, 0, 0));
	QBrush videoRecordingBrush(vidGradient);

	float startedRecordingVidAt = -1.0f;
	float startedRecordingNavAt = -1.0f;

	for(RecordedEvent e : events)
	{
		float eventTime = ((float) e.getTimeFromStart()) / 1000.0f;

		if(e.getType() == "PictureTaken")
		{
			addMarker(pictureIcon, eventTime);
		}
		else if(e.getType() == "NavdataRecordingStart")
		{
			addMarker(navdataIcon, eventTime);
			startedRecordingNavAt = eventTime;
		}
		else if(e.getType() == "NavdataRecordingStop")
		{
			if(startedRecordingNavAt >= 0.0f)
			{
				painter.setPen(Qt::NoPen);
				painter.setBrush(navdataRecordingBrush);
				painter.fillRect(startedRecordingNavAt * ((float) width() / _time), 0, (eventTime - startedRecordingNavAt) * ((float) width() / _time), height(), navdataRecordingBrush);
				startedRecordingNavAt = -1.0f;
			}
		}
		else if(e.getType() == "VideoRecordingStart")
		{
			addMarker(videoIcon, eventTime);
			startedRecordingVidAt = eventTime;
		}
		else if(e.getType() == "VideoRecordingStop")
		{
			if(startedRecordingVidAt >= 0.0f)
			{
				painter.setPen(Qt::NoPen);
				painter.setBrush(videoRecordingBrush);
				painter.fillRect(startedRecordingVidAt * ((float) width() / _time), 0, (eventTime - startedRecordingVidAt) * ((float) width() / _time), height(), videoRecordingBrush);
				startedRecordingVidAt = -1.0f;
			}
		}
		else if(e.getType() == "TakeOff")
		{
			addMarker(takeoffIcon, eventTime);
		}
		else if(e.getType() == "Land")
		{
			addMarker(landIcon, eventTime);
		}
		else if(e.getType() == "Emergency")
		{
			addMarker(emergencyIcon, eventTime);
		}
	}

	// Draw the horizontal time indicator
	int secondsPerTick = 5;
	float tickSpacing = (float) width() / (_time / (float) secondsPerTick);
	int tickHeight = 6;
	int minorTicksPerTick = 5;
	QColor tickColor = QColor::fromRgb(0, 48, 104);
	QColor minorTickColor = QColor::fromRgb(112, 112, 112);
	QPen tickPen(tickColor);
	tickPen.setWidth(1);
	QPen minorTickPen(minorTickColor);
	minorTickPen.setWidth(1);

	for(int horizontalOffset = 0; horizontalOffset < width(); horizontalOffset += tickSpacing)
	{
		// Draw major marker
		painter.setPen(tickPen);
		painter.drawLine(horizontalOffset, height(), horizontalOffset, height() - tickHeight);

		// Draw minor markers
		painter.setPen(minorTickPen);
		for(int i = 1; i < minorTicksPerTick; i++)
		{
			painter.drawLine(horizontalOffset + (float) i * ((float) tickSpacing / (float) minorTicksPerTick), height(), horizontalOffset + (float) i * ((float) tickSpacing / (float) minorTicksPerTick), height() - (tickHeight / 2));
		}
	}

	// Draw the current time indicator
	QColor currTimeColor = QColor::fromRgb(255, 0, 0);
	QPen currTimePen(currTimeColor);
	currTimePen.setWidth(1);

	painter.setPen(currTimePen);
	painter.drawLine(_currentTime * ((float) width() / (float) _time), height(), _currentTime * ((float) width() / (float)_time), 0);

	// Draw the "current mouse position" line
	QColor mouseLineColor = QColor::fromRgb(255, 0, 0, 80);
	QPen mouseLinePen(mouseLineColor);
	mouseLinePen.setWidth(1);

	painter.setPen(mouseLinePen);
	painter.drawLine(_mouseX, height(), _mouseX, 0);
	painter.drawText(QPoint(_mouseX + 5, 20), QString("t = ").append(QString::number((float) _mouseX / ((float) width() / _time), 'f', 1)));

	// Draw the event markers
	drawMarkers(painter);
	drawTooltips(painter);
}

void TimelineWidget::mouseMoveEvent(QMouseEvent *e)
{
	_mouseX = e->pos().x();

	snapToMarkers(&_mouseX, 10);

	if(QApplication::mouseButtons() == Qt::LeftButton)
	{
		_currentTime = (float) e->pos().x() / ((float) width() / (float) _time);
		Q_EMIT newTimeSelected(_currentTime);
	}
	Q_EMIT update();
}

void TimelineWidget::mouseReleaseEvent(QMouseEvent *e)
{
	_mouseX = e->pos().x();

	snapToMarkers(&_mouseX, 10);

	_currentTime = (float) _mouseX / ((float) width() / (float) _time);

	if(e->pos().y() >= height() - pictureIcon.height())
	{
		// Clicked on a marker

		RecordedEvent *clicked = nullptr;

		// Find out which one:
		int closest = 1000000;

		for(RecordedEvent e : events)
		{
			int eventValue = (e.getTimeFromStart() / 1000.0f) * ((float) width() / _time);

			if(eventValue <= _mouseX + 12 && eventValue >= _mouseX - 12)
			{
				if(abs(eventValue - _mouseX) < closest)
				{
					clicked = &e;
				}
			}
		}

		if(clicked != nullptr)
		{
			Q_EMIT markerPressed(clicked);
		}
	}

	Q_EMIT newTimeSelected(_currentTime);
	Q_EMIT update();
}

void TimelineWidget::addEvent(RecordedEvent e)
{
	events.push_back(e);
	Q_EMIT update();
}

void TimelineWidget::removeEvents()
{
	events.clear();
	Q_EMIT update();
}

void TimelineWidget::addMarker(QImage i, float t)
{
	markers.push_back(i);
	markerTimes.push_back(t);
}

void TimelineWidget::drawMarkers(QPainter &painter)
{
	int i = 0;

	for(QImage marker : markers)
	{
		painter.drawImage(QPoint((float) markerTimes[i] * ((float) width() / _time) - marker.width() / 2.0f, height() - marker.height()), marker);
		i++;
	}

	markers.clear();
	markerTimes.clear();
}

void TimelineWidget::snapToMarkers(int *value, int snapThreshold)
{
	int closest = 1000000;

	for(RecordedEvent e : events)
	{
		int eventValue = (e.getTimeFromStart() / 1000.0f) * ((float) width() / _time);

		if(eventValue <= *value + snapThreshold && eventValue >= *value - snapThreshold)
		{
			if(abs(eventValue - *value) < closest)
			{
				closest = abs(eventValue - *value);
				*value = eventValue;
			}
		}
	}
}

void TimelineWidget::drawTooltips(QPainter &painter)
{
	QBrush labelBg(QColor::fromRgb(0, 0, 0, 200));
	QPen textPen(QColor::fromRgb(255, 255, 255));
	QFont font;
	font.setFamily(font.defaultFamily());
	QFontMetrics fmetrics(font);

	int closest = 1000000;

	for(RecordedEvent e : events)
	{
		int eventValue = (e.getTimeFromStart() / 1000.0f) * ((float) width() / _time);

		if(eventValue <= _mouseX + 12 && eventValue >= _mouseX - 12)
		{
			if(abs(eventValue - _mouseX) < closest)
			{
				QString label = "";

				if(e.getType() == "TakeOff")
				{
					label = tr("Take Off");
				}
				else if(e.getType() == "Land")
				{
					label = tr("Landing");
				}
				else if(e.getType() == "Emergency")
				{
					label = tr("Emergency");
				}
				else if(e.getType() == "VideoRecordingStart")
				{
					label = tr("Start Recording Video");
				}
				else if(e.getType() == "NavdataRecordingStart")
				{
					label = tr("Start Recording Navdata");
				}
				else if(e.getType() == "PictureTaken")
				{
					label = tr("Picture");
				}

				if(label.length() > 0)
				{
					int lblw = fmetrics.width(label) + 10;
					int lblh = fmetrics.height() + 6;

					painter.setPen(textPen);
					painter.setBrush(labelBg);

					painter.fillRect(_mouseX - lblw / 2, height() - lblh - 35, lblw, lblh, labelBg);
					painter.drawText(_mouseX - lblw / 2 + 5, height() - lblh/2 - 30, label);
				}
			}
		}
	}
}
