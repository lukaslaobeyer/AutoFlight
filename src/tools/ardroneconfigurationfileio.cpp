#include "ardroneconfigurationfileio.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "../autoflight.h"

using namespace std;

bool ARDroneConfigurationFileIO::saveARDroneConfiguration(ARDroneConfiguration *ardc, int index)
{
	pugi::xml_document doc;

	pugi::xml_node root = doc.append_child("af:ardroneconfig");
	pugi::xml_attribute xmlns = root.append_attribute("xmlns:af");
	xmlns.set_value("http://lbpclabs.com/autoflight");

	addARDroneConfigNode(root, "altitude_max", boost::lexical_cast<string>(ardc->altitude_max));
	addARDroneConfigNode(root, "no_hull", boost::lexical_cast<string>(ardc->no_hull));
	addARDroneConfigNode(root, "outdoor_flight", boost::lexical_cast<string>(ardc->outdoor_flight));
	addARDroneConfigNode(root, "pitch_roll_max", boost::lexical_cast<string>(ardc->pitch_roll_max));
	addARDroneConfigNode(root, "vertical_speed_max", boost::lexical_cast<string>(ardc->vertical_speed_max));
	addARDroneConfigNode(root, "yaw_speed_max", boost::lexical_cast<string>(ardc->yaw_speed_max));

	string path = AutoFlight::getProgramDirectory() + "ardroneconfig_" + boost::lexical_cast<string>(index) + ".xml";
	doc.save_file(path.c_str());

	return false;
}

ARDroneConfiguration *ARDroneConfigurationFileIO::loadARDroneConfiguration(int index)
{
	string path = AutoFlight::getProgramDirectory() + "ardroneconfig_" + boost::lexical_cast<string>(index) + ".xml";

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());

	if(result.status != pugi::status_ok)
	{
		cout << "[WARNING] Could not find/parse AR.Drone configuration file." << endl;
		return nullptr;
	}

	pugi::xml_node root = doc.root().first_child();

	ARDroneConfiguration *ardc = new ARDroneConfiguration;

	for(pugi::xml_node child = root.first_child(); child; child = child.next_sibling())
	{
		if(strcmp(child.name(), "af:ardconfigkey") != 0 || child.attribute("key").as_string()[0] == '\0' || child.attribute("value").as_string()[0] == '\0')
		{
			cerr << "[ERROR] Invalid AR.Drone configuration file (" << path << ")." << endl;
			return nullptr;
		}

		string key = child.attribute("key").as_string();
		string value_str = child.attribute("value").as_string();
		float value;
		try
		{
			stringstream value_strstr(value_str);
			value_strstr.imbue(locale("C"));
			value_strstr >> value;
		}
		catch(exception &e)
		{
			cout << e.what() << endl;
			return nullptr;
		}

		if(key == "altitude_max")
		{
			ardc->altitude_max = value;
		}
		else if(key == "no_hull")
		{
			ardc->no_hull = value;
		}
		else if(key == "outdoor_flight")
		{
			ardc->outdoor_flight = value;
		}
		else if(key == "pitch_roll_max")
		{
			ardc->pitch_roll_max = value;
		}
		else if(key == "vertical_speed_max")
		{
			ardc->vertical_speed_max = value;
		}
		else if(key == "yaw_speed_max")
		{
			ardc->yaw_speed_max = value;
		}
	}

	return ardc;
}

void ARDroneConfigurationFileIO::addARDroneConfigNode(pugi::xml_node &root, string name, string value)
{
	replace(value.begin(), value.end(), ',', '.'); // Decimal separator needs to be a dot (boost lexical_cast seems to recognize the current locale)

	pugi::xml_node event = root.append_child("af:ardconfigkey");

	pugi::xml_attribute key_attr = event.append_attribute("key");
	key_attr.set_value(name.c_str());

	pugi::xml_attribute value_attr = event.append_attribute("value");
	value_attr.set_value(value.c_str());
}
