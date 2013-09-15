#ifndef SESSIONREADER_H
#define SESSIONREADER_H

#include <string>
#include <vector>
#include <pugixml/pugixml.hpp>
#include <boost/date_time.hpp>

class RecordedEvent
{
	public:
		RecordedEvent(std::string type, std::string time, long long relativeTime, std::string content);
		~RecordedEvent();

		std::string getType();
		boost::posix_time::ptime getTime();
		std::string getContent();
		long long getTimeFromStart();
	private:
		std::string _type;
		std::string _time;
		std::string _content;
		long long _relativeTime;
};

class SessionReader
{
	public:
		SessionReader();
		~SessionReader();

		bool readSession(std::string path);
		std::vector<RecordedEvent> getEvents();

		static std::vector<std::string> getSessionSaves();
	private:
		pugi::xml_node _root;
};

#endif
