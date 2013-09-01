#ifndef SESSIONRECORDER_H
#define SESSIONRECORDER_H

#include <string>
#include <pugixml/pugixml.hpp>

class SessionRecorder
{
	public:
		SessionRecorder(pugi::xml_document &doc);
		~SessionRecorder();

		void addEvent(std::string eventType, std::string content = "");
	private:
		pugi::xml_node _root;
};

#endif
