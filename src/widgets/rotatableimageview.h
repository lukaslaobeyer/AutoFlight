#ifndef ROTATABLEIMAGEVIEW_H
#define ROTATABLEIMAGEVIEW_H

#include <QWidget>
#include <QGraphicsView>

class RotatableImageView : public QWidget
{
	Q_OBJECT
	
	public:
		explicit RotatableImageView(QString file, QWidget *parent = 0);
		void rotate(int degrees);
	private:
		QGraphicsView *imageView;
		int _width;
		int _height;
};

#endif
