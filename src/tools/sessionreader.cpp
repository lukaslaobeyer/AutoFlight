#include "sessionreader.h"
#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"
#include "../autoflight.h"

using namespace std;

RecordedEvent::RecordedEvent(pugi::xml_node eventNode)
{
	_eventNode = eventNode;
}

RecordedEvent::~RecordedEvent()
{

}

string RecordedEvent::getType()
{
	string type = _eventNode.attribute("type").as_string();
	return type;
}

string RecordedEvent::getContent()
{
	string content = _eventNode.value();
	return content;
}

boost::posix_time::ptime RecordedEvent::getTime()
{
	string timestr = _eventNode.attribute("time").as_string();
	boost::posix_time::ptime time = boost::posix_time::from_iso_string(timestr);
	return time;
}


SessionReader::SessionReader()
{

}

SessionReader::~SessionReader()
{

}

bool SessionReader::readSession(std::string path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result parseres = doc.load_file(path.c_str());
	if(parseres.status != pugi::status_ok)
	{
		return false;
	}
	_root = doc.child("af:session");
	return true;
}

vector<string> SessionReader::getSessionSaves()
{
	vector<string> files;

	string sessionSaveDir = AutoFlight::getHomeDirectory() + "AutoFlightSaves/Sessions";
	if(boost::filesystem::exists(sessionSaveDir))
	{
		boost::filesystem::directory_iterator iterator(sessionSaveDir);
		for(; iterator != boost::filesystem::directory_iterator(); ++iterator)
		{
			files.push_back(iterator->path().filename().string());
		}

		return files;
	}
	else
	{
		return files;
	}
}

vector<RecordedEvent> SessionReader::getEvents()
{
	vector<RecordedEvent> events;

	for(pugi::xml_node node = _root.first_child(); node; node = node.next_sibling())
	{
		events.push_back(node);
	}

	return events;
}
