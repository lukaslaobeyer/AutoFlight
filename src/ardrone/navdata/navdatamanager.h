#ifndef NAVDATAMANAGER_H
#define NAVDATAMANAGER_H

#include <boost/asio.hpp>
#include "navdatakeys.h"

class NavdataManager
{	
	public:
		explicit NavdataManager();
		~NavdataManager();
		void init(std::string ip, boost::asio::io_service &io_service); // Always call this before using other methods
		bool isConnected();
		void update(); // Process available packets, call this regularily (>60 times per second)
		AFNavdata *getNavdata();
		void close();
	private:
		void startReceive();
		void packetReceived(const boost::system::error_code &error, std::size_t bytes_transferred);
		bool parseNavdata(char data[], int receivedbytes);
		
		boost::asio::ip::udp::socket *socket = NULL;
		boost::asio::ip::udp::endpoint sender_endpoint;
		bool navdataAvailable = false;
		AFNavdata navdata;
		uint32_t last_seqNum = 0;
		char _receivedDataBuffer[2048];
};

#endif
