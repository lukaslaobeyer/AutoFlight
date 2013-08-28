#ifndef QVIDEOLISTENER_H
#define QVIDEOLISTENER_H

#include <QImage>

class QVideoListener
{
	public:
        	virtual ~QVideoListener() {}
    public:
        	virtual void videoFrameAvailable(QImage frame) = 0; // Must be implemented as slot
};

#endif
