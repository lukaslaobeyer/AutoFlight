#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QGLWidget>

class VideoDisplay : public QGLWidget
{
	Q_OBJECT

	public:
		VideoDisplay(QWidget *parent = 0);

		void setCurrentFrame(const QImage &img);
		void setCurrentData(double yaw, double pitch, double roll, double altitude, double charge, double speed, double rollInput, double pitchInput, double altitudeInput, double yawInput);

		void setMaximized(bool maximize);
		void showHUD(bool show);
	protected:
		void paintEvent(QPaintEvent *);

	private:
		double to360Format(double number, bool clockwise);
		void drawTriangle(QPainter &p, int size, int x, int y, int rotate);

		QImage _img;
		bool _maximize = false;
		bool _hud = false

		double yaw = 0, pitch = 0, roll = 0, altitude = 0, charge = 0, speed = 0, rollInput = 0, pitchInput = 0, altitudeInput = 0, yawInput = 0;
};

#endif
