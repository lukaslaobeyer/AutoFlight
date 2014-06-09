#include "framereconstructor.h"
#include <algorithm>
#include <iostream>

using namespace std;

FrameReconstructor::FrameReconstructor(unsigned int bufferSize)
{
	frameBuffer = new char[bufferSize];
}

FrameReconstructor::~FrameReconstructor()
{
	if(frameBuffer != nullptr)
	{
		delete[] frameBuffer;
	}
}

bool FrameReconstructor::decodePacket(char *data, unsigned int bytes_transferred, unsigned int offset)
{
	if(partialFrame && frameSize > 0 && partialFrameOffset > 0)
	{
		// There's a partial frame in the buffer, and the packet passed to this function should
		// contain the remaining data.


		if((frameSize - partialFrameOffset) <= bytes_transferred)
		{
			// All the remaining data is in this packet
			copy(data, data + frameSize - partialFrameOffset, frameBuffer + partialFrameOffset);
			completeFrameAvailable = true;
			partialFrame = false;

			if((frameSize - partialFrameOffset) == bytes_transferred)
			{
				moreDataAvailable = false;
			}
			else
			{
				moreDataAvailable = true;
			}

			_offset = frameSize - partialFrameOffset;
			partialFrameOffset = 0;

			return true;
		}
		else
		{
			// This is just a continuation of the frame data but there's more in the next packet
			copy(data, data + bytes_transferred, frameBuffer + partialFrameOffset);
			partialFrameOffset += bytes_transferred;
			moreDataAvailable = false;

			return true;
		}
	}
	else
	{
		// No partial frame in the buffer that needs to be completed.

		moreDataAvailable = false;
		completeFrameAvailable = false;
		partialFrame = false;

		// Check if there's a header
		if(frameHasPaVE(data, offset))
		{
			// Parse the header
			currentHeader = *parsePaVE(data, offset);

			frameSize = currentHeader.payload_size;
			headerSize = currentHeader.header_size;

			// Check whether there's a partial frame, one complete frame or more than one frame in this packet
			if((offset + frameSize + headerSize) > bytes_transferred)
			{
				if(offset + headerSize < bytes_transferred)
				{
					// Partial frame in this packet
					copy(data + offset + headerSize, data + bytes_transferred, frameBuffer);
					partialFrame = true;
					partialFrameOffset = bytes_transferred - offset - headerSize;

					_offset = 0;

					return true;
				}
				else
				{
					// TODO: ???
					cout << "Offset: " << offset << "; Frame size: " << frameSize << "; Header size: " << headerSize << "; Transferred: " << bytes_transferred << endl;
					return false;
				}
			}
			else if((offset + frameSize + headerSize) < bytes_transferred)
			{
				// More than one frame in this packet
				copy(data + offset + headerSize, data + offset + headerSize + frameSize, frameBuffer);
				completeFrameAvailable = true;
				moreDataAvailable = true;

				_offset = frameSize + headerSize;

				return true;
			}
			else if((offset + frameSize + headerSize) == bytes_transferred)
			{
				// Exactly one frame in this packet
				copy(data + offset + headerSize, data + bytes_transferred, frameBuffer);
				completeFrameAvailable = true;

				_offset = 0;

				return true;
			}
		}
		else
		{
			// There's no header! This should not happen. Reconstructing the packet failed.
			return false;
		}
	}

	return false;
}

bool FrameReconstructor::moreFramesInPacket()
{
	return moreDataAvailable;
}

bool FrameReconstructor::frameAvailable()
{
	return completeFrameAvailable;
}

char *FrameReconstructor::getNextFrame()
{
	return frameBuffer;
}

PaVE *FrameReconstructor::getNextHeader()
{
	return &currentHeader;
}

unsigned int FrameReconstructor::getFrameSize()
{
	return frameSize;
}

unsigned int FrameReconstructor::getHeaderSize()
{
	return headerSize;
}

unsigned int FrameReconstructor::getOffset()
{
	return _offset;
}
