#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <libavcodec/avcodec.h>

struct VideoFrame
{
	PixelFormat format;
	int width;
	int height;
};

#endif
