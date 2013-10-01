#ifndef ICONTROLLERINPUTLISTENER_H
#define ICONTROLLERINPUTLISTENER_H

#include "controllerinput.h"

class IControllerInputListener
{
	public:
        	virtual ~IControllerInputListener() {}
        	virtual void controllerInputAvailable(ControllerInput *in) = 0;
};

#endif
