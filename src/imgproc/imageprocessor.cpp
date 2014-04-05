#include "imageprocessor.h"
#include <iostream>
#include <vector>

using namespace std;

ImageProcessor::ImageProcessor()
{
	_iv = new ImageVisualizer;
}

void ImageProcessor::startProcessing()
{
	if(_t_proc == nullptr)
	{
		_update_latest = true;
		_t_proc = new boost::thread(&ImageProcessor::runProcessingLoop, this);
	}
}

void ImageProcessor::runProcessingLoop()
{
	while(_update_latest)
	{
		processLatestFrame();
	}
}

void ImageProcessor::stopProcessing()
{
	if(_t_proc != nullptr)
	{
		_update_latest = false;
		_t_proc->join();

		delete _t_proc;
		_t_proc = nullptr;
	}
}

void ImageProcessor::videoFrameAvailable(cv::Mat frame)
{
	// Only copy the new frame into _latestFrame if no operation is being done currently
	if(_update_latest && _status == READY_FOR_NEXT_FRAME)
	{
		// Perform a deep copy of the latest frame for processing
		_latestFrame = frame.clone();

		_status = READY_FOR_PROCESS;
	}
}

void ImageProcessor::processLatestFrame()
{
	while(_status != READY_FOR_PROCESS);

	_status = PROCESSING_FRAME;
	// Just trying out some stuff

	// Reduce noise
	cv::GaussianBlur(_latestFrame, _latestFrame, cv::Size(3, 3), 0, 0);

	// Color threshold
	cv::Mat latestFrameHSV;
	cv::cvtColor(_latestFrame, latestFrameHSV, CV_BGR2HSV);
	cv::Mat redOnly;
	cv::Mat red0; // Red with hue between 0 and 30
	cv::Mat red1; // Red with hue between 150 and 180
	cv::inRange(latestFrameHSV, cv::Scalar(0, 80, 40), cv::Scalar(10, 200, 200), red0);
	cv::inRange(latestFrameHSV, cv::Scalar(170, 80, 40), cv::Scalar(180, 200, 200), red1);
	cv::bitwise_or(red0, red1, redOnly);

	cv::Mat eroded;
	cv::erode(redOnly, eroded, cv::Mat(), cv::Point(-1, -1), 20);
	//cv::morphologyEx(redOnly, opened, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 20);

	cv::Moments m = cv::moments(eroded, true);
	cv::Point centroid(m.m10/m.m00, m.m01/m.m00);

	cv::Mat final;
	cv::cvtColor(eroded, final, CV_GRAY2BGR);

	if(centroid.x > 0 && centroid.y > 0)
	{
		cv::circle(final, centroid, 5, cv::Scalar(0, 255, 0), 2);
	}

	//_iv->showImage(display);
	_iv->showImage(final);

	_status = READY_FOR_NEXT_FRAME;
}
