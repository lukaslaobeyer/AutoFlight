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
	_receivedDataBuffer = new char[BUFFER_MAX_SIZE * 10];
	_recording_receivedDataBuffer = new char[BUFFER_MAX_SIZE * 10];
}

VideoManager::~VideoManager()
{
	while(_status == PROCESSING);

	// Stop recording if recording
	stopRecording();

	closeDecoder();

	if(socket != NULL)
	{
		socket->close();
		delete socket;
	}

	if(recordingSocket != NULL)
	{
		recordingSocket->close();
		delete recordingSocket;
	}

	delete[] _receivedDataBuffer;
	if(_reconstructionFrameBuffer != NULL)
	{
		delete[] _reconstructionFrameBuffer;
	}
	for(int i = 0; i < _allocatedFrames; i++)
	{
		delete[] _rawFrame[i];
	}
	_rawFrame.clear();

	delete[] _recording_receivedDataBuffer;
	if(_recording_reconstructionFrameBuffer != NULL)
	{
		delete[] _recording_reconstructionFrameBuffer;
	}
	for(int i = 0; i < _recording_allocatedFrames; i++)
	{
		delete[] _recording_rawFrame[i];
	}
	_recording_rawFrame.clear();
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
		for(unsigned int i = 0; i < _recording_rawFrame.size(); i++)
		{
			AVPacket recording_packet;
			av_init_packet(&recording_packet);

			recording_packet.stream_index = 0; // First stream as there's only one stream
			recording_packet.data = (unsigned char *) _recording_rawFrame[i];
			recording_packet.size = _recording_frame_size;

			int ret_code = av_interleaved_write_frame(_recording_ctx, &recording_packet);

			if(ret_code != 0)
			{
				cout << "Error while writing video frame: " << ret_code << endl;
			}
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
	static int reconstructed_frame_write_position = -1; // When a frame is split over multiple packets, this is the position to continue writing to when the next packet arrives
	int readOffset = 0;
	_availableFrames = 0;

	while(_status == PROCESSING);

	_status = PROCESSING;

	if(!error)
	{
		// Check if there's data belonging to a previous frame at the beginning of the received buffer
		if(!frameHasPaVE(_receivedDataBuffer) && reconstructed_frame_write_position >= 0)
		{
			if(_reconstructionFrameBuffer != NULL && _reconstructionPaVE != NULL)
			{
				// Received data contains a part of a previous (not completely received) frame
				if(_reconstructionPaVE->payload_size - reconstructed_frame_write_position > bytes_transferred)
				{
					// There's even more data than in this packet, so copy everything into the frame reconstruction buffer
					copy(_receivedDataBuffer, _receivedDataBuffer + bytes_transferred, _reconstructionFrameBuffer + reconstructed_frame_write_position);
					reconstructed_frame_write_position += bytes_transferred;
					readOffset += bytes_transferred;
				}
				else
				{
					_pave.push_back(_reconstructionPaVE);

					// Copy the missing part of a partial frame into the reconstruction buffer
					copy(_receivedDataBuffer, _receivedDataBuffer + (_reconstructionPaVE->payload_size - reconstructed_frame_write_position), _reconstructionFrameBuffer + reconstructed_frame_write_position);

					// Complete frame available
					if(_allocatedFrames < _availableFrames + 1) // Check whether allocating a new framebuffer is needed
					{
						_rawFrame.push_back(new char[BUFFER_MAX_SIZE]); // Allocate a new buffer
						_allocatedFrames++;
					}

					// Copy frame into it's buffer
					copy(_reconstructionFrameBuffer, _reconstructionFrameBuffer + _reconstructionPaVE->payload_size, _rawFrame[_availableFrames]);

					reconstructed_frame_write_position = -1; // Set this to a negative value to make clear there's nothing that has to be reconstructed anymore
					readOffset +=_reconstructionPaVE->payload_size - reconstructed_frame_write_position;

					_availableFrames++;
				}
			}
			else
			{
				cerr << "Could not reconstruct partial frame: reconstruction buffer not initialized!" << endl;
			}
		}

		bool moreFrames = false;

		if(readOffset < (int) bytes_transferred)
		{
			moreFrames = frameHasPaVE(_receivedDataBuffer, readOffset);
		}

		while(moreFrames)
		{
			// Get the PaVE header (contains information about the frame)
			PaVE *currentHeader = parsePaVE(_receivedDataBuffer, readOffset);

			if(currentHeader == NULL)
			{
				// Unable to parse PaVE, can not really happen
				_status = READY;
				startReceive();
				cerr << "Could not parse PaVE" << endl;
				return;
			}

			if(bytes_transferred - readOffset >= currentHeader->payload_size + currentHeader->header_size)
			{
				_pave.push_back(currentHeader);

				// Complete frame available
				if(_allocatedFrames < _availableFrames + 1) // Check whether allocating a new framebuffer is needed
				{
					_rawFrame.push_back(new char[BUFFER_MAX_SIZE]); // Allocate a new buffer
					_allocatedFrames++;
				}

				// Copy frame into it's buffer
				copy(_receivedDataBuffer + readOffset + currentHeader->header_size, _receivedDataBuffer + readOffset + currentHeader->header_size + currentHeader->payload_size, _rawFrame[_availableFrames]);

				readOffset += currentHeader->header_size + currentHeader->payload_size;

				_availableFrames++;
			}
			else
			{
				_reconstructionPaVE = currentHeader;

				// Partial frame available
				if(_reconstructionFrameBuffer == NULL)
				{
					_reconstructionFrameBuffer = new char[BUFFER_MAX_SIZE]; // Allocate the reconstruction buffer
				}

				// Copy partial frame into the buffer
				copy(_receivedDataBuffer + readOffset + currentHeader->header_size, _receivedDataBuffer + bytes_transferred, _reconstructionFrameBuffer);

				reconstructed_frame_write_position = bytes_transferred - readOffset - currentHeader->header_size;
				readOffset = bytes_transferred;
			}

			// Check if there are more frames in the buffer
			if(readOffset < (int) bytes_transferred)
			{
				moreFrames = frameHasPaVE(_receivedDataBuffer, readOffset);
			}
			else
			{
				moreFrames = false;
			}
		}
	}

	// Check if raw frame data could be extracted
	if(_availableFrames > 0)
	{
		for(unsigned int i = 0; i < _pave.size(); i++)
		{
			if(_pave[i]->frame_type == ardrone::video::frame_type::I || _pave[i]->frame_type == ardrone::video::frame_type::IDR)
			{
				_got_first_iframe = true;
				break;
			}
		}

		decodePacket();

		// Clear the PaVE header list
		_pave.clear();
	}
	else if(reconstructed_frame_write_position < 0)
	{
		cerr << "Could not find video data in packet" << endl;
	}

	_decodedPackets++;

	_status = READY;

	// Listen for the next packet
	startReceive();
}

void VideoManager::recording_packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	static int reconstructed_frame_write_position = -1; // When a frame is split over multiple packets, this is the position to continue writing to when the next packet arrives
	int readOffset = 0;
	_recording_availableFrames = 0;

	if((_recording || _start_recording_requested) && !(_stop_recording_requested))
	{
		if(!error)
		{
			// Check if there's data belonging to a previous frame at the beginning of the received buffer
			if(!frameHasPaVE(_recording_receivedDataBuffer) && reconstructed_frame_write_position >= 0)
			{
				if(_recording_reconstructionFrameBuffer != NULL && _recording_reconstructionPaVE != NULL)
				{
					// Received data contains a part of a previous (not completely received) frame
					if(_recording_reconstructionPaVE->payload_size - reconstructed_frame_write_position > bytes_transferred)
					{
						// There's even more data than in this packet, so copy everything into the frame reconstruction buffer
						copy(_recording_receivedDataBuffer, _recording_receivedDataBuffer + bytes_transferred, _recording_reconstructionFrameBuffer + reconstructed_frame_write_position);
						reconstructed_frame_write_position += bytes_transferred;
						readOffset += bytes_transferred;
					}
					else
					{
						_recording_pave.push_back(_recording_reconstructionPaVE);

						// Copy the missing part of a partial frame into the reconstruction buffer
						copy(_recording_receivedDataBuffer, _recording_receivedDataBuffer + (_recording_reconstructionPaVE->payload_size - reconstructed_frame_write_position), _reconstructionFrameBuffer + reconstructed_frame_write_position);

						// Complete frame available
						if(_recording_allocatedFrames < _recording_availableFrames + 1) // Check whether allocating a new framebuffer is needed
						{
							_recording_rawFrame.push_back(new char[BUFFER_MAX_SIZE]); // Allocate a new buffer
							_recording_allocatedFrames++;
						}

						// Copy frame into it's buffer
						copy(_recording_reconstructionFrameBuffer, _recording_reconstructionFrameBuffer + _recording_reconstructionPaVE->payload_size, _recording_rawFrame[_recording_availableFrames]);

						reconstructed_frame_write_position = -1; // Set this to a negative value to make clear there's nothing that has to be reconstructed
						readOffset +=_recording_reconstructionPaVE->payload_size - reconstructed_frame_write_position;

						_recording_availableFrames++;
					}
				}
				else
				{
					cerr << "[Recording] Could not reconstruct partial frame: reconstruction buffer not initialized!" << endl;
				}
			}

			bool moreFrames = false;

			if(readOffset < (int) bytes_transferred)
			{
				moreFrames = frameHasPaVE(_recording_receivedDataBuffer, readOffset);
			}

			while(moreFrames)
			{
				// Get the PaVE header (contains information about the frame)
				PaVE *currentHeader = parsePaVE(_recording_receivedDataBuffer, readOffset);

				if(currentHeader == NULL)
				{
					// Unable to parse PaVE, can not really happen
					_status = READY;
					recording_startReceive();
					cerr << "[Recording] Could not parse PaVE" << endl;
					return;
				}

				if(bytes_transferred - readOffset >= currentHeader->payload_size + currentHeader->header_size)
				{
					_recording_pave.push_back(currentHeader);

					// Complete frame available
					if(_recording_allocatedFrames < _recording_availableFrames + 1) // Check whether allocating a new framebuffer is needed
					{
						_recording_rawFrame.push_back(new char[BUFFER_MAX_SIZE]); // Allocate a new buffer
						_recording_allocatedFrames++;
					}

					// Copy frame into it's buffer
					copy(_recording_receivedDataBuffer + readOffset + currentHeader->header_size, _recording_receivedDataBuffer + readOffset + currentHeader->header_size + currentHeader->payload_size, _recording_rawFrame[_recording_availableFrames]);

					readOffset += currentHeader->header_size + currentHeader->payload_size;

					_recording_availableFrames++;
				}
				else
				{
					_recording_reconstructionPaVE = currentHeader;

					// Partial frame available
					if(_recording_reconstructionFrameBuffer == NULL)
					{
						_recording_reconstructionFrameBuffer = new char[BUFFER_MAX_SIZE]; // Allocate the reconstruction buffer
					}

					// Copy partial frame into the buffer
					copy(_recording_receivedDataBuffer + readOffset + currentHeader->header_size, _recording_receivedDataBuffer + bytes_transferred, _recording_reconstructionFrameBuffer);

					recording_reconstructed_frame_write_position = bytes_transferred - readOffset - currentHeader->header_size;
					readOffset = bytes_transferred;
				}

				// Check if there are more frames in the buffer
				if(readOffset < (int) bytes_transferred)
				{
					moreFrames = frameHasPaVE(_recording_receivedDataBuffer, readOffset);
				}
				else
				{
					moreFrames = false;
				}
			}
		}

		// Check if raw frame data could be extracted
		if(_recording_availableFrames > 0)
		{
			for(unsigned int i = 0; i < _recording_pave.size(); i++)
			{
				if(_recording_pave[i]->frame_type == ardrone::video::frame_type::I || _recording_pave[i]->frame_type == ardrone::video::frame_type::IDR)
				{
					_recording_got_first_iframe = true;
					break;
				}
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

			// Clear the PaVE header list
			_recording_pave.clear();
		}
		else if(reconstructed_frame_write_position < 0)
		{
			cerr << "[Recording] Could not find video data in packet" << endl;
		}

		_recording_decodedPackets++;
	}
	else if(_stop_recording_requested)
	{
		_recording = false;
		_stop_recording_requested = false;
	}

	// Listen for the next packet
	recording_startReceive();
}

/*void VideoManager::recording_packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
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
}*/

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
	if(p->signature[0] == 'P' && p->signature[1] == 'a' && p->signature[2] == 'V' && p->signature[3] == 'E')
	{
		return p;
	}
	else
	{
		return NULL;
	}
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

	for(int i = 0; i < _availableFrames; i++)
	{
		if(initializationNeeded || (_pave[i]->encoded_stream_width != _previous_width))
		{
			cout << "Initializing buffers" << endl;

			cfg.pCodecCtxMP4->width = _pave[i]->encoded_stream_width;
			cfg.pCodecCtxMP4->height = _pave[i]->encoded_stream_height;
			cfg.pCodecCtxH264->width = _pave[i]->encoded_stream_width;
			cfg.pCodecCtxH264->height = _pave[i]->encoded_stream_height;

			cfg.destPicture.width = _pave[i]->display_width;
			cfg.destPicture.height = _pave[i]->display_height;
			cfg.destPicture.format = PIX_FMT_BGR24;

			_decode_buffer_size = avpicture_get_size(cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);
			_decode_buffer = (uint8_t *) av_realloc(_decode_buffer, _decode_buffer_size * sizeof(uint8_t));

			avpicture_fill((AVPicture *) cfg.pFrameOutput, _decode_buffer, cfg.destPicture.format, cfg.destPicture.width, cfg.destPicture.height);

			cfg.swsCtx = sws_getCachedContext(cfg.swsCtx, _pave[i]->display_width, _pave[i]->display_height,
											  cfg.pCodecCtxH264->pix_fmt, _pave[0]->display_width, _pave[i]->display_height,
											  cfg.destPicture.format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		}

		if(_got_first_iframe) // Wait for I-frame
		{
			_packet.data = (unsigned char *) _rawFrame[i];
			_packet.size = _pave[i]->payload_size;

			// Try to decode the packet
			int frameFinished = 0;

			if(_pave[i]->video_codec == ardrone::video::codec::MPEG4_VISUAL)
			{
				avcodec_decode_video2(cfg.pCodecCtxMP4, cfg.pFrame, &frameFinished, &_packet);
			}
			else if(_pave[i]->video_codec ==  ardrone::video::codec::MPEG4_AVC)
			{
				avcodec_decode_video2(cfg.pCodecCtxH264, cfg.pFrame, &frameFinished, &_packet);
			}

			if(frameFinished)
			{
				// Frame successfully decoded :)
				cfg.pFrameOutput->data[0] = _decode_buffer;

				sws_scale(cfg.swsCtx, (const uint8_t *const*)cfg.pFrame->data, cfg.pFrame->linesize, 0,
						  _pave[0]->display_height, cfg.pFrameOutput->data, cfg.pFrameOutput->linesize);

				_frame = cv::Mat(_pave[i]->display_height, _pave[i]->display_width, CV_8UC3, _decode_buffer);
			}
			else
			{
				cerr << "Could not decode frame: Error #" << frameFinished << endl;
			}

			_previous_width = _pave[i]->encoded_stream_width;
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
