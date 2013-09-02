#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}
#include "codecconfig.h"
#include "pave.h"

#define BUFFER_MAX_SIZE 204800
#define READY 3
#define PROCESSING 4
#define WRITING_FRAME 5

class CouldNotInitializeDecoderException : public std::runtime_error
{
	public:
		CouldNotInitializeDecoderException() : std::runtime_error("Could not initialize video decoder!") { }
};

class VideoManager
{	
	public:
		explicit VideoManager();
		~VideoManager();
		void init(std::string ip, boost::asio::io_service &io_service); // Always call this before using other methods
		void update(); // Process available packets, call this regularily (>60 times per second)
		cv::Mat getVideoFrame();
		bool takePicture(std::string savePath);
		bool startRecording(std::string savePath);
		bool stopRecording();
		void close();
	private:
		void startReceive();
		void recording_startReceive();
		void packetReceived(const boost::system::error_code &error, size_t bytes_transferred);
		void recording_packetReceived(const boost::system::error_code &error, size_t bytes_transferred);
		void recording_writeFrame();
		PaVE *parsePaVE(char *frame, unsigned int offset = 0);
		bool frameHasPaVE(char *frame, unsigned int offset = 0);
		void decodePacket();
		bool initializeDecoder();
		void closeDecoder();

		bool decoderInitialized = false;
		CodecConfig cfg;
		boost::asio::ip::tcp::socket *socket = NULL;
		cv::Mat _frame;
		int _previous_width = -1; // Width of the previous received frame, needed to see if the size changed
		int _allocatedFrames = 0;
		char *_receivedDataBuffer = NULL;
		int _availableFrames = 0;
		std::vector<char *> _rawFrame;
		std::vector<PaVE *> _pave;
		char *_reconstructionFrameBuffer = NULL;
		PaVE *_reconstructionPaVE = NULL;
		unsigned int _recording_frame_size = -1;
		AVPacket _packet; // Received data is stored into this, and then decoded.
		bool _got_first_iframe = false; // Needed so we don't feed P-frames to the decoder before we have an I-frame
		bool _recording_got_first_iframe = false;
		uint8_t *_decode_buffer = NULL;
		int _decode_buffer_size;
		int _status = READY; // Either READY or PROCESSING (defined above)
		unsigned long _decodedPackets = 0;

		bool _start_recording_requested = false;
		bool _stop_recording_requested = false;
		bool _recording = false;
		int _recording_status;

		AVFormatContext *_recording_ctx = NULL;
		boost::asio::ip::tcp::socket *recordingSocket = NULL;
		char _recording_receivedDataBuffer[BUFFER_MAX_SIZE];
		char _recording_rawFrame[BUFFER_MAX_SIZE];
		PaVE _recording_pave;
		bool recording_frame_ready = false;
		int recording_reconstructed_frame_write_position = -1;
};

#endif
