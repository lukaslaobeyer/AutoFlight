#ifndef CODECCONFIG_H
#define CODECCONFIG_H

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

#include "videoframe.h"

struct CodecConfig
{
	AVCodec *pCodecMP4 = NULL;
	AVCodec *pCodecH264 = NULL;

	AVCodecContext *pCodecCtxMP4 = NULL;
	AVCodecContext *pCodecCtxH264 = NULL;

	AVDictionary *optionsDict = NULL;

	SwsContext *swsCtx = NULL;

	AVFrame *pFrame = NULL;
	AVFrame *pFrameOutput = NULL;

	VideoFrame destPicture;
};

#endif
