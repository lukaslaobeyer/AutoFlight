#ifndef IMAGEVISUALIZER_H
#define IMAGEVISUALIZER_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageVisualizer : public QObject
{
	Q_OBJECT

	public:
		ImageVisualizer();
		void showImage(cv::Mat &f);
	private Q_SLOTS:
		void videoFrameAvailable(QImage img);
	Q_SIGNALS:
		void videoFrameAvailableSignal(QImage img);
};

#endif
