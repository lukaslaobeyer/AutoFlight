#include "videodisplay.h"
#include <cmath>

VideoDisplay::VideoDisplay(QWidget *parent) : QGLWidget(parent)
{

}

void VideoDisplay::setCurrentFrame(const QImage &img)
{
	_connectionLost = false;
	_img = img;
	Q_EMIT update();
}

void VideoDisplay::navdataAvailable(AFNavdata *nd)
{
	yaw = nd->psi;
	pitch = nd->theta;
	roll = nd->phi;

	altitude = nd->altitude;
	charge = nd->vbatpercentage;
	speed = sqrt(nd->vx * nd->vx + nd->vy * nd->vy);
}

void VideoDisplay::controllerInputAvailable(ControllerInput *in)
{
	setInputData(in->roll, in->pitch, in->altitude, in->yaw);
}

void VideoDisplay::setInputData(double prollInput, double ppitchInput, double paltitudeInput, double pyawInput)
{
	rollInput = prollInput;
	pitchInput = ppitchInput;
	altitudeInput = paltitudeInput;
	yawInput = pyawInput;
}

void VideoDisplay::setNavdata(double pyaw, double ppitch, double proll, double paltitude, double pcharge, double pspeed)
{
	yaw = pyaw;
	pitch = ppitch;
	roll = proll;
	altitude = paltitude;
	charge = pcharge;
	speed = pspeed;
}

void VideoDisplay::setMaximized(bool maximize)
{
	_maximize = maximize;
	Q_EMIT update();
}

void VideoDisplay::showHUD(bool show)
{
	_hud = show;
	Q_EMIT update();
}

void VideoDisplay::connectionLost()
{
	_connectionLost = true;
	Q_EMIT update();
}

void VideoDisplay::paintEvent(QPaintEvent *)
{
	QPainter p(this);

    //Set the painter to use a smooth scaling algorithm.
    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

    QRect imgRect;
    imgRect.setX(rect().width() / 2 - _img.width() / 2);
    imgRect.setY(rect().height() / 2 -_img.height() / 2);
    imgRect.setWidth(_img.width());
    imgRect.setHeight(_img.height());

    if(_maximize)
    {
    	// Calculate the dimensions to fit the whole widget maintaining aspect ratio
    	int newheight = height();
		int newwidth = (int)(height() * ((double)imgRect.width() / (double)imgRect.height()));
		if(newwidth >= width()) {
			newwidth = width();
			newheight = (int)(width() * ((double)imgRect.height() / (double)imgRect.width()));
		}

		int newX = width()/2-newwidth/2;
		int newY = height()/2-newheight/2;

		imgRect.setX(newX);
		imgRect.setY(newY);
		imgRect.setWidth(newwidth);
		imgRect.setHeight(newheight);
    }

    p.drawImage(imgRect, _img);

    if(_hud)
    {
    	// Draw the head-up display

    	static QBrush normalBrush(QColor::fromRgb(0, 255, 0));
    	static QPen normalPen(normalBrush, 1);
    	static QPen thickPen(normalBrush, 2);

    	double screenwidth = imgRect.width();
    	double screenheight = imgRect.height();
    	int offsetX = imgRect.x();
    	int offsetY = imgRect.y();

    	p.translate(offsetX, offsetY);

    	p.setFont(QFont("Sans Serif", 13));
    	p.setPen(normalPen);

		int border = 20;

		// Draws the CENTER thing

		p.drawLine(screenwidth/2 - 50, screenheight/2 - 3, screenwidth/2 - 8, screenheight/2 - 3);
		p.drawLine(screenwidth/2 + 50, screenheight/2 - 3, screenwidth/2 + 8, screenheight/2 - 3);

		p.drawLine(screenwidth/2 - 50, screenheight/2 + 3, screenwidth/2 - 8, screenheight/2 + 3);
		p.drawLine(screenwidth/2 + 50, screenheight/2 + 3, screenwidth/2 + 8, screenheight/2 + 3);

		p.drawLine(screenwidth/2 - 50, screenheight/2 + 3, screenwidth/2 - 50, screenheight/2 - 3);
		p.drawLine(screenwidth/2 + 50, screenheight/2 + 3, screenwidth/2 + 50, screenheight/2 - 3);

		p.drawLine(screenwidth/2 - 8, screenheight/2 + 3, screenwidth/2, screenheight / 2 + 11);
		p.drawLine(screenwidth/2 + 8, screenheight/2 + 3, screenwidth/2, screenheight / 2 + 11);

		p.drawLine(screenwidth/2 - 8, screenheight/2 - 3, screenwidth/2, screenheight / 2 + 5);
		p.drawLine(screenwidth/2 + 8, screenheight/2 - 3, screenwidth/2, screenheight / 2 + 5);

		// Draws the PITCH, ROLL and YAW indicator

		int degree_per_step = 10;
		int ladder_width = 200;
		int roll_width = screenwidth - border*16;
		int viewAngleVert = 40; // For AR.Drone 2.0
		int viewAngle = 93; // For AR.Drone 2.0
		int tickSize = 25;
		int degree_per_tick = 15;
		float ladder_diff = screenheight / (viewAngleVert / degree_per_step);
		float yaw_diff = screenwidth / (viewAngle / degree_per_tick);

		p.setPen(normalPen);

		p.translate(screenwidth/2, screenheight/2);
		p.rotate(-roll);
		p.translate(-screenwidth/2, -screenheight/2);


		// The ROLL indicator
		p.setPen(thickPen);
		p.drawLine(screenwidth/2-roll_width/2, (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2), screenwidth/2+roll_width/2, (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2));
		p.setPen(normalPen);


		// The YAW indicator
		int yawmax = (360+degree_per_tick) / degree_per_tick;

		p.setFont(QFont("Sans Serif", 10));
		for(int i = 0; i <= yawmax; i++)
		{
			if(((i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) <= screenwidth/2 + roll_width/2) && ((i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) >= screenwidth/2 - roll_width/2))
			{
				QString value;
				value.sprintf("%03d", (int) to360Format(i*degree_per_tick, true));

				p.drawLine((int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2), (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2) - tickSize/2, (int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2), (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2)+tickSize/2);
				p.drawText((int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) - 13, (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2) - tickSize/2 - 5, value);
			}
		}

		for(int i = -1; i >= -yawmax; i--)
		{
			if(((i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) <= screenwidth/2 + roll_width/2) && ((i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) >= screenwidth/2 - roll_width/2))
			{
				QString value;
				value.sprintf("%03d", (int) to360Format(i*degree_per_tick, true));

				p.drawLine((int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2), (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2) - tickSize/2, (int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2), (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2)+tickSize/2);
				p.drawText((int) (i*yaw_diff + (yaw_diff/degree_per_tick)*-yaw + screenwidth/2) - 13, (int) ((ladder_diff/degree_per_step)*pitch + screenheight/2) - tickSize/2 - 5, value);
			}
		}
		p.setFont(QFont("Sans Serif", 13));

		// The PITCH indicator
		int pitchmax = 100 / degree_per_step;

		for(int i = 1; i <= pitchmax; i++)
		{
			p.drawLine(screenwidth/2-ladder_width/2, (int) (i*ladder_diff + (ladder_diff/degree_per_step)*pitch + screenheight/2), screenwidth/2+ladder_width/2, (int) (i*ladder_diff + (ladder_diff/degree_per_step)*pitch + screenheight/2));
			p.drawText(screenwidth/2-ladder_width/2-30, i*ladder_diff + (ladder_diff/degree_per_step)*pitch + 8 + screenheight/2, QString::number(i*degree_per_step));
		}

		for(int i = -1; i >= -pitchmax; i--)
		{
			p.drawLine(screenwidth/2-ladder_width/2, (int) (i*ladder_diff + (ladder_diff/degree_per_step)*pitch + screenheight/2), screenwidth/2+ladder_width/2, (int) (i*ladder_diff + (ladder_diff/degree_per_step)*pitch + screenheight/2));
			p.drawText(screenwidth/2-ladder_width/2-30, i*ladder_diff + (ladder_diff/degree_per_step)*pitch + 8 + screenheight/2, QString::number(i*degree_per_step));
		}

		p.translate(screenwidth/2, screenheight/2);
		p.rotate(roll);
		p.translate(-screenwidth/2, -screenheight/2);

		// Draw the ALTITUDE ladder

		double alt_diff = 80; // Pixels between longer green lines

		double units_per_step = 40;

		double altimeter_width = 50;

		p.setPen(thickPen);

		p.drawLine(screenwidth - border - altimeter_width, border, screenwidth - border - altimeter_width, screenheight - border);
		p.drawLine(screenwidth - border - altimeter_width, border, screenwidth - border, border);
		p.drawLine(screenwidth - border - altimeter_width, screenheight - border, screenwidth - border, screenheight - border);

		p.setPen(normalPen);

		int small_ticks = 5;

		for(int i = 0; i < (screenheight)/2; i++)
		{
			if(((i*alt_diff - (alt_diff/units_per_step)*altitude + screenheight/2) < (screenheight - border)) && ((i*alt_diff - (alt_diff/units_per_step)*altitude + screenheight/2) > border))
			{
				p.drawLine(screenwidth - border - altimeter_width, (int) (-i*alt_diff - (alt_diff/units_per_step)*-altitude + screenheight/2), screenwidth - border, (int) (-i*alt_diff - (alt_diff/units_per_step)*-altitude + screenheight/2));

				for(int j = 0; j < small_ticks; j++)
				{
					if(i*alt_diff - (alt_diff/units_per_step)*altitude + screenheight/2 + (alt_diff/small_ticks)*j < (screenheight - border))
					{
						p.drawLine(screenwidth - border - altimeter_width, (int) (-i*alt_diff - (alt_diff/units_per_step)*-altitude + screenheight/2 - (alt_diff/small_ticks)*j), screenwidth - border - altimeter_width/2, (int) (-i*alt_diff - (alt_diff/units_per_step)*-altitude + screenheight/2 - (alt_diff/small_ticks)*j));
					}
				}

				p.drawText(screenwidth - altimeter_width - border - 35, -i*alt_diff - (alt_diff/units_per_step)*-altitude + 8 + screenheight/2, QString::number(i*units_per_step));
			}
		}

		p.setPen(thickPen);
		p.drawLine(screenwidth - border - altimeter_width, screenheight/2 - 5, screenwidth - border - altimeter_width + 15, screenheight / 2);
		p.drawLine(screenwidth - border - altimeter_width, screenheight/2 + 5, screenwidth - border - altimeter_width + 15, screenheight / 2);
		p.setPen(normalPen);

		// Draw the BATTERY indicator

		int batt_width = 125;

		if(charge < 25)
		{
			p.setPen(QColor::fromRgb(255, 166, 0));
			if(charge < 15)
			{
				p.setPen(QColor::fromRgb(255, 0, 0));
			}
		}

		p.setFont(QFont("Sans Serif", 11));
		p.drawRect(border, screenheight-border-20, batt_width, 20);
		p.fillRect(border, screenheight-border-20, (int)(batt_width*((float) (charge/100.0f))), 20, normalBrush);
		p.drawText(border, screenheight-border-25, QString::number(charge).append("% battery charge"));

		p.setPen(normalPen);

		// Draw the CONTROLLER INPUT indicators
		int input_width = 20;
		int size = screenheight - 250;

		p.setPen(normalPen);

		p.drawLine(border + input_width/2, screenheight/2 - size/2, border + input_width/2, screenheight/2 + size/2);
		p.drawLine(border, screenheight/2 - size/2, border + input_width, screenheight/2 - size/2);
		p.drawLine(border, screenheight/2 + size/2, border + input_width, screenheight/2 + size/2);
		p.drawLine(border + 3, screenheight/2, border + input_width - 3, screenheight/2);

		drawTriangle(p, input_width/2, border + input_width/2, (int) (screenheight/2 - (altitudeInput * ((float) size/2))), 0);
		drawTriangle(p, input_width/2, border + input_width/2, (int) (screenheight/2 - (pitchInput * ((float) size/2))), 180);


		p.drawLine(screenwidth/2 - size/2, screenheight - (border + input_width/2), screenwidth/2 + size/2, screenheight - (border + input_width/2));
		p.drawLine(screenwidth/2 - size/2, screenheight - (border + input_width/2) - input_width/2, screenwidth/2 - size/2, screenheight - (border + input_width/2) + input_width/2);
		p.drawLine(screenwidth/2 + size/2, screenheight - (border + input_width/2) - input_width/2, screenwidth/2 + size/2, screenheight - (border + input_width/2) + input_width/2);
		p.drawLine(screenwidth/2, screenheight - (border + input_width/2) - input_width/2 + 3, screenwidth/2, screenheight - (border + input_width/2) + input_width/2 - 3);

		drawTriangle(p, input_width/2, (int) (screenwidth/2 + (rollInput * ((float) size/2))), screenheight - (border + input_width / 2), 90);
		drawTriangle(p, input_width/2, (int) (screenwidth/2 + (yawInput * ((float) size/2))), screenheight - (border + input_width / 2), -90);

		p.setPen(normalPen);

		// Draw the SPEEDOMETER
		/*int needleangle = (int) (speed*20-10);

		p.drawImage(speedometer, 0, 0, null);

		p.translate(25, 25);
		p.rotate(-needleangle);
		p.translate(-25, -25);
		p.drawImage(speedometer_needle, -60, -60, null);
		p.translate(25, 25);
		p.rotate(needleangle);
		p.translate(-25, -25);*/

		p.translate(-offsetX, -offsetY);
    }

    if(_connectionLost)
    {
    	p.setPen(Qt::transparent);
    	QBrush b = QColor::fromRgb(0, 0, 0, 175);
    	p.fillRect(imgRect, b);

    	p.setPen(Qt::white);

    	QFont f1("Sans Serif", 28, QFont::Bold);
    	QString text1 = tr("Connection to AR.Drone lost");
    	QFontMetrics fm1(f1);

    	QFont f2("Sans Serif", 14, QFont::Bold);
    	QString text2 = tr("Once you are in the AR.Drone's WiFi range, you can click 'Connect to AR.Drone' again.");
    	QFontMetrics fm2(f2);

    	p.setFont(f1);
    	p.drawText(QPoint(imgRect.width() / 2 - fm1.width(text1) / 2 + imgRect.x(), imgRect.height() / 2 - fm1.height() + imgRect.y()), text1);
    	p.setFont(f2);
    	p.drawText(QPoint(imgRect.width() / 2 - fm2.width(text2) / 2 + imgRect.x(), imgRect.height() / 2 + fm2.height() / 2 + imgRect.y()), text2);
    }
}

double VideoDisplay::to360Format(double value, bool clockwise)
{
	if(value < 0.0 && clockwise)
	{
		value = 360 + value;
	}
	else
	{
		if(value < 0.0 && !clockwise)
		{
			value = value * (-1);
		}
		else if(value > 0.0 && !clockwise)
		{
			value = 360 - value;
		}
	}

	return value;
}

void VideoDisplay::drawTriangle(QPainter &p, int size, int x, int y, int rotate)
{
	p.translate(x, y);
	p.rotate(rotate);

	p.drawLine(-size, -size/2, -size, size/2);
	p.drawLine(-size, -size/2, 0, 0);
	p.drawLine(-size, size/2, 0, 0);

	p.rotate(-rotate);
	p.translate(-x, -y);
}
