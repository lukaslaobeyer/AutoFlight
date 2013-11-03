#ifndef MAP3D_H
#define MAP3D_H

#include <QWidget>
#include <QSize>
#include <QGLWidget>
#include <glm/glm.hpp>
#include <QPoint>
#include <vector>
#include "../ardrone/navdata/qnavdatalistener.h"

class Map3D : public QGLWidget, public QNavdataListener
{
	Q_OBJECT

	public:
		explicit Map3D(QWidget *parent = 0);
		~Map3D();

		QSize minimumSizeHint() const;
		QSize sizeHint() const;

		void requestUpdateGL();
	public Q_SLOTS:
		void navdataAvailable(AFNavdata *nd);
	protected:
	    void initializeGL();
	    void paintGL();
	    void resizeGL(int width, int height);

	    long long deltaT(); // Returns ms since last call to this function

	    void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
	    void calculateSurfaceNormal(glm::vec3 p1, glm::vec3 p2);

	    void drawGrid(float gridWidth, float gridHeight);
	    void drawDrone(float x, float y, float z, float heading, float pitch, float roll);
	    void drawPath(float x, float y, float z);

	    void mousePressEvent(QMouseEvent *event);
	    void mouseMoveEvent(QMouseEvent *event);
	private:
	    float rotationX = 0.0f;
	    float rotationY = 25.0f;
	    float offsetX = 0.0f;
	    float offsetY = 0.0f;
	    float zoom = -10.0f;

	    long long lastT = 0;
	    long long lastGLUpdate = 0;

	    float dronePosX = 0;
	    float dronePosY = 0;
	    float droneAlt = 0;
	    float dronePitch = 0;
	    float droneRoll = 0;
	    float droneYaw = 0;

	    QPoint lastPos; // Used for user rotation control with mouse

	    std::vector<glm::vec3 *> _path;
};

#endif
