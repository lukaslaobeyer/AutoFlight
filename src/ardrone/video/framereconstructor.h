#ifndef FRAMERECONSTRUCTOR_H
#define FRAMERECONSTRUCTOR_H

#define BUFFER_MAX_SIZE 204800

#include "pave.h"

/* FrameReconstructor Usage
 ***************************
 *
 * 1.  Pass a received data buffer containing frame data and the size of the packet into the 'decodePacket' function.
 * 1.1    If true is returned, proceed.
 * 1.2    If false is returned, no frame data could be found. Abort and wait for the next packet.
 * 2.  Check if a complete frame could successfully be extracted from the packet with 'frameAvailable'.
 * 2.1    If true, call the 'getNextFrame' and 'getNextHeader' functions to get a pointer to the data buffer containing the
 *        frame and the frame's header with information. The frame should now be decoded. Then proceed.
 * 2.2    If false, the packet only contained a partial frame. Wait for the next packet and pass it to
 *        decodePacket. Then, repeat from step 2.
 * 3.  Check if there are more frames in the current packet with 'moreFramesInPacket'.
 * 3.1    If true is returned, repeat from step 1, passing the same received data into the function again, but incrementing the
 *        offset parameter by the amount of bytes already decoded ('getOffset' can be used for that).
 * 3.2    If false is returned, all available frame data has been extracted. You are done with this packet. Wait for a new
 *        packet and repeat from step 1.
 *
 */

class FrameReconstructor
{
	public:
		FrameReconstructor(unsigned int bufferSize = BUFFER_MAX_SIZE);
		~FrameReconstructor();

		bool decodePacket(char *data, unsigned int bytes_transferred, unsigned int offset = 0);
		bool moreFramesInPacket();
		bool frameAvailable();
		unsigned int getFrameSize();
		unsigned int getHeaderSize();
		char *getNextFrame();
		PaVE *getNextHeader();
		unsigned int getOffset();

	private:
		char *frameBuffer = nullptr;         // Buffer to which the frames are written
		PaVE currentHeader;
		unsigned int frameSize = 0;          // Size of the current frame's video data
		unsigned int headerSize = 0;         // Size of the current frame's PaVE header
		bool moreDataAvailable = false;      // More than one frame in the received packet
		bool completeFrameAvailable = false; // Completely reconstructed frame available in the buffer
		bool partialFrame = false;           // Partial frame available
		unsigned int partialFrameOffset = 0; // When a partial frame is received, this is the position at which decodePacket should continue writing to the buffer
		                                     // when another packet is available
		unsigned int _offset = 0;            // The offset returned by getOffset
};

#endif
