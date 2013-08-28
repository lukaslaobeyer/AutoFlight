#ifndef INAVDATALISTENER_H
#define INAVDATALISTENER_H

#include "navdatakeys.h"

class INavdataListener
{
	public:
        	virtual ~INavdataListener() {}
        	virtual void navdataAvailable(AFNavdata *navdata) = 0;
};

#endif
