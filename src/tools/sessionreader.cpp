#include "sessionreader.h"
#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"
#include "../autoflight.h"

using namespace std;

RecordedEvent::RecordedEvent(string type, string time, long long relativeTime, string content)
{
	_type = type;
	_time = time;
	_relativeTime = relativeTime;
	_content = content;
}

RecordedEvent::~RecordedEvent()
{

}

string RecordedEvent::getType()
{
	return _type;
}

string RecordedEvent::getContent()
{
	return _content;
}

boost::posix_time::ptime RecordedEvent::getTime()
{
	boost::posix_time::time_input_facet *tif = new boost::posix_time::time_input_facet;
	tif->set_iso_extended_format();
	std::locale locale(std::locale::classic(), tif);

	boost::posix_time::ptime time;

	istringstream is(_time);
	is.imbue(locale);
	is >> time;

	return time;
}

long long RecordedEvent::getTimeFromStart()
{
	return _relativeTime;
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

	long long startTime = 0;

	for(pugi::xml_node node : _root.children())
	{
		if(string(node.name()) == string("af:event"))
		{
			// Calculate the time from the start to the current node
			// Warning: This rudimentary method may be problematic if the same flight is split over two months, say you start flying
			//          at 23:53 on September 30th and stop on October 1st at 00:06

			long nodeTime = 0;

			string startTimeStr = node.attribute("time").value();
			int day, hour, minute, second, millisecond;
			day = boost::lexical_cast<int>(startTimeStr.substr(8, 2));
			hour = boost::lexical_cast<int>(startTimeStr.substr(11, 2));
			minute = boost::lexical_cast<int>(startTimeStr.substr(14, 2));
			second = boost::lexical_cast<int>(startTimeStr.substr(17, 2));
			millisecond = boost::lexical_cast<int>(startTimeStr.substr(20, 3));

			long long time = millisecond
						   + second * 1000
						   + minute * 1000 * 60
						   + hour   * 1000 * 60 * 60
						   + day    * 1000 * 60 * 60 * 24;

			if(string(node.attribute("type").value()) == "ProgramStart")
			{
				startTime = time;
			}
			else
			{
				nodeTime = time;
			}

			RecordedEvent e(node.attribute("type").value(), node.attribute("time").value(), nodeTime - startTime, node.text().get());
			events.push_back(e);
		}
	}

	return events;
}
