#ifndef ICONNECTIONSTATUSLISTENER_H
#define ICONNECTIONSTATUSLISTENER_H

class IConnectionStatusListener
{
	public:
        	virtual ~IConnectionStatusListener() {}
        	virtual void connectionEstablished() = 0;
        	virtual void connectionLost() = 0;
};

#endif
