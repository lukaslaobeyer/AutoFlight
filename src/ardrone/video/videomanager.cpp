#include "videomanager.h"
#include <iomanip>
#include <boost/bind.hpp>
#include <iostream>

extern "C"
{
	#include <libswscale/swscale.h>
}

#include "videoframe.h"
#include "../../afconstants.h"

using namespace std;
using boost::asio::ip::tcp;

VideoManager::VideoManager()
{
	_receivedDataBuffer = new char[BUFFER_MAX_SIZE * MAX_FRAMES_PER_PACKET];
	_rawFrame = new char[MAX_FRAMES_PER_PACKET * BUFFER_MAX_SIZE];
}

VideoManager::~VideoManager()
{
	// Stop recording if recording
	stopRecording();

	closeDecoder();

	if(socket != NULL)
	{
		socket->close();
		delete socket;
	}

	delete[] _receivedDataBuffer;

	delete[] _rawFrame;
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

bool VideoManager::startRecording(string savePath)
{
	if(_decodedPackets >= 1)
	{
		av_register_all();

		avformat_alloc_output_context2(&_recording_ctx, NULL, "mp4", savePath.c_str());
		if(_recording_ctx == NULL)
		{
			return false;
		}

		// Add a video stream
		AVStream *vstream = NULL;
		vstream = avformat_new_stream(_recording_ctx, cfg.pCodecH264);
		vstream->codec = cfg.pCodecCtxH264;
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
			cout << "Error while writing video frame: " << ret_code << endl;
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
	socket->async_receive(boost::asio::buffer(_receivedDataBuffer, BUFFER_MAX_SIZE * MAX_FRAMES_PER_PACKET), boost::bind(&VideoManager::packetReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void VideoManager::recording_startReceive()
{
	recordingSocket->async_receive(boost::asio::buffer(_recording_receivedDataBuffer), boost::bind(&VideoManager::recording_packetReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void VideoManager::packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	frame_ready = false;

	while(_status == PROCESSING);

	_status = PROCESSING;

	if(!error)
	{
		if(frameHasPaVE(_receivedDataBuffer))
		{
			_pave[0] = *parsePaVE(_receivedDataBuffer);

			if(_pave[0].payload_size >= BUFFER_MAX_SIZE)
			{
				// Frame to big for buffer. Should never happen.
				cout << "Frame too big for buffer. Discarding it." << endl;
				_status = READY;
				startReceive();
				return;
			}

			if(_pave[0].payload_size + _pave[0].header_size <= bytes_transferred)
			{
				// Full frame received
				memcpy(_rawFrame, &_receivedDataBuffer[_pave[0].header_size], _pave[0].payload_size); // Copy the frame to the buffer, and remove the PaVE header
				framesInPacket = 1;
				reconstructed_frame_write_position = -1; // No reconstruction needed when a full frame is received, so set this variables to -1
				_frame_size[0] = _pave[0].payload_size;

				if(_pave[0].payload_size + _pave[0].header_size < bytes_transferred)
				{
					// Multiple frames in one packet
					int frameIndex = 1;
					int frameOffset = _pave[0].payload_size + _pave[0].header_size;

					while(frameHasPaVE(_receivedDataBuffer, frameOffset)) // Check if more PaVE headers are present
					{
						if(frameIndex < MAX_FRAMES_PER_PACKET)
						{
							_pave[frameIndex] = *parsePaVE(_receivedDataBuffer, frameOffset);

							if(_pave[frameIndex].payload_size >= BUFFER_MAX_SIZE)
							{
								cout << "Frame too big for buffer. Discarding it." << endl;
								break;
							}

							memcpy(_rawFrame + (BUFFER_MAX_SIZE * frameIndex), _receivedDataBuffer + frameOffset + _pave[frameIndex].header_size, _pave[frameIndex].payload_size); // Copy the frame to the buffer, and remove the PaVE header
							_frame_size[frameIndex] = _pave[frameIndex].payload_size;

							frameOffset += _pave[frameIndex].payload_size + _pave[frameIndex].header_size;

							frameIndex++;
							framesInPacket++;
						}
						else
						{
							cout << "Too many frames in one packet (more than " << MAX_FRAMES_PER_PACKET << ")!" << endl;
							break;
						}
					}
				}

				frame_ready = true;
			}
			else if(_pave[0].payload_size + _pave[0].header_size > bytes_transferred)
			{
				// Frame seems to be split over more than one packet
				memcpy(_rawFrame, &_receivedDataBuffer[_pave[0].header_size], bytes_transferred - _pave[0].header_size);  // Copy the partial frame to the buffer, and remove the PaVE header
				frame_ready = false;
				reconstructed_frame_write_position = bytes_transferred - _pave[0].header_size;
				_frame_size[0] = _pave[0].payload_size;
			}
		}
		else
		{
			if(frame_ready == false && reconstructed_frame_write_position >= 0 && _frame_size[0] >= 0)
			{
				// Data seems to be a part of a split packet: add received data
				//memcpy(&_rawFrame[reconstructed_frame_write_position], &_receivedDataBuffer, (unsigned long int) bytes_transferred);
				memcpy(_rawFrame + reconstructed_frame_write_position, _receivedDataBuffer, (unsigned long int) bytes_transferred);

				if(_frame_size[0] <= reconstructed_frame_write_position + bytes_transferred)
				{
					// Frame has been completely received
					framesInPacket = 1;
					frame_ready = true;
					reconstructed_frame_write_position = -1;
				}
				else
				{
					reconstructed_frame_write_position += bytes_transferred;
				}
			}
			else
			{
				cout << "Frame has no PaVE header!" << endl;
			}
		}
	}

	if(frame_ready)
	{
		if(_pave[0].frame_type == ardrone::video::frame_type::I || _pave[0].frame_type == ardrone::video::frame_type::IDR)
		{
			_got_first_iframe = true;
		}

		decodePacket();
	}

	_decodedPackets++;

	_status = READY;

	// Listen for the next packet
	startReceive();
}

void VideoManager::recording_packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	recording_frame_ready = false;

	if((_recording || _start_recording_requested) && !(_stop_recording_requested))
	{
		if(!error)
		{
			if(frameHasPaVE(_recording_receivedDataBuffer))
			{
				_recording_pave = *parsePaVE(_recording_receivedDataBuffer);

				if(_recording_pave.payload_size >= BUFFER_MAX_SIZE)
				{
					// Frame to big for buffer. Should never happen.
					recording_startReceive();
					return;
				}

				if(_recording_pave.payload_size + _recording_pave.header_size == bytes_transferred)
				{
					// Full frame received
					memcpy(&_recording_rawFrame, &_recording_receivedDataBuffer[_recording_pave.header_size], bytes_transferred - _recording_pave.header_size); // Copy the frame to the buffer, and remove the PaVE header
					recording_frame_ready = true;
					recording_reconstructed_frame_write_position = -1; // No reconstruction needed when a full frame is received, so set this variables to -1
					_recording_frame_size = _recording_pave.payload_size;
				}
				else if(_recording_pave.payload_size + _recording_pave.header_size > bytes_transferred)
				{
					// Frame seems to be split over more than one packet
					memcpy(&_recording_rawFrame, &_recording_receivedDataBuffer[_recording_pave.header_size], bytes_transferred - _recording_pave.header_size);  // Copy the partial frame to the buffer, and remove the PaVE header
					recording_frame_ready = false;
					recording_reconstructed_frame_write_position = bytes_transferred - _recording_pave.header_size;
					_recording_frame_size = _recording_pave.payload_size;
				}
			}
			else
			{
				if(recording_frame_ready == false && recording_reconstructed_frame_write_position >= 0 && _recording_frame_size >= 0)
				{
					// Data seems to be a part of a split packet: add received data
					memcpy(&_recording_rawFrame[recording_reconstructed_frame_write_position], &_recording_receivedDataBuffer, (unsigned long int) bytes_transferred);

					if(_recording_frame_size <= recording_reconstructed_frame_write_position + bytes_transferred)
					{
						// Frame has been completely received
						recording_frame_ready = true;
						recording_reconstructed_frame_write_position = -1;
					}
					else
					{
						recording_reconstructed_frame_write_position += bytes_transferred;
					}
				}
				else
				{
					cout << "[Recording] Frame has no PaVE header!" << endl;
				}
			}
		}

		if(recording_frame_ready)
		{
			if(_recording_pave.frame_type == ardrone::video::frame_type::I || _recording_pave.frame_type == ardrone::video::frame_type::IDR)
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
				recording_writeFrame();
			}
		}
	}
	else if(_stop_recording_requested)
	{
		_recording = false;
		_stop_recording_requested = false;
	}
	// Listen for the next packet in the record stream
	recording_startReceive();
}

bool VideoManager::frameHasPaVE(char *frame, unsigned int offset)
{
	if((uint8_t) frame[offset] == 'P' && (uint8_t) frame[offset + 1] == 'a' && (uint8_t) frame[offset + 2] == 'V' && (uint8_t) frame[offset + 3] == 'E')
	{
		return true;
	}
	else
	{
		return false;
	}
}

PaVE *VideoManager::parsePaVE(char *frame, unsigned int offset)
{
	PaVE *p = (PaVE *) (frame + offset);
	return p;
}

bool VideoManager::initializeDecoder()
{
	cout << "Initializing decoders" << endl;

	avcodec_register_all();

	// Basically same as in the AR.Drone SDK

	cfg.pCodecMP4 = avcodec_find_decoder(CODEC_ID_MPEG4);
	cfg.pCodecH264 = avcodec_find_decoder(CODEC_ID_H264);
	if(NULL == cfg.pCodecMP4 || NULL == cfg.pCodecH264)
	{
		cout << "Needed codecs are not supported!" << endl;
		return false;
	}

	cfg.pCodecCtxMP4 = avcodec_alloc_context3(cfg.pCodecMP4);
	cfg.pCodecCtxH264 = avcodec_alloc_context3(cfg.pCodecH264);
	if(NULL == cfg.pCodecCtxMP4 || NULL == cfg.pCodecCtxH264)
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

	cfg.pFrameOutput = avcodec_alloc_frame();
	cfg.pFrame = avcodec_alloc_frame();

	if(NULL == cfg.pFrameOutput || NULL == cfg.pFrame)
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

	for(int i = 0; i < framesInPacket; i++)
	{
		if(initializationNeeded || (_pave[i].encoded_stream_width != _previous_width))
		{
			cout << "Initializing buffers" << endl;

			cfg.pCodecCtxMP4->width = _pave[i].encoded_stream_width;
			cfg.pCodecCtxMP4->height = _pave[i].encoded_stream_height;
			cfg.pCodecCtxH264->width = _pave[i].encoded_stream_width;
			cfg.pCodecCtxH264->height = _pave[i].encoded_stream_height;

			cfg.destPicture.width = _pave[i].display_width;
			cfg.destPicture.height = _pave[i].display_height;
			cfg.destPicture.format = PIX_FMT_BGR24;

			_decode_buffer_size = avpicture_get_size(cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);
			_decode_buffer = (uint8_t *) av_realloc(_decode_buffer, _decode_buffer_size * sizeof(uint8_t));

			avpicture_fill((AVPicture *) cfg.pFrameOutput, _decode_buffer, cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);

			cfg.swsCtx = sws_getCachedContext(cfg.swsCtx, _pave[i].display_width, _pave[i].display_height,
											  cfg.pCodecCtxH264->pix_fmt, _pave[0].display_width, _pave[i].display_height,
											  cfg.destPicture.format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		}

		if(_got_first_iframe) // Wait for I-frame
		{
			_packet.data = (unsigned char*) &_rawFrame[i * BUFFER_MAX_SIZE];
			_packet.size = _frame_size[i];

			// Try to decode the packet
			int frameFinished = 0;

			if(_pave[i].video_codec == ardrone::video::codec::MPEG4_VISUAL)
			{
				avcodec_decode_video2(cfg.pCodecCtxMP4, cfg.pFrame, &frameFinished, &_packet);
			}
			else if(_pave[i].video_codec ==  ardrone::video::codec::MPEG4_AVC)
			{
				avcodec_decode_video2(cfg.pCodecCtxH264, cfg.pFrame, &frameFinished, &_packet);
			}

			if(frameFinished)
			{
				// Frame successfully decoded :)
				cfg.pFrameOutput->data[0] = _decode_buffer;

				sws_scale(cfg.swsCtx, (const uint8_t *const*)cfg.pFrame->data, cfg.pFrame->linesize, 0,
						  _pave[0].display_height, cfg.pFrameOutput->data, cfg.pFrameOutput->linesize);

				_frame = cv::Mat(_pave[i].display_height, _pave[i].display_width, CV_8UC3, _decode_buffer);
			}

			_previous_width = _pave[i].encoded_stream_width;
		}
	}
}

bool VideoManager::takePicture(string savePath)
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
		cout << "Could not save image! " << ex.what() << endl;
		succeeded = false;
	}
	_status = READY;

	return succeeded;
}

void VideoManager::close()
{
	socket->close();
}
