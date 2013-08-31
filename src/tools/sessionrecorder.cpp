#include "sessionrecorder.h"
#include "boost/date_time.hpp"
#include "../autoflight.h"

using namespace std;

SessionRecorder::SessionRecorder(pugi::xml_document &doc)
{
	_root = doc.append_child("af:session");
}

SessionRecorder::~SessionRecorder()
{

}

void SessionRecorder::addEvent(string eventName)
{
	string timestamp;
	const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	timestamp = boost::posix_time::to_iso_extended_string(now);

	pugi::xml_node event = _root.append_child("af:event");

	pugi::xml_attribute time_attr = event.append_attribute("time");
	time_attr.set_value(timestamp.c_str());

	pugi::xml_attribute eventID = event.append_attribute("id");
	eventID.set_value(eventName.c_str());
}
