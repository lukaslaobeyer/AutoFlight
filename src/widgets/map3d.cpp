#include "map3d.h"

#include <QtWidgets>
#include <iostream>
#include <QDateTime>

using namespace std;

Map3D::Map3D(QWidget *parent) : QGLWidget(parent)
{
	setWindowTitle(tr("Map View"));
	setAutoFillBackground(false);
	setFocusPolicy(Qt::StrongFocus);
}

Map3D::~Map3D()
{
	for(glm::vec3 *vec : _path)
	{
		delete vec;
	}

	_path.clear();
}

QSize Map3D::minimumSizeHint() const
{
	return QSize(600, 400);
}

QSize Map3D::sizeHint() const
{
    return QSize(700, 500);
}

long long Map3D::deltaT()
{
	bool zero = false;

	if(lastT == 0)
	{
		zero = true;
	}

	long long currentT = QDateTime::currentMSecsSinceEpoch();
	long long deltaT = currentT - lastT;
	lastT = currentT;

	if(zero)
	{
		return 0;
	}
	else
	{
		return deltaT;
	}
}

void Map3D::requestUpdateGL()
{
	long long currentT = QDateTime::currentMSecsSinceEpoch();
	long long deltaT = currentT - lastGLUpdate;

	if(deltaT >= 33) // Do not update more often than every 33 ms (ca. 30 fps)
	{
		updateGL();
		lastGLUpdate = currentT;
	}
}

void Map3D::navdataAvailable(AFNavdata *nd)
{
	droneYaw = nd->psi;
	droneRoll = nd->phi;
	dronePitch = nd->theta;
	droneAlt = nd->altitude;

	// Calculate position using speed and direction data
	int dT = deltaT();
	float translateX = nd->vx * 0.001 * dT;
	float translateY = nd->vy * 0.001 * dT;
	dronePosX = dronePosX + cos(M_PI/180 * (droneYaw-90)) * translateY + cos(M_PI/180 * (90-(droneYaw-90))) * translateX;
	dronePosY = dronePosY + sin(M_PI/180 * (droneYaw-90)) * translateY + sin(M_PI/180 * (90-(droneYaw-90))) * translateX;

	requestUpdateGL();
}

void Map3D::setDroneAttitude(float yaw, float pitch, float roll, float altitude)
{
	droneYaw = yaw;
	dronePitch = pitch;
	droneRoll = roll;
	droneAlt = altitude;
	requestUpdateGL();
}

void Map3D::setDronePosition(float x, float y)
{
	dronePosX = x;
	dronePosY = y;
	requestUpdateGL();
}

void Map3D::setDronePath(vector<glm::vec3 *> *path)
{
	// Free memory of existing path
	for(glm::vec3 *vec : _path)
	{
		delete vec;
	}

	_path.clear();

	// Hard copy the vector
	for(glm::vec3 *vec : *path)
	{
		_path.push_back(new glm::vec3(vec->x, vec->y, vec->z));
	}

	_sessionViewer = true;

	requestUpdateGL();
}

void Map3D::initializeGL()
{
	// Antialiasing for smooth lines and edges
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION); // Select the projection matrix
	glLoadIdentity(); // Reset the projection matrix
	glMatrixMode(GL_MODELVIEW); // Select the modelview matrix
	glLoadIdentity(); // Reset the modelview matrix

	glShadeModel(GL_SMOOTH); // Enables smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background
	glClearDepth(1.0f); // Depth buffer setup
	glEnable(GL_DEPTH_TEST); // Enables depth testing
	glDepthFunc(GL_LEQUAL); // The type of depth test to do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

	// Initialize lights
	static GLfloat lightPosition[4] = {0.0f, 0.0f, 0.0f, -1.0f};
	static GLfloat whiteLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat lModelAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};

	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f); // sets shininess
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); // sets light position
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight); // sets specular light to white
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight); // sets diffuse light to white
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lModelAmbient); // global ambient lights
	glEnable(GL_LIGHTING); // enables lighting
	glEnable(GL_LIGHT0); // enables light0
	glEnable(GL_COLOR_MATERIAL); // enables OpenGL to use glColor3f to define material color
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // tell opengl glColor3f effects the ambient and diffuse properties of material
}

void Map3D::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspective(45, ((float) width / (float) height), 0.01f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

void Map3D::paintGL()
{
	// Disabled 2D stuff because of performance impact

	// Needed to revert configuration made to be able to use QPainter
	//initializeGL();
	//resizeGL(width(), height());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// User defined perspective
	glTranslatef(offsetX, offsetY, zoom);
	glRotatef(rotationY, 1.0f, 0.0f, 0.0f);
	glRotatef(rotationX, 0.0f, 1.0f, 0.0f);

	// Automatic following "camera"
	glRotatef((float) droneYaw, 0.0f, 1.0f, 0.0f);
	if(_sessionViewer)
	{
		glTranslatef((float) dronePosX, 0.0f, (float) dronePosY);
	}
	else
	{
		glTranslatef((float) -dronePosX, 0.0f, (float) -dronePosY);
	}

	drawGrid(10.0f, 10.0f);
	drawPath(-dronePosX, -dronePosY, droneAlt);
	drawDrone(-dronePosX, -dronePosY, droneAlt, -droneYaw, -dronePitch, -droneRoll);

	// Paint 2D stuff
	/*glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	draw2D(painter);
	painter.end();*/
}

void Map3D::draw2D(QPainter &p)
{
	 if(!_showHelp)
	 {
		 QString text = tr("Press 'H' for help");
		 QFontMetrics metrics = QFontMetrics(font());
		 int border = qMax(4, metrics.leading());

		 QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125), Qt::AlignCenter | Qt::TextWordWrap, text);
		 p.setRenderHint(QPainter::TextAntialiasing);
		 p.setPen(Qt::white);
		 p.fillRect(QRect(0, height() - rect.height() - border * 2, width(), rect.height() + 2*border), QColor(0, 0, 0, 127));
		 p.drawText((width() - rect.width())/2, height() - rect.height() - border, rect.width(), rect.height(), Qt::AlignCenter | Qt::TextWordWrap, text);
	 }
	 else
	 {
		 int w = 500;
		 int h = 170;

		 // Draw help panel
		 QRect helpRect(width() / 2 - w/2, height() / 2 - h/2, w, h);
		 p.setPen(QPen());
		 p.setBrush(QBrush(QColor(0, 0, 0, 180)));
		 p.drawRoundedRect(helpRect, 2.0, 2.0);

		 QFont titleFont("Sans Serif", 16, QFont::Bold);
		 QFont commandFont("Sans Serif", 12, QFont::Bold);
		 QFont descriptionFont("Sans Serif", 12, QFont::Normal);
		 QString title = tr("Navigation");
		 QFontMetrics metrics = QFontMetrics(titleFont);
		 p.setPen(Qt::white);
		 p.setFont(titleFont);
		 p.drawText(QPoint(width() / 2 - metrics.width(title) / 2, height() / 2 - h/2 + metrics.height() + 10), title);

		 p.setFont(commandFont);
		 p.drawText(QPoint(width() / 2 - w/2 + 20, height()/2 - h/2 + 70), tr("Rotation"));
		 p.drawText(QPoint(width() / 2 - w/2 + 20, height()/2 - h/2 + 70 + 20), tr("Pan"));
		 p.drawText(QPoint(width() / 2 - w/2 + 20, height()/2 - h/2 + 70 + 20 + 20), tr("Zoom"));
		 p.drawText(QPoint(width() / 2 - w/2 + 20, height()/2 - h/2 + 70 + 20 + 20 + 20), tr("Reset View"));

		 p.setFont(descriptionFont);
		 p.drawText(QPoint(width() / 2 - w/2 + 150, height()/2 - h/2 + 70), tr("Click with left mouse button and drag"));
		 p.drawText(QPoint(width() / 2 - w/2 + 150, height()/2 - h/2 + 70 + 20), tr("Click with middle mouse button and drag"));
		 p.drawText(QPoint(width() / 2 - w/2 + 150, height()/2 - h/2 + 70 + 20 + 20), tr("Click with right mouse button and drag"));
		 p.drawText(QPoint(width() / 2 - w/2 + 150, height()/2 - h/2 + 70 + 20 + 20 + 20), tr("Press 'R'"));
	 }
}

void Map3D::drawGrid(float gridWidth, float gridHeight)
{
	// Draws the ground plane as a grid

	for(int i = (int) -gridWidth; i <= gridWidth; i++)
	{
		if(i == 0)
		{
			glLineWidth(2.5f);
		}
		else
		{
			glLineWidth(1.0f);
		}

		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(i, 0.0f, (float) -gridHeight);
			glVertex3f(i, 0.0f, (float) gridHeight);
		glEnd();
	}

	for(int i = (int) -gridHeight; i <= gridHeight; i++)
	{
		if(i == 0)
		{
			glLineWidth(2.5f);
		}
		else
		{
			glLineWidth(1.0f);
		}

		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f((float) -gridWidth, 0.0f, i);
			glVertex3f((float) gridWidth, 0.0f, i);
		glEnd();
	}
}

void Map3D::drawDrone(float x, float y, float z, float heading, float pitch, float roll)
{
	// Draw the position marker at the right position
	glPushMatrix();
	glTranslatef(x, z, y);
	glRotatef(heading, 0.0f, 1.0f, 0.0f);
	glRotatef(pitch, 1.0f, 0.0f, 0.0f);
	glRotatef(roll, 0.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		calculateSurfaceNormal(glm::vec3(0.25f, 0.25f, 0.5f), glm::vec3(0.25f, -0.25f, 0.5f));
		glVertex3f(0.25f, 0.25f, 0.5f);
		glVertex3f(0.25f, -0.25f, 0.5f);
		glVertex3f(-0.25f, -0.25f, 0.5f);
		glVertex3f(-0.25f, 0.25f, 0.5f);
	glEnd();

	glBegin(GL_TRIANGLES);
		calculateSurfaceNormal(glm::vec3(-0.0f, 0.0f, -0.5f), glm::vec3(-0.25f, -0.25f, 0.5f));
		glVertex3f(-0.0f, 0.0f, -0.5f);
		glVertex3f(-0.25f, -0.25f, 0.5f);
		glVertex3f(-0.25f, 0.25f, 0.5f);

		calculateSurfaceNormal(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.25f, -0.25f, 0.5f));
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(0.25f, -0.25f, 0.5f);
		glVertex3f(0.25f, 0.25f, 0.5f);

		calculateSurfaceNormal(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.25f, -0.25f, 0.5f));
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(0.25f, -0.25f, 0.5f);
		glVertex3f(-0.25f, -0.25f, 0.5f);

		calculateSurfaceNormal(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.25f, 0.25f, 0.5f));
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(0.25f, 0.25f, 0.5f);
		glVertex3f(-0.25f, 0.25f, 0.5f);
	glEnd();

	// Reset transform
	glPopMatrix();
}

void Map3D::drawPath(float x, float y, float z) {
	// Add segment to path if drone moved more than 0.05 meter
	if(!_sessionViewer)
	{
		if(_path.size() >= 1)
		{
			if((abs(x - _path[_path.size() - 1]->x) >= 0.05f) || (abs(y - _path[_path.size() - 1]->y) >= 0.05f))
			{
				glm::vec3 *v = new glm::vec3(x, y, z);
				_path.push_back(v);
			}
		}
		else
		{
			glm::vec3 *v = new glm::vec3(x, y, z);
			_path.push_back(v);
		}
	}

	// Draw path
	glColor3f(0.0f, 1.0f, 0.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINE_STRIP);
		for(unsigned int i = 0; i < _path.size(); i++)
		{
			glVertex3f(_path[i]->x, _path[i]->z, _path[i]->y);
		}
	glEnd();
	glLineWidth(1.0f);
}

void Map3D::resetUserTransform()
{
	rotationX = 0.0f;
	rotationY = 25.0f;
	offsetX = 0.0f;
	offsetY = 0.0f;
	zoom = -10.0f;

	requestUpdateGL();
}

void Map3D::keyPressEvent(QKeyEvent *e)
{
	if(!e->isAutoRepeat())
	{
		if(e->key() == Qt::Key_H)
		{
			_showHelp = true;
			requestUpdateGL();
		}
		else if(e->key() == Qt::Key_R)
		{
			resetUserTransform();
		}
	}
}

void Map3D::keyReleaseEvent(QKeyEvent *e)
{
	if(!e->isAutoRepeat())
	{
		if(e->key() == Qt::Key_H)
		{
			_showHelp = false;
			requestUpdateGL();
		}
	}
}

void Map3D::mousePressEvent(QMouseEvent *e)
{
	lastPos = e->pos();
}

void Map3D::mouseMoveEvent(QMouseEvent *e)
{
	float dx = e->x() - lastPos.x();
	float dy = e->y() - lastPos.y();

	if(e->buttons() & Qt::LeftButton)
	{
		rotationX += 180.0f * (dx / width());
		rotationY += 180.0f * (dy / height());
	}
	else if(e->buttons() & Qt::RightButton)
	{
		zoom += 15.0f * (dy / width());
	}
	else if(e->buttons() & Qt::MiddleButton)
	{
		offsetX += 5.0f * (dx / width());
		offsetY -= 5.0f * (dy / height());
	}

	lastPos = e->pos();

	requestUpdateGL();
}

void Map3D::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void Map3D::calculateSurfaceNormal(glm::vec3 p1, glm::vec3 p2)
{
	glm::vec3 normal = glm::normalize(glm::cross(p1, p2));
	glNormal3f(normal.x, normal.y, normal.z);
}
