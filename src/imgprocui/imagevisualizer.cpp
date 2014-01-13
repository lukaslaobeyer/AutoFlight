#include "imagevisualizer.h"

#include <QtWidgets>

ImageVisualizer::ImageVisualizer() : QObject()
{
	QObject::connect(this, SIGNAL(videoFrameAvailableSignal(QImage)), this, SLOT(videoFrameAvailable(QImage)));
}

void ImageVisualizer::showImage(cv::Mat &f)
{
	QImage::Format format;

	if(f.type() == CV_8UC3)
	{
		format = QImage::Format_RGB888;
	}
	else if(f.type() == CV_8UC1)
	{
		format = QImage::Format_Indexed8;
	}
	else
	{
		qDebug("Unsupported image format! (type of matrix not CV_8UC1 or CV_8UC3)");
		return;
	}

	QImage img(f.data, f.cols, f.rows, f.step, format);
	if(f.type() == CV_8UC3)
	{
		img = img.rgbSwapped();
	}

	Q_EMIT videoFrameAvailableSignal(img);
}

void ImageVisualizer::videoFrameAvailable(QImage img)
{
	static bool initialized = false;
	static QGraphicsScene scene;
	static QGraphicsView view(&scene);
	static QGraphicsPixmapItem item;

	if(!initialized)
	{
		item.setPixmap(QPixmap::fromImage(img));

		scene.addItem(&item);
		view.show();

		initialized = true;
	}
	else
	{
		item.setPixmap(QPixmap::fromImage(img));
	}
}
