#ifndef AUTOFLIGHT_H
#define AUTOFLIGHT_H

#include <string>
#include <pugixml/pugixml.hpp>
#ifdef __MINGW32__
#include <winsock2.h>
#endif
#include <boost/date_time.hpp>
#include "ardrone/ardrone.h"
#include "autoscript/asengine.h"
#include "tools/sessionrecorder.h"

class AutoFlight
{
	public:
		AutoFlight(std::string drone_ip = ardrone::DEFAULT_IP);
		~AutoFlight();

		bool attemptConnectionToDrone();
		ARDrone *ardrone();
		ASEngine *asengine();
		SessionRecorder *sessionrecorder();

		void saveSession();

		static std::string getProgramDirectory(); // Returns the directory where the AutoFlight executable is (needed to find support files)
		static std::string getHomeDirectory();

		static std::string af_timestamp();
	private:

		ARDrone *_drone = NULL;
		ASEngine *_ase = NULL;
		SessionRecorder *_srec = NULL;

		pugi::xml_document _sessionRecDoc;
};

#endif
