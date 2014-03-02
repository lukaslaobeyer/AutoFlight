#include "controllerconfigurationfileio.h"
#include <Gamepad.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "../autoflight.h"

using namespace std;

bool ControllerConfigurationFileIO::saveControllerConfiguration(ControllerConfiguration *cc)
{
	pugi::xml_document doc;

	pugi::xml_node root = doc.append_child("af:controllerconfig");
	pugi::xml_attribute xmlns = root.append_attribute("xmlns:af");
	xmlns.set_value("http://lbpclabs.com/autoflight");

	addControllerConfigNode(root, "height", cc->height);
	addControllerConfigNode(root, "yaw", cc->yaw);
	addControllerConfigNode(root, "pitch", cc->pitch);
	addControllerConfigNode(root, "roll", cc->roll);

	addControllerConfigNode(root, "heightP", cc->heightP);
	addControllerConfigNode(root, "yawP", cc->yawP);
	addControllerConfigNode(root, "pitchP", cc->pitchP);
	addControllerConfigNode(root, "rollP", cc->rollP);

	addControllerConfigNode(root, "heightM", cc->heightM);
	addControllerConfigNode(root, "yawM", cc->yawM);
	addControllerConfigNode(root, "pitchM", cc->pitchM);
	addControllerConfigNode(root, "rollM", cc->rollM);

	addControllerConfigNode(root, "takeoff", cc->takeoff);
	addControllerConfigNode(root, "land", cc->land);
	addControllerConfigNode(root, "emergency", cc->emergency);
	addControllerConfigNode(root, "recording", cc->recording);
	addControllerConfigNode(root, "photo", cc->photo);
	addControllerConfigNode(root, "flip", cc->flip);
	addControllerConfigNode(root, "zap", cc->zap);
	addControllerConfigNode(root, "slow", cc->slow);

	string device = boost::lexical_cast<std::string>(cc->vendorID) + "." + boost::lexical_cast<std::string>(cc->productID);
	string path = AutoFlight::getProgramDirectory() + "controllerconfig_" + device + ".xml";
	doc.save_file(path.c_str());

	return false;
}

ControllerConfiguration *ControllerConfigurationFileIO::loadControllerConfiguration()
{
	Gamepad_detectDevices();

	int numDev = Gamepad_numDevices();
	for(int i = 0; i < numDev; i++)
	{
		if(Gamepad_deviceAtIndex(i) != NULL)
		{
			int vendorID = Gamepad_deviceAtIndex(i)->vendorID;
			int productID = Gamepad_deviceAtIndex(i)->productID;
			int deviceID = Gamepad_deviceAtIndex(i)->deviceID;

			ControllerConfiguration *cc = loadControllerConfiguration(vendorID, productID, deviceID);

			if(cc != nullptr)
			{
				return cc;
			}
		}
	}

	return nullptr;
}

ControllerConfiguration *ControllerConfigurationFileIO::loadControllerConfiguration(int vendorID, int productID, int deviceID)
{
	string device = boost::lexical_cast<std::string>(vendorID) + "." + boost::lexical_cast<std::string>(productID);
	string path = AutoFlight::getProgramDirectory() + "controllerconfig_" + device + ".xml";

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());

	if(result.status != pugi::status_ok)
	{
		cout << "[WARNING] Could not find/parse controller configuration file." << endl;
		return nullptr;
	}

	pugi::xml_node root = doc.root().first_child();

	ControllerConfiguration *cc = new ControllerConfiguration;

	cc->vendorID = vendorID;
	cc->productID = productID;
	cc->deviceID = deviceID;

	for(pugi::xml_node child = root.first_child(); child; child = child.next_sibling())
	{
		if(strcmp(child.name(), "af:cconfigkey") != 0 || child.attribute("key").as_string()[0] == '\0' || child.attribute("value").as_string()[0] == '\0')
		{
			cerr << "[ERROR] Invalid controller configuration file (" << path << ")." << endl;
			delete cc;
			cc = nullptr;
			return nullptr;
		}

		string key = child.attribute("key").as_string();
		int value = child.attribute("value").as_int();

		if(key == "height")
		{
			cc->height = value;
		}
		else if(key == "heightP")
		{
			cc->heightP = value;
		}
		else if(key == "heightM")
		{
			cc->heightM = value;
		}
		else if(key == "yaw")
		{
			cc->yaw = value;
		}
		else if(key == "yawP")
		{
			cc->yawP = value;
		}
		else if(key == "yawM")
		{
			cc->yawM = value;
		}
		else if(key == "pitch")
		{
			cc->pitch = value;
		}
		else if(key == "pitchP")
		{
			cc->pitchP = value;
		}
		else if(key == "pitchM")
		{
			cc->pitchM = value;
		}
		else if(key == "roll")
		{
			cc->roll = value;
		}
		else if(key == "rollP")
		{
			cc->rollP = value;
		}
		else if(key == "rollM")
		{
			cc->rollM = value;
		}
		else if(key == "takeoff")
		{
			cc->takeoff = value;
		}
		else if(key == "land")
		{
			cc->land = value;
		}
		else if(key == "emergency")
		{
			cc->emergency = value;
		}
		else if(key == "recording")
		{
			cc->recording = value;
		}
		else if(key == "photo")
		{
			cc->photo = value;
		}
		else if(key == "flip")
		{
			cc->flip = value;
		}
		else if(key == "zap")
		{
			cc->zap = value;
		}
		else if(key == "slow")
		{
			cc->slow = value;
		}
	}

	return cc;
}

void ControllerConfigurationFileIO::addControllerConfigNode(pugi::xml_node root, string name, int value)
{
	pugi::xml_node event = root.append_child("af:cconfigkey");

	pugi::xml_attribute key_attr = event.append_attribute("key");
	key_attr.set_value(name.c_str());

	pugi::xml_attribute value_attr = event.append_attribute("value");
	value_attr.set_value(value);
}
