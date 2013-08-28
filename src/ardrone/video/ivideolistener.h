#ifndef IVIDEOLISTENER_H
#define IVIDEOLISTENER_H

#include <opencv2/core/core.hpp>

class IVideoListener
{
	public:
        	virtual ~IVideoListener() {}
        	virtual void videoFrameAvailable(cv::Mat frame) = 0;
};

#endif
