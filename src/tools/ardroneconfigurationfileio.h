#ifndef ARDRONECONFIGURATIONFILEIO_H
#define ARDRONECONFIGURATIONFILEIO_H

#include <pugixml/pugixml.hpp>
#include <string>
#include "../ardrone/ardroneconfiguration.h"

namespace ARDroneConfigurationFileIO
{
	bool saveARDroneConfiguration(ARDroneConfiguration *cc, int index);
	ARDroneConfiguration *loadARDroneConfiguration(int index);
	void addARDroneConfigNode(pugi::xml_node &root, std::string name, std::string value);
};

#endif
