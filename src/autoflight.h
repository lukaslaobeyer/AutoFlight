#ifndef AUTOFLIGHT_H
#define AUTOFLIGHT_H

#include <string>
#include "ardrone/ardrone.h"
#include "autoscript/asengine.h"

class AutoFlight
{
	public:
		AutoFlight();
		~AutoFlight();

		bool attemptConnectionToDrone();
		ARDrone *ardrone();
		ASEngine *asengine();

		static std::string getProgramDirectory(); // Returns the directory where the AutoFlight executable is (needed to find support files)
		static std::string getHomeDirectory();
	private:
		ARDrone *_drone = NULL;
		ASEngine *_ase = NULL;
};

#endif
