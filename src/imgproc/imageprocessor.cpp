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

	// Only red objects

	// Reduce noise
	cv::GaussianBlur(_latestFrame, _latestFrame, cv::Size(3, 3), 0, 0);

	// Edge detection
	cv::Mat cannyFrame;
	cv::Canny(_latestFrame, cannyFrame, 100, 100);

	// Color threshold
	cv::Mat latestFrameHSV;
	cv::cvtColor(_latestFrame, latestFrameHSV, CV_BGR2HSV);
	cv::Mat redOnly;
	cv::inRange(latestFrameHSV, cv::Scalar(-30, 120, 80), cv::Scalar(30, 255, 255), redOnly);

	// Mask
	cv::Mat display;
	cannyFrame.copyTo(display, redOnly);

	//_iv->showImage(display);
	_iv->showImage(redOnly);

	_status = READY_FOR_NEXT_FRAME;
}
