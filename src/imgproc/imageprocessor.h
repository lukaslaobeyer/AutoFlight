#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#define PROCESSING_FRAME 2
#define READY_FOR_PROCESS 1
#define READY_FOR_NEXT_FRAME 0

#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <boost/thread.hpp>
#include "../ardrone/video/ivideolistener.h"
#include "../imgprocui/imagevisualizer.h"

class ImageProcessor : public IVideoListener
{
	public:
		ImageProcessor();
		void startProcessing();
		void stopProcessing();
		void videoFrameAvailable(cv::Mat frame);
	private:
		void processLatestFrame();
		void runProcessingLoop();

		cv::Mat _latestFrame;

		ImageVisualizer *_iv;

		boost::thread *_t_proc = nullptr;;

		volatile int _status = READY_FOR_NEXT_FRAME;
		volatile bool _update_latest = false; // Update the _latestFrame Mat with video frames received over the videoFrameAvailable method?
};

#endif
