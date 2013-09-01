#include "sessionrecorder.h"
#include "boost/date_time.hpp"
#include "../autoflight.h"

using namespace std;

SessionRecorder::SessionRecorder(pugi::xml_document &doc)
{
	_root = doc.append_child("af:session");
	pugi::xml_attribute xmlns = _root.append_attribute("xmlns:af");
	xmlns.set_value("http://lbpclabs.com/autoflight");
}

SessionRecorder::~SessionRecorder()
{

}

void SessionRecorder::addEvent(string eventType, string content)
{
	string timestamp;
	const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	timestamp = boost::posix_time::to_iso_extended_string(now);

	pugi::xml_node event = _root.append_child("af:event");

	pugi::xml_attribute time_attr = event.append_attribute("time");
	time_attr.set_value(timestamp.c_str());

	pugi::xml_attribute type_attr = event.append_attribute("type");
	type_attr.set_value(eventType.c_str());

	if(content.size() > 0)
	{
		event.set_value(content.c_str());
	}
}
