#ifndef CONTROLLINK_H
#define CONTROLLINK_H

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <stdexcept>
#include "atcommands/atcommand.h"

class ControlLinkNotInitializedException : public std::runtime_error
{
	public:
		ControlLinkNotInitializedException() : std::runtime_error("Control Link not initialized!") { }
};

class ControlLink
{	
	public:
		explicit ControlLink();
		~ControlLink();
		void init(std::string ip, boost::asio::io_service &io_service);
		void sendATCommands(std::vector<ATCommand> cmds);
		void setAppID();
		void close();
	private:
		boost::asio::ip::udp::socket *socket = NULL;
		int seqNum = 1; // AT Command sequence number
};

#endif
