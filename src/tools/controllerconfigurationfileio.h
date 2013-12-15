#ifndef CONTROLLERCONFIGURATIONFILEIO_H
#define CONTROLLERCONFIGURATIONFILEIO_H

#include <pugixml/pugixml.hpp>
#include <string>
#include "../ardrone/input/controllerconfiguration.h"

namespace ControllerConfigurationFileIO
{
	bool saveControllerConfiguration(ControllerConfiguration *cc);
	ControllerConfiguration *loadControllerConfiguration();
	ControllerConfiguration *loadControllerConfiguration(int vendorID, int productID, int deviceID);
	void addControllerConfigNode(pugi::xml_node root, std::string name, int value);
};

#endif
