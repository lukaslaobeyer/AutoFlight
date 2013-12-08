#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include "../ardrone/navdata/navdatakeys.h"
#include "../ardrone/input/controllerinput.h"
#include <QGLWidget>

class VideoDisplay : public QGLWidget
{
	Q_OBJECT

	public:
		VideoDisplay(QWidget *parent = 0);

		void setCurrentFrame(const QImage &img);
		void setInputData(double prollInput, double ppitchInput, double paltitudeInput, double pyawInput);
		void setNavdata(double yaw, double pitch, double roll, double altitude, double charge, double speed);

		void setMaximized(bool maximize);
		void showHUD(bool show);
		void connectionLost();

	public Q_SLOTS:
		void navdataAvailable(AFNavdata *nd);
		void controllerInputAvailable(ControllerInput *in);

	protected:
		void paintEvent(QPaintEvent *e);

	private:
		double to360Format(double number, bool clockwise);
		void drawTriangle(QPainter &p, int size, int x, int y, int rotate);

		QImage _img;
		bool _maximize = false;
		bool _hud = false;
		bool _connectionLost = false;

		double yaw = 0, pitch = 0, roll = 0, altitude = 0, charge = 0, speed = 0, rollInput = 0, pitchInput = 0, altitudeInput = 0, yawInput = 0;
};

#endif
