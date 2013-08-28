#ifndef QNAVDATALISTENER_H
#define QNAVDATALISTENER_H

#include "navdatakeys.h"

class QNavdataListener
{
	public:
        	virtual ~QNavdataListener() {}
    public:
        	virtual void navdataAvailable(AFNavdata *navdata) = 0; // Must be implemented as slot
};

#endif
