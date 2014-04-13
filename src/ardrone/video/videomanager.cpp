#include "videomanager.h"
#include <iomanip>
#include <boost/bind.hpp>
#include <iostream>

extern "C"
{
	#include <libswscale/swscale.h>
}

#include "videoframe.h"
#include "framereconstructor.h"
#include "../../afconstants.h"

using namespace std;
using boost::asio::ip::tcp;

VideoManager::VideoManager()
{
	_receivedDataBuffer = new char[BUFFER_MAX_SIZE * 10];
	_recording_receivedDataBuffer = new char[BUFFER_MAX_SIZE * 10];
}

VideoManager::~VideoManager()
{
	while(_status == PROCESSING);

	// Stop recording if recording
	stopRecording();

	closeDecoder();

	if(socket != nullptr)
	{
		delete socket;
	}

	if(recordingSocket != nullptr)
	{
		delete recordingSocket;
	}

	delete[] _receivedDataBuffer;
	delete[] _recording_receivedDataBuffer;
}

void VideoManager::init(string ip, boost::asio::io_service &io_service)
{
	// For the live stream
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(ip, to_string(ardrone::VIDEO_PORT));
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	socket = new tcp::socket(io_service);
	boost::asio::connect(*socket, endpoint_iterator);

	startReceive();

	// For the record stream
	tcp::resolver r_resolver(io_service);
	tcp::resolver::query r_query(ip, to_string(ardrone::VIDEO_RECORDING_PORT));
	tcp::resolver::iterator r_endpoint_iterator = r_resolver.resolve(r_query);

	recordingSocket = new tcp::socket(io_service);
	boost::asio::connect(*recordingSocket, r_endpoint_iterator);

	recording_startReceive();
}

cv::Mat VideoManager::getVideoFrame()
{
	return _frame;
}

void VideoManager::update()
{
	socket->get_io_service().poll();
}

bool VideoManager::startRecording(const string &savePath)
{
	static bool already_registered = false;

	if(_decodedPackets >= 1)
	{
		if(!already_registered)
		{
			av_register_all();
			already_registered = true;
		}

		avformat_alloc_output_context2(&_recording_ctx, NULL, "mp4", savePath.c_str());
		if(_recording_ctx == NULL)
		{
			return false;
		}

		// Add a video stream
		AVCodec *codec = avcodec_find_decoder(CODEC_ID_H264);
		AVCodecContext *codecctx = avcodec_alloc_context3(codec);

		codecctx->pix_fmt = PIX_FMT_YUV420P;
		codecctx->skip_frame = AVDISCARD_DEFAULT;
		codecctx->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
		codecctx->err_recognition = AV_EF_CAREFUL;
		codecctx->skip_loop_filter = AVDISCARD_DEFAULT;
		codecctx->workaround_bugs = FF_BUG_AUTODETECT;
		codecctx->codec_type = AVMEDIA_TYPE_VIDEO;
		codecctx->codec_id = CODEC_ID_H264;
		codecctx->skip_idct = AVDISCARD_DEFAULT;
		codecctx->width = 1280;
		codecctx->height = 720;

		AVStream *vstream = nullptr;
		vstream = avformat_new_stream(_recording_ctx, codec);
		vstream->codec = codecctx;
		vstream->codec->time_base.den = 30; // 30 fps
		vstream->codec->time_base.num = 1;

		// Open output file
		if (!(_recording_ctx->oformat->flags & AVFMT_NOFILE))
		{
			int ret_code = avio_open(&_recording_ctx->pb, savePath.c_str(), AVIO_FLAG_WRITE);

			if(ret_code < 0)
			{
				cout << "Could not open file: " << ret_code << endl;
				return false;
			}
		}

		int ret_code = avformat_write_header(_recording_ctx, NULL);
		if(ret_code < 0)
		{
			cout << "Error ocurred when opening output file: " << ret_code << endl;
			return false;
		}

		_start_recording_requested = true;
		return true;
	}
	else
	{
		return false;
	}
}

void VideoManager::recording_writeFrame()
{
	_recording_status = WRITING_FRAME;

	if(_recording_ctx != NULL)
	{
		AVPacket recording_packet;
		av_init_packet(&recording_packet);

		recording_packet.stream_index = 0; // First stream as there's only one stream
		recording_packet.data = (unsigned char *) _recording_rawFrame;
		recording_packet.size = _recording_frame_size;

		int ret_code = av_interleaved_write_frame(_recording_ctx, &recording_packet);

		if(ret_code != 0)
		{
			cerr << "[Error] Could not write video frame: " << ret_code << endl;
		}
	}

	_recording_status = READY;
}

bool VideoManager::stopRecording()
{
	if(_recording || _start_recording_requested)
	{
		_stop_recording_requested = true;

		while(_recording_status == WRITING_FRAME);

		// Close and free resources
		if(_recording_ctx != NULL)
		{
			av_write_trailer(_recording_ctx);
		}
		else
		{
			return false;
		}

		if(!(_recording_ctx->oformat->flags & AVFMT_NOFILE))
		{
			avio_close(_recording_ctx->pb);
		}

		avformat_free_context(_recording_ctx);

		_recording_ctx = NULL;

		return true;
	}
	else
	{
		return false;
	}
}

void VideoManager::startReceive()
{
	socket->async_receive(boost::asio::buffer(_receivedDataBuffer, BUFFER_MAX_SIZE), boost::bind(&VideoManager::packetReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void VideoManager::recording_startReceive()
{
	recordingSocket->async_receive(boost::asio::buffer(_recording_receivedDataBuffer, BUFFER_MAX_SIZE), boost::bind(&VideoManager::recording_packetReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void VideoManager::packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	static FrameReconstructor r;

	while(_status == PROCESSING);

	_status = PROCESSING;

	if(!error)
	{
		bool moreFrames = true;
		int offset = 0;

		while(moreFrames)
		{
			bool frameInPacket = r.decodePacket(_receivedDataBuffer, bytes_transferred, offset);

			if(!frameInPacket)
			{
				// No frame data in packet. Should not happen.
				cerr << "[Error] No frame in packet!" << endl;
				moreFrames = false;
			}
			else
			{
				bool frameAvailable = r.frameAvailable();

				if(frameAvailable)
				{
					// Complete frame available.
					_rawFrame = r.getNextFrame();
					_pave = r.getNextHeader();
					offset += r.getOffset();

					// Decode the frame
					if(_rawFrame != nullptr && _pave != nullptr)
					{
						decodePacket();
					}

					// Check if there are more frames in the packet
					moreFrames = r.moreFramesInPacket();
				}
				else
				{
					// Didn't receive a complete frame. Wait for the next packet so the frame can be completed
					moreFrames = false;
				}
			}
		}
	}

	_decodedPackets++;

	_status = READY;

	// Listen for the next packet
	startReceive();
}

void VideoManager::recording_packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	static FrameReconstructor r;

	if((_recording || _start_recording_requested) && !(_stop_recording_requested))
	{
		if(!error)
		{
			bool moreFrames = true;
			int offset = 0;

			while(moreFrames)
			{
				bool frameInPacket = r.decodePacket(_recording_receivedDataBuffer, bytes_transferred, offset);

				if(!frameInPacket)
				{
					// No frame data in packet. Should not happen.
					cerr << "[Error] [Recording] No frame in packet!" << endl;
					moreFrames = false;
				}
				else
				{
					bool frameAvailable = r.frameAvailable();

					if(frameAvailable)
					{
						// Complete frame available.
						_recording_rawFrame = r.getNextFrame();
						_recording_pave = r.getNextHeader();
						offset += r.getOffset();

						if(_recording_rawFrame != nullptr && _recording_pave != nullptr)
						{
							recording_frame_ready = true;

							if(_recording_pave->frame_type == ardrone::video::frame_type::I || _recording_pave->frame_type == ardrone::video::frame_type::IDR)
							{
								_recording_got_first_iframe = true;
							}

							if(_start_recording_requested && _recording_got_first_iframe)
							{
								_recording = true;
								_start_recording_requested = false;
							}

							if(_recording)
							{
								_recording_frame_size = _recording_pave->payload_size;
								recording_writeFrame();
							}
						}

						// Check if there are more frames in the packet
						moreFrames = r.moreFramesInPacket();
					}
					else
					{
						// Didn't receive a complete frame. Wait for the next packet so the frame can be completed
						moreFrames = false;
					}
				}
			}
		}
	}
	else if(_stop_recording_requested)
	{
		_recording = false;
		_stop_recording_requested = false;
	}

	_recording_decodedPackets++;

	// Listen for the next packet
	recording_startReceive();
}

bool VideoManager::initializeDecoder()
{
	cout << "Initializing decoders" << endl;

	avcodec_register_all();

	// Basically same as in the AR.Drone SDK

	cfg.pCodecMP4 = avcodec_find_decoder(CODEC_ID_MPEG4);
	cfg.pCodecH264 = avcodec_find_decoder(CODEC_ID_H264);
	if(nullptr == cfg.pCodecMP4 || nullptr == cfg.pCodecH264)
	{
		cout << "Needed codecs are not supported!" << endl;
		return false;
	}

	cfg.pCodecCtxMP4 = avcodec_alloc_context3(cfg.pCodecMP4);
	cfg.pCodecCtxH264 = avcodec_alloc_context3(cfg.pCodecH264);
	if(nullptr == cfg.pCodecCtxMP4 || nullptr == cfg.pCodecCtxH264)
	{
		cout << "Could not allocate codec context!" << endl;
		return false;
	}

	cfg.pCodecCtxMP4->pix_fmt = PIX_FMT_YUV420P;
	cfg.pCodecCtxMP4->skip_frame = AVDISCARD_DEFAULT;
	cfg.pCodecCtxMP4->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
	cfg.pCodecCtxMP4->err_recognition = AV_EF_CAREFUL;
	cfg.pCodecCtxMP4->skip_loop_filter = AVDISCARD_DEFAULT;
	cfg.pCodecCtxMP4->workaround_bugs = FF_BUG_AUTODETECT;
	cfg.pCodecCtxMP4->codec_type = AVMEDIA_TYPE_VIDEO;
	cfg.pCodecCtxMP4->codec_id = CODEC_ID_MPEG4;
	cfg.pCodecCtxMP4->skip_idct = AVDISCARD_DEFAULT;

	cfg.pCodecCtxH264->pix_fmt = PIX_FMT_YUV420P;
	cfg.pCodecCtxH264->skip_frame = AVDISCARD_DEFAULT;
	cfg.pCodecCtxH264->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
	cfg.pCodecCtxH264->err_recognition = AV_EF_CAREFUL;
	cfg.pCodecCtxH264->skip_loop_filter = AVDISCARD_DEFAULT;
	cfg.pCodecCtxH264->workaround_bugs = FF_BUG_AUTODETECT;
	cfg.pCodecCtxH264->codec_type = AVMEDIA_TYPE_VIDEO;
	cfg.pCodecCtxH264->codec_id = CODEC_ID_H264;
	cfg.pCodecCtxH264->skip_idct = AVDISCARD_DEFAULT;


	// Open codec

	if(avcodec_open2(cfg.pCodecCtxMP4, cfg.pCodecMP4, &cfg.optionsDict) < 0)
	{
		cout << "Error opening MP4 codec" << endl;
		return false;
	}
	if(avcodec_open2(cfg.pCodecCtxH264, cfg.pCodecH264, &cfg.optionsDict) < 0)
	{
		cout << "Error opening H264 codec" << endl;
		return false;
	}

	cfg.pFrameOutput = av_frame_alloc();
	cfg.pFrame = av_frame_alloc();

	if(nullptr == cfg.pFrameOutput || nullptr == cfg.pFrame)
	{
		cout << "Could not allocate frames" << endl;
		return false;
	}

	av_init_packet(&_packet);

	decoderInitialized = true;

	return true;
}

void VideoManager::closeDecoder()
{
	if(decoderInitialized)
	{
		av_free_packet(&_packet);
		av_free(_decode_buffer);
		av_free(cfg.pFrame);
		av_free(cfg.pFrameOutput);
		avcodec_close(cfg.pCodecCtxH264);
		avcodec_close(cfg.pCodecCtxMP4);
	}
}

void VideoManager::decodePacket()
{
	static bool got_first_iframe = false;
	bool initializationNeeded = false;

	if(!decoderInitialized)
	{
		if(!initializeDecoder())
		{
			throw CouldNotInitializeDecoderException();
		}
		else
		{
			initializationNeeded = true;
		}
	}

	if(initializationNeeded || (_pave->encoded_stream_width != _previous_width))
	{
		cout << "Initializing buffers" << endl;

		cfg.pCodecCtxMP4->width = _pave->encoded_stream_width;
		cfg.pCodecCtxMP4->height = _pave->encoded_stream_height;
		cfg.pCodecCtxH264->width = _pave->encoded_stream_width;
		cfg.pCodecCtxH264->height = _pave->encoded_stream_height;

		cfg.destPicture.width = _pave->display_width;
		cfg.destPicture.height = _pave->display_height;
		cfg.destPicture.format = PIX_FMT_BGR24;

		_decode_buffer_size = avpicture_get_size(cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);
		_decode_buffer = (uint8_t *) av_realloc(_decode_buffer, _decode_buffer_size * sizeof(uint8_t));

		avpicture_fill((AVPicture *) cfg.pFrameOutput, _decode_buffer, cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);

		cfg.swsCtx = sws_getCachedContext(cfg.swsCtx, _pave->display_width, _pave->display_height,
										  cfg.pCodecCtxH264->pix_fmt, _pave->display_width, _pave->display_height,
										  cfg.destPicture.format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	}

	if((_pave->frame_type == ardrone::video::frame_type::I || _pave->frame_type == ardrone::video::frame_type::IDR))
	{
		got_first_iframe = true;
	}

	if(got_first_iframe) // Wait for I-frame
	{
		_packet.data = (unsigned char *) _rawFrame;
		_packet.size = _pave->payload_size;

		// Try to decode the packet
		int frameFinished = 0;

		if(_pave->video_codec == ardrone::video::codec::MPEG4_VISUAL)
		{
			avcodec_decode_video2(cfg.pCodecCtxMP4, cfg.pFrame, &frameFinished, &_packet);
		}
		else if(_pave->video_codec ==  ardrone::video::codec::MPEG4_AVC)
		{
			avcodec_decode_video2(cfg.pCodecCtxH264, cfg.pFrame, &frameFinished, &_packet);
		}

		if(frameFinished)
		{
			// Frame successfully decoded :)
			cfg.pFrameOutput->data[0] = _decode_buffer;

			sws_scale(cfg.swsCtx, (const uint8_t *const*)cfg.pFrame->data, cfg.pFrame->linesize, 0,
					  _pave->display_height, cfg.pFrameOutput->data, cfg.pFrameOutput->linesize);

			_frame = cv::Mat(_pave->display_height, _pave->display_width, CV_8UC3, _decode_buffer);
		}
		else
		{
			cerr << "[Error] Could not decode frame: Error #" << frameFinished << endl;
		}
	}

	_previous_width = _pave->encoded_stream_width;
}

bool VideoManager::takePicture(const string &savePath)
{
	bool succeeded = false;

	while(_status == PROCESSING);

	_status = PROCESSING;
	try
	{
#ifdef __MINGW32__
#warning The OpenCV HighGUI module can conflict with Qt5. Make sure you have built OpenCV with Qt5.
#endif
		imwrite(savePath, _frame);
		succeeded = true;
	}
	catch (runtime_error& ex)
	{
		cerr << "Could not save image! " << ex.what() << endl;
		succeeded = false;
	}
	_status = READY;

	return succeeded;
}

void VideoManager::close()
{
	delete socket;
	delete recordingSocket;
	socket = nullptr;
	recordingSocket = nullptr;
}
