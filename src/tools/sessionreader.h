#ifndef SESSIONREADER_H
#define SESSIONREADER_H

#include <string>
#include <vector>
#include <pugixml/pugixml.hpp>
#include <boost/date_time.hpp>

class RecordedEvent
{
	public:
		RecordedEvent(pugi::xml_node eventNode);
		~RecordedEvent();

	private:
		pugi::xml_node _eventNode;

		std::string getType();
		boost::posix_time::ptime getTime();
		std::string getContent();
};

class SessionReader
{
	public:
		SessionReader();
		~SessionReader();

		bool readSession(std::string path);

		static std::vector<std::string> getSessionSaves();
	private:
		pugi::xml_node _root;

		std::vector<RecordedEvent> getEvents();
};

#endif
